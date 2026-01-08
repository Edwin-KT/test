#define CROW_MAIN

#include <crow.h>
#include <crow/websocket.h>
#include <crow/json.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "DBManager.h"
#include "PasswordService.h"
#include "Handlers.h"
#include "Config.h"
#include "AutoGameStarter.h"
#include "GameLobby.h"

using json = nlohmann::json;

int main() {
    DBManager::init();
    AutoGameStarter autoStarter;
    crow::SimpleApp app;

    // Auth routes
    CROW_ROUTE(app, "/status")(handleStatus);
    CROW_ROUTE(app, "/register").methods("POST"_method)(handleRegister);
    CROW_ROUTE(app, "/login").methods("POST"_method)(handleLogin);
    CROW_ROUTE(app, "/users")(handleGetUsers);

    // Game lobby routes
    CROW_ROUTE(app, "/joinLobby").methods("POST"_method)(handleJoinLobby);
    CROW_ROUTE(app, "/create_game").methods("POST"_method)(handleCreateGame);
    CROW_ROUTE(app, "/joinByPin").methods("POST"_method)(handleJoinByPin);
    CROW_ROUTE(app, "/lobbyStatus")(handleLobbyStatus);

    // Game control routes
    CROW_ROUTE(app, "/games/<int>/state")(handleGetGameState);
    CROW_ROUTE(app, "/games/<int>/playCard").methods("POST"_method)(handlePlayCard);
    CROW_ROUTE(app, "/games/<int>/endTurn").methods("POST"_method)(handleEndTurn);
    CROW_ROUTE(app, "/games/<int>/messages")(handleGetMessages);
    CROW_ROUTE(app, "/games/<int>/sendMessage").methods("POST"_method)(handleSendMessage);

    // WebSocket for real-time gameplay
    CROW_WEBSOCKET_ROUTE(app, "/game")
        .onopen([](crow::websocket::connection& conn) {
        std::cout << "[WS] Client connected\n";
            })
        .onmessage([](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
        // Added 'bool is_binary' above ^
        try {
            auto msg = crow::json::load(data);
            if (!msg) return;

            if (msg.has("action") && msg["action"].s() == "join") {
                std::string username = msg["username"].s();
                int gameId = msg["gameId"].i();

                GameLobby::instance().registerWebSocket(gameId, username, &conn);

                conn.send_text(R"({"action":"joined","status":"ok"})");
            }
        }
        catch (...) {
            conn.send_text(R"({"error":"Invalid JSON"})");
        }
            })
        .onclose([](crow::websocket::connection& conn, const std::string& reason, uint16_t status) {
        // Added 'uint16_t status' above ^
        std::cout << "[WS] Client disconnected: " << reason << " (Status: " << status << ")\n";
            });
           

    // leave-game route
    CROW_ROUTE(app, "/leaveGame")
        .methods("POST"_method)(handleLeaveGame);

    constexpr uint16_t PORT = Config::PORT;
    std::cout << " The Game server started on http://localhost:" << PORT << "/status\n";
    std::cout << " WebSocket available at ws://localhost:" << PORT << "/game\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    app.port(PORT).multithreaded().run();
    return 0;
}
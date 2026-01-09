#include "Handlers.h"
#include "DBManager.h"
#include "GameLobby.h"
#include "PasswordService.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

crow::response handleStatus() {
    crow::json::wvalue res;
    res["status"] = "ok";
    return crow::response(200, res.dump());
}

crow::response handleRegister(const crow::request& req) {
    try {
        auto body = json::parse(req.body);

        if (!body.contains("username") || !body.contains("password")) {
            crow::json::wvalue res;
            res["error"] = "Missing username or password";
            return crow::response(400, res.dump());
        }

        std::string username = body["username"].get<std::string>();
        std::string password = body["password"].get<std::string>();

        std::cout << "[REGISTER] Attempt from username: " << username << std::endl;

        if (username.empty() || username.size() > 20) {
            crow::json::wvalue res;
            res["error"] = "Username must be 1-20 characters";
            return crow::response(400, res.dump());
        }

        if (DBManager::userExists(username)) {
            crow::json::wvalue res;
            res["error"] = "Username already exists";
            return crow::response(409, res.dump());
        }

        auto [hash, salt] = PasswordService::hashPasswordWithSalt(password);
        if (!DBManager::addUser(username, hash, salt)) {
            crow::json::wvalue res;
            res["error"] = "Failed to create user";
            return crow::response(500, res.dump());
        }

        std::cout << "[REGISTER] User created: " << username << std::endl;

        crow::json::wvalue res;
        res["success"] = true;
        res["message"] = "User registered successfully";
        res["username"] = username;
        return crow::response(201, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Invalid JSON format";
        return crow::response(400, res.dump());
    }
}

crow::response handleLogin(const crow::request& req) {
    
    try {
        auto body = json::parse(req.body);

        if (!body.contains("username") || !body.contains("password")) {
            crow::json::wvalue res;
            res["error"] = "Missing username or password";
            return crow::response(400, res.dump());
        }

        std::string username = body["username"].get<std::string>();
        std::string password = body["password"].get<std::string>();

        std::cout << "[LOGIN] Attempt from username: " << username << std::endl;

        auto userOpt = DBManager::getUser(username);
        if (!userOpt.has_value() || !PasswordService::verifyPassword(password, userOpt->passwordHash, userOpt->salt)) {

            std::cout << "[LOGIN] Failed login attempt: " << username << std::endl;

            crow::json::wvalue res;
            res["error"] = "Invalid username or password";
            return crow::response(401, res.dump());
        }

        std::cout << "[LOGIN] Success for username: " << username << std::endl;

        crow::json::wvalue res;
        res["success"] = true;
        res["message"] = "Login successful";
        res["username"] = username;
        return crow::response(200, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Invalid JSON format";
        return crow::response(400, res.dump());
    }
}
crow::response handleCreateGame(const crow::request& req) {
    try {
        auto body = json::parse(req.body);
        if (!body.contains("username") || !body.contains("pin")) {
            crow::json::wvalue res;
            res["error"] = "Missing username or pin";
            return crow::response(400, res.dump());
        }

        std::string username = body["username"];
        std::string pin = body["pin"];

        
        bool isPublic = pin.empty();
        auto gameId = GameLobby::instance().createGame(username, pin, isPublic);

        if (!gameId) {
            crow::json::wvalue res;
            res["error"] = "Failed to create game";
            return crow::response(500, res.dump());
        }

        crow::json::wvalue res;
        res["success"] = true;
        res["gameId"] = *gameId;
        res["isPublic"] = isPublic;
        return crow::response(200, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Invalid JSON format";
        return crow::response(400, res.dump());
    }
}
crow::response handleGetUsers() {
    crow::json::wvalue res;

    auto users = DBManager::getAllUsers(); 

    auto& arr = res["users"];

    for (size_t i = 0; i < users.size(); i++) {
        arr[i]["username"] = users[i].username;
        arr[i]["games_played"] = users[i].gamesPlayed;
        arr[i]["games_won"] = users[i].gamesWon;
        arr[i]["hours_played"] = users[i].hoursPlayed;
        arr[i]["performance_score"] = users[i].performanceScore;
    }

    return crow::response(200, res.dump());
}

crow::response handleJoinLobby(const crow::request& req) {
    try {
        auto body = json::parse(req.body);

        if (!body.contains("username")) {
            crow::json::wvalue res;
            res["error"] = "Missing username";
            return crow::response(400, res.dump());
        }

        std::string username = body["username"].get<std::string>();

        auto gameIdOpt = GameLobby::instance().findRandomGame();

        if (gameIdOpt.has_value()) {
            
            if (!GameLobby::instance().joinGame(*gameIdOpt, username)) {
                crow::json::wvalue res;
                res["error"] = "Failed to join existing game";
                return crow::response(409, res.dump());
            }

            crow::json::wvalue res;
            res["success"] = true;
            res["gameId"] = *gameIdOpt;
            res["joined"] = true;
            return crow::response(200, res.dump());
        }

        auto newGameId = GameLobby::instance().createGame(username);
        if (!newGameId.has_value()) {
            crow::json::wvalue res;
            res["error"] = "Failed to create game";
            return crow::response(500, res.dump());
        }

        crow::json::wvalue res;
        res["success"] = true;
        res["gameId"] = *newGameId;
        res["created"] = true;
        return crow::response(200, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Invalid JSON";
        return crow::response(400, res.dump());
    }
}

crow::response handleLobbyStatus() {
    try {
        crow::json::wvalue res;

        auto sessions = GameLobby::instance().getAllSessions();
        int i = 0;

        for (auto& session : sessions) {
            if (!session) continue;

            std::lock_guard lock(session->mtx);

            res["games"][i]["gameId"] = session->id;
            res["games"][i]["playersCount"] = session->players.size();
            res["games"][i]["started"] = session->started;
            res["games"][i]["isPublic"] = session->isPublic;
            i++;
        }

        return crow::response(200, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Failed to get lobby status";
        return crow::response(500, res.dump());
    }
}

crow::response handleJoinByPin(const crow::request& req) {
    try {
        auto body = json::parse(req.body);

        if (!body.contains("username") || !body.contains("pin")) {
            crow::json::wvalue res;
            res["error"] = "Missing username or pin";
            return crow::response(400, res.dump());
        }

        std::string username = body["username"].get<std::string>();
        std::string pin = body["pin"].get<std::string>();

        auto gameIdOpt = GameLobby::instance().findGame(pin);

        if (!gameIdOpt.has_value()) {
            crow::json::wvalue res;
            res["error"] = "Game not found";
            return crow::response(404, res.dump());
        }

        if (!GameLobby::instance().joinGame(*gameIdOpt, username)) {
            crow::json::wvalue res;
            res["error"] = "Failed to join game";
            return crow::response(409, res.dump());
        }

        crow::json::wvalue res;
        res["success"] = true;
        res["gameId"] = *gameIdOpt;
        return crow::response(200, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Invalid JSON";
        return crow::response(400, res.dump());
    }
}

crow::response handleStartGame(const crow::request& req) {
    try {
        auto body = json::parse(req.body);

        if (!body.contains("gameId")) {
            crow::json::wvalue res;
            res["error"] = "Missing gameId";
            return crow::response(400, res.dump());
        }

        int gameId = body["gameId"].get<int>();

        auto session = GameLobby::instance().getSession(gameId);
        if (!session) {
            crow::json::wvalue res;
            res["error"] = "Game not found";
            return crow::response(404, res.dump());
        }

        GameLobby::instance().startGame(gameId);

        crow::json::wvalue res;
        res["success"] = true;
        res["gameId"] = gameId;
        res["started"] = true;
        return crow::response(200, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Invalid JSON";
        return crow::response(400, res.dump());
    }
}

crow::response handleLeaveGame(const crow::request& req) {
    try {
        auto body = json::parse(req.body);

        if (!body.contains("gameId") || !body.contains("username")) {
            crow::json::wvalue res;
            res["error"] = "Missing gameId or username";
            return crow::response(400, res.dump());
        }

        int gameId = body["gameId"].get<int>();
        std::string username = body["username"].get<std::string>();

        if (!GameLobby::instance().leaveGame(gameId, username)) {
            crow::json::wvalue res;
            res["error"] = "Failed to leave game";
            return crow::response(409, res.dump());
        }

        crow::json::wvalue res;
        res["success"] = true;
        res["gameId"] = gameId;
        return crow::response(200, res.dump());
    }
    catch (...) {
        crow::json::wvalue res;
        res["error"] = "Invalid JSON";
        return crow::response(400, res.dump());
    }
}

int getPlayerIndex(const std::vector<std::string>& players, const std::string& username) {
    auto it = std::find(players.begin(), players.end(), username);
    if (it != players.end()) {
        return std::distance(players.begin(), it);
    }
    return -1;
}

crow::response handleGetGameState(const crow::request& req, int gameId) {
    auto session = GameLobby::instance().getSession(gameId);
    if (!session) return crow::response(404, "Game not found");

    std::lock_guard lock(session->mtx);
    if (!session->started || !session->gameLogic) {
        nlohmann::json j;
        j["status"] = "Lobby";
        j["stacks"] = nlohmann::json::array();
        j["playerHand"] = nlohmann::json::array();
        j["deckCount"] = 0;
        j["isMyTurn"] = false;
        return crow::response(200, j.dump());
    }

    std::string username;
    if (req.url_params.get("username") != nullptr) {
        username = req.url_params.get("username");
    }
    else {
        return crow::response(400, "Missing username");
    }

    int playerIndex = getPlayerIndex(session->players, username);
    if (playerIndex == -1) return crow::response(403, "Not a player in this game");

    json fullState = session->gameLogic->GetStateForClient(playerIndex);

    json simpleState;
    simpleState["stacks"] = json::array();
    for (const auto& s : fullState["stacks"]) {
        simpleState["stacks"].push_back(s["topValue"]);
    }

    simpleState["playerHand"] = json::array();
    const auto& myData = fullState["players"][playerIndex];
    if (myData.contains("hand")) {
        for (const auto& c : myData["hand"]) {
            simpleState["playerHand"].push_back(c["value"]);
        }
    }

    simpleState["isMyTurn"] = (fullState["currentPlayerIndex"] == playerIndex && fullState["status"] == "Running");
    simpleState["status"] = fullState["status"];

    simpleState["deckCount"] = fullState["deckCount"];

    return crow::response(200, simpleState.dump());
}

crow::response handlePlayCard(const crow::request& req, int gameId) {
    try {
        auto body = json::parse(req.body);
        std::string username = body.value("username", "");
        int cardValue = body.value("cardValue", -1);
        int stackIndex = body.value("stackIndex", -1);

        if (username.empty() || cardValue == -1 || stackIndex == -1) {
            return crow::response(400, "Invalid parameters");
        }

        auto session = GameLobby::instance().getSession(gameId);
        if (!session) return crow::response(404, "Game not found");

        std::lock_guard lock(session->mtx);
        if (!session->started) return crow::response(400, "Game not started");

        int playerIndex = getPlayerIndex(session->players, username);
        if (playerIndex == -1) return crow::response(403, "Not player");

        const auto& hand = session->gameLogic->GetPlayerHand(playerIndex);
        int handIndex = -1;
        for (size_t i = 0; i < hand.size(); ++i) {
            if (hand[i].GetValue() == cardValue) {
                handIndex = i;
                break;
            }
        }

        if (handIndex == -1) {
            crow::json::wvalue res; res["success"] = false; res["error"] = "Card not in hand";
            return crow::response(200, res.dump());
        }

        bool success = session->gameLogic->ProcessPlayCard(playerIndex, handIndex, stackIndex);

        crow::json::wvalue res;
        res["success"] = success;
        return crow::response(200, res.dump());
    }
    catch (...) {
        return crow::response(400, "JSON Error");
    }
}

crow::response handleEndTurn(const crow::request& req, int gameId) {
    try {
        auto body = json::parse(req.body);
        std::string username = body.value("username", "");

        auto session = GameLobby::instance().getSession(gameId);
        if (!session) return crow::response(404);

        std::lock_guard lock(session->mtx);
        int playerIndex = getPlayerIndex(session->players, username);
        if (playerIndex == -1) return crow::response(403);

        bool success = session->gameLogic->ProcessEndTurn(playerIndex);

        crow::json::wvalue res;
        res["success"] = success;
        return crow::response(200, res.dump());
    }
    catch (...) {
        return crow::response(400);
    }
}

crow::response handleGetMessages(const crow::request& req, int gameId) {
    auto session = GameLobby::instance().getSession(gameId);
    if (!session) return crow::response(404);

    std::lock_guard lock(session->mtx);
    crow::json::wvalue res;

    res["messages"] = session->getMessages();

    return crow::response(200, res.dump());
}

crow::response handleSendMessage(const crow::request& req, int gameId) {
    try {
        auto body = json::parse(req.body);
        std::string message = body.value("message", "");
        std::string username = body.value("username", "Anon");

        auto session = GameLobby::instance().getSession(gameId);
        if (!session) return crow::response(404);

        std::lock_guard lock(session->mtx);

        session->addMessage(username + ": " + message);

        crow::json::wvalue res;
        res["success"] = true;
        return crow::response(200, res.dump());
    }
    catch (...) {
        return crow::response(400);
    }
}
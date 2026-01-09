#pragma once

#include <crow.h>


crow::response handleStatus();
crow::response handleRegister(const crow::request& req);
crow::response handleLogin(const crow::request& req);
crow::response handleJoinLobby(const crow::request& req);
crow::response handleCreateGame(const crow::request& req);
crow::response handleLobbyStatus();
crow::response handleJoinByPin(const crow::request& req);
crow::response handleStartGame(const crow::request& req);
crow::response handleGetUsers();
crow::response handleLeaveGame(const crow::request& req);

crow::response handleGetGameState(const crow::request& req, int gameId);
crow::response handlePlayCard(const crow::request& req, int gameId);
crow::response handleEndTurn(const crow::request& req, int gameId);
crow::response handleGetMessages(const crow::request& req, int gameId);
crow::response handleSendMessage(const crow::request& req, int gameId);
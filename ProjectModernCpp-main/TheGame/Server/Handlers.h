#pragma once

#include <crow.h>

// Route handlers

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





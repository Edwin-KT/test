#pragma once

#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <crow/websocket.h>
#include <memory>          
#include "GameLogic.h"     

struct GameSession {
    int id;
    std::string pin;                    // PIN-ul jocului (optional)
    std::vector<std::string> players;   // jucatori care au intrat
    bool started = false;
    std::chrono::steady_clock::time_point creationTime;
    mutable std::mutex mtx;
    bool isPublic = true;
    std::unique_ptr<GameLogic> gameLogic; 
    std::unordered_map<std::string, crow::websocket::connection*> wsConnections;

    bool isAvailableForRandom() const;
    GameSession(int sessionId, std::string_view pin = "", bool publicGame = true);

    bool canJoin() const;
    bool hasMinimumPlayers() const;     // >=2
    bool shouldStartNow() const;         // >=2 sau >30s
    void finalizeJoining();             // muta din queue in players
    void initializeGame();              // ← adăugat

    bool matchesPin(std::string_view pin) const;
};
#include "GameSession.h"
#include <algorithm>

GameSession::GameSession(int sessionId, std::string_view pin, bool publicGame)
    : id(sessionId), pin(pin), isPublic(publicGame), creationTime(std::chrono::steady_clock::now()) {
}

bool GameSession::canJoin() const {
    return !started && players.size() < 5;
}

bool GameSession::hasMinimumPlayers() const {
    return players.size() >= 2;
}

bool GameSession::shouldStartNow() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - creationTime);
    return hasMinimumPlayers() || (elapsed.count() >= 30 && players.size() >= 2);
}
bool GameSession::isAvailableForRandom() const { 
    return isPublic && !started && players.size() < 5;
}
void GameSession::finalizeJoining() {
    
}

void GameSession::initializeGame() {
    gameLogic = std::make_unique<GameLogic>(players);
    gameLogic->Initialize(); 
}

bool GameSession::matchesPin(std::string_view pin) const {
    if (pin.empty()) return true; // daca nu are PIN, oricine poate intra
    return this->pin == pin;
}
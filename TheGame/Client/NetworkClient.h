#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <vector>

struct GameState {
    std::vector<int> stacks;
    std::vector<int> playerHand;
    bool isMyTurn;
    std::string status;
    int deckCount = 0;
};

class NetworkClient {
public:
    static std::string m_username;

    static bool registerUser(const std::string& username, const std::string& password);
    static bool loginUser(const std::string& username, const std::string& password);

    // --- NEW LOBBY FUNCTIONS ---
    static int createGame(const std::string& pin);
    static int joinGame(const std::string& pin);
    static int joinRandomGame();
    // ---------------------------

    static std::vector<std::string> getMessages(const std::string& gameId);
    static bool sendMessage(const std::string& gameId, const std::string& message);
    static bool playCard(const std::string& gameId, int cardValue, int stackIndex);
    static bool endTurn(const std::string& gameId);
    static bool startGame(const std::string& gameId);

    static GameState getGameState(const std::string& gameId);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};
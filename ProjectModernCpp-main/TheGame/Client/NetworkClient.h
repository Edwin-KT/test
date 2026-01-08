#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <vector>

struct GameState {
	std::vector<int> stacks;
	std::vector<int> playerHand;
	bool isMyTurn;
};

class NetworkClient {
public:
    static bool registerUser(const std::string& username, const std::string& password);
    static bool loginUser(const std::string& username, const std::string& password);
	static std::vector<std::string> getMessages(const std::string& gameId);
	static bool sendMessage(const std::string& gameId, const std::string& message);
	static bool playCard(const std::string& gameId, int cardValue, int stackIndex);
	static GameState getGameState(const std::string& gameId);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};
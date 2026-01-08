#include "NetworkClient.h"
#include <nlohmann/json.hpp> 
#include <iostream>

size_t NetworkClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
bool NetworkClient::registerUser(const std::string& username, const std::string& password) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    nlohmann::json data = { {"username", username}, {"password", password} };
    std::string body = data.dump();

    std::cout << "Sending JSON: " << body << std::endl;  //  Logging

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/register");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cout << "CURL error: " << res << std::endl;
        return false;
    }

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        std::cout << "Server response: " << j.dump(2) << std::endl;  //  Logging
        return j.value("success", false);
    }
    catch (...) {
        std::cout << "Failed to parse response: " << response << std::endl;
        return false;
    }
}
bool NetworkClient::loginUser(const std::string& username, const std::string& password) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    nlohmann::json data = { {"username", username}, {"password", password} };
    std::string body = data.dump();

    std::cout << "Sending login JSON: " << body << std::endl;  

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/login");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cout << "CURL error: " << res << std::endl;
        return false;
    }

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        std::cout << "Server response: " << j.dump(2) << std::endl;  
        return j.value("success", false);
    }
    catch (...) {
        std::cout << "Failed to parse response: " << response << std::endl;
        return false;
    }
}

std::vector<std::string> NetworkClient::getMessages(const std::string& gameId)
{
    CURL* curl = curl_easy_init();
    if (!curl) return {};
    std::string url = "http://localhost:18080/games/" + gameId + "/messages";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cout << "CURL error: " << res << std::endl;
        return {};
    }
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        std::vector<std::string> messages;
        for (const auto& msg : j["messages"]) {
            messages.push_back(msg.get<std::string>());
        }
        return messages;
    }
    catch (...) {
        std::cout << "Failed to parse response: " << response << std::endl;
        return {};
	}
}

bool NetworkClient::sendMessage(const std::string& gameId, const std::string& message)
{
    // to do 
    //return true;
	CURL* curl = curl_easy_init();
    if (!curl) return false;
    nlohmann::json data = { {"message", message} };
    std::string body = data.dump();
    std::string url = "http://localhost:18080/games/" + gameId + "/sendMessage";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cout << "CURL error: " << res << std::endl;
        return false;
    }
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return j.value("success", false);
    }
    catch (...) {
        std::cout << "Failed to parse response: " << response << std::endl;
        return false;
	}
}

bool NetworkClient::playCard(const std::string& gameId, int cardValue, int stackIndex)
{
    // to do
    //return true;
	CURL* curl = curl_easy_init();
    if (!curl) return false;
    nlohmann::json data = { {"cardValue", cardValue}, {"stackIndex", stackIndex} };
    std::string body = data.dump();
    std::string url = "http://localhost:18080/games/" + gameId + "/playCard";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cout << "CURL error: " << res << std::endl;
        return false;
    }
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return j.value("success", false);
    }
    catch (...) {
        std::cout << "Failed to parse response: " << response << std::endl;
        return false;
	}
}

GameState NetworkClient::getGameState(const std::string& gameId)
{
    // to do
	CURL* curl = curl_easy_init();
    if (!curl) return {};
    std::string url = "http://localhost:18080/games/" + gameId + "/state";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cout << "CURL error: " << res << std::endl;
        return {};
    }
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        GameState gameState;
        gameState.stacks = j["stacks"].get<std::vector<int>>();
        gameState.playerHand = j["playerHand"].get<std::vector<int>>();
        gameState.isMyTurn = j["isMyTurn"].get<bool>();
        return gameState;
    }
    catch (...) {
        std::cout << "Failed to parse response: " << response << std::endl;
        return {};
	}
}

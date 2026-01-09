#include "NetworkClient.h"
#include <nlohmann/json.hpp> 
#include <iostream>

std::string NetworkClient::m_username = "";

size_t NetworkClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool NetworkClient::registerUser(const std::string& username, const std::string& password) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    nlohmann::json data = { {"username", username}, {"password", password} };
    std::string body = data.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/register");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return false;

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        if (j.value("success", false)) {
            m_username = username; 
            return true;
        }
        return false;
    }
    catch (...) { return false; }
}

bool NetworkClient::loginUser(const std::string& username, const std::string& password) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    nlohmann::json data = { {"username", username}, {"password", password} };
    std::string body = data.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/login");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return false;

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        if (j.value("success", false)) {
            m_username = username; 
            return true;
        }
        return false;
    }
    catch (...) { return false; }
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
    if (res != CURLE_OK) return {};
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        std::vector<std::string> messages;
        if (j.contains("messages")) {
            for (const auto& msg : j["messages"]) {
                messages.push_back(msg.get<std::string>());
            }
        }
        return messages;
    }
    catch (...) { return {}; }
}

bool NetworkClient::sendMessage(const std::string& gameId, const std::string& message)
{
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    nlohmann::json data = { {"message", message}, {"username", m_username} };
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
    if (res != CURLE_OK) return false;
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return j.value("success", false);
    }
    catch (...) { return false; }
}

bool NetworkClient::startGame(const std::string& gameId)
{
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    int gId = std::stoi(gameId);

    nlohmann::json data = { {"gameId", gId} };
    std::string body = data.dump();
    std::string url = "http://localhost:18080/startGame";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return false;

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return j.value("success", false);
    }
    catch (...) { return false; }
}

int NetworkClient::createGame(const std::string& pin) {
    CURL* curl = curl_easy_init();
    if (!curl) return -1;

    nlohmann::json data = { {"username", m_username}, {"pin", pin} };
    std::string body = data.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/create_game");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return -1;

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        if (j.value("success", false)) {
            return j["gameId"].get<int>();
        }
    }
    catch (...) {}
    return -1;
}

int NetworkClient::joinGame(const std::string& pin) {
    CURL* curl = curl_easy_init();
    if (!curl) return -1;

    nlohmann::json data = { {"username", m_username}, {"pin", pin} };
    std::string body = data.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/joinByPin");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return -1;

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        if (j.value("success", false)) {
            return j["gameId"].get<int>();
        }
    }
    catch (...) {}
    return -1;
}

int NetworkClient::joinRandomGame() {
    CURL* curl = curl_easy_init();
    if (!curl) return -1;

    nlohmann::json data = { {"username", m_username} };
    std::string body = data.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/joinLobby");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return -1;

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        if (j.value("success", false)) {
            return j["gameId"].get<int>();
        }
    }
    catch (...) {}
    return -1;
}

bool NetworkClient::playCard(const std::string& gameId, int cardValue, int stackIndex)
{
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    nlohmann::json data = { {"cardValue", cardValue}, {"stackIndex", stackIndex}, {"username", m_username} };
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
    if (res != CURLE_OK) return false;
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return j.value("success", false);
    }
    catch (...) { return false; }
}

bool NetworkClient::endTurn(const std::string& gameId)
{
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    nlohmann::json data = { {"username", m_username} };
    std::string body = data.dump();
    std::string url = "http://localhost:18080/games/" + gameId + "/endTurn";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return false;
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return j.value("success", false);
    }
    catch (...) { return false; }
}

GameState NetworkClient::getGameState(const std::string& gameId)
{
    CURL* curl = curl_easy_init();
    if (!curl) return {};

    std::string url = "http://localhost:18080/games/" + gameId + "/state?username=" + m_username;
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || http_code != 200) return {};

    if (response.empty() || response[0] != '{') return {};

    try {
        nlohmann::json j = nlohmann::json::parse(response);
        GameState gameState;
        if (j.contains("stacks")) gameState.stacks = j["stacks"].get<std::vector<int>>();
        if (j.contains("playerHand")) gameState.playerHand = j["playerHand"].get<std::vector<int>>();
        if (j.contains("isMyTurn")) gameState.isMyTurn = j["isMyTurn"].get<bool>();
        if (j.contains("status")) gameState.status = j["status"].get<std::string>();
        if (j.contains("deckCount")) gameState.deckCount = j["deckCount"].get<int>();
        return gameState;
    }
    catch (...) { return {}; }
}


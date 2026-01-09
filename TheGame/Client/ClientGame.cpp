#include "ClientGame.h"

ClientGame::ClientGame()
    : m_deckCount(0), m_currentPlayerIndex(-1), m_myPlayerIndex(-1), m_cardsPlayedThisTurn(0)
{
}

void ClientGame::UpdateState(const nlohmann::json& j)
{
    m_gameStatus = j["status"].get<std::string>();
    m_currentPlayerIndex = j["currentPlayerIndex"].get<int>();
    m_cardsPlayedThisTurn = j["cardsPlayedThisTurn"].get<int>();
    m_myPlayerIndex = j["yourIndex"].get<int>();
    m_deckCount = j["deckCount"].get<int>();

    m_localStacks.clear();
    for (const auto& sJson : j["stacks"]) {
        std::string typeStr = sJson["type"].get<std::string>();
        int topVal = sJson["topValue"].get<int>();

        StackType type = (typeStr == "Ascending") ? StackType::Ascending : StackType::Descending;
        int baseVal = (type == StackType::Ascending) ? 1 : 100;


        Stack tempStack(type, baseVal);


        if (topVal != baseVal) {
            tempStack.PlaceCard(Card(topVal));
        }
        m_localStacks.push_back(std::move(tempStack));
    }

    m_myHand.clear();
    const auto& playersArray = j["players"];
    if (m_myPlayerIndex >= 0 && m_myPlayerIndex < playersArray.size()) {
        const auto& myData = playersArray[m_myPlayerIndex];
        if (myData.contains("hand")) {
            for (const auto& cJson : myData["hand"]) {
                m_myHand.emplace_back(cJson["value"].get<int>());
            }
        }
    }
}

bool ClientGame::IsMyTurn() const {
    return m_currentPlayerIndex == m_myPlayerIndex && m_gameStatus == "Running";
}

bool ClientGame::CanPlayCardLocally(int handIndex, int stackIndex) const {
    if (!IsMyTurn()) return false;
    if (handIndex < 0 || handIndex >= m_myHand.size()) return false;
    if (stackIndex < 0 || stackIndex >= m_localStacks.size()) return false;

    return m_localStacks[stackIndex].CanPlay(m_myHand[handIndex]);
}

const std::vector<Card>& ClientGame::GetMyHand() const { return m_myHand; }
const std::vector<Stack>& ClientGame::GetStacks() const { return m_localStacks; }
int ClientGame::GetDeckCount() const { return m_deckCount; }
int ClientGame::GetCurrentPlayerIndex() const { return m_currentPlayerIndex; }
int ClientGame::GetMyPlayerIndex() const { return m_myPlayerIndex; }
std::string ClientGame::GetGameStatus() const { return m_gameStatus; }
int ClientGame::GetCardsPlayedThisTurn() const { return m_cardsPlayedThisTurn; }
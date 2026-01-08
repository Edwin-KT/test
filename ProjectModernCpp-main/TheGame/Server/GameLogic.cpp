#include "GameLogic.h"
#include <iostream>

GameLogic::GameLogic(const std::vector<std::string>& playerNames)
    : m_status(GameStatus::Running), m_currentPlayerIndex(0), m_cardsPlayedThisTurn(0)
{
    m_deck.Initialize();

    for (const auto& name : playerNames) {
        m_players.push_back(std::make_unique<Player>(name));
    }

    m_stacks.push_back(std::make_unique<Stack>(StackType::Ascending, 1));
    m_stacks.push_back(std::make_unique<Stack>(StackType::Ascending, 1));
    m_stacks.push_back(std::make_unique<Stack>(StackType::Descending, 100));
    m_stacks.push_back(std::make_unique<Stack>(StackType::Descending, 100));

    int cardsToDeal = 6;
    if (m_players.size() == 2) cardsToDeal = 8;
    else if (m_players.size() == 3) cardsToDeal = 7;

    for (int i = 0; i < cardsToDeal; ++i) {
        for (auto& playerPtr : m_players) {
            if (!m_deck.IsEmpty()) {
                playerPtr->AddCardToHand(m_deck.Draw());
            }
        }
    }
}

bool GameLogic::ProcessPlayCard(int playerIndex, int handCardIndex, int stackIndex)
{
    if (m_status != GameStatus::Running) return false;
    if (playerIndex != m_currentPlayerIndex) return false;
    if (stackIndex < 0 || stackIndex >= m_stacks.size()) return false;

    Player& player = *m_players[playerIndex];

    const auto& hand = player.GetHand();
    if (handCardIndex < 0 || handCardIndex >= hand.size()) return false;

    const Card& cardToPlay = hand[handCardIndex];
    Stack& targetStack = *m_stacks[stackIndex];

    if (targetStack.CanPlay(cardToPlay)) {
        Card playedCard = player.PlayCard(handCardIndex);
        targetStack.PlaceCard(std::move(playedCard));

        m_cardsPlayedThisTurn++;
        return true;
    }

    return false;
}

bool GameLogic::ProcessEndTurn(int playerIndex)
{
    if (m_status != GameStatus::Running) return false;
    if (playerIndex != m_currentPlayerIndex) return false;

    int minCardsToPlay = m_deck.IsEmpty() ? 1 : 2;
    if (m_players[playerIndex]->IsHandEmpty()) minCardsToPlay = 0;

    if (m_cardsPlayedThisTurn < minCardsToPlay) return false;


    Player& player = *m_players[playerIndex];
    while (m_cardsPlayedThisTurn > 0 && !m_deck.IsEmpty()) {
        player.AddCardToHand(m_deck.Draw());
        m_cardsPlayedThisTurn--;
    }

    m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
    m_cardsPlayedThisTurn = 0;

    CheckGameStatus();

    return true;
}

void GameLogic::CheckGameStatus()
{
    if (m_deck.IsEmpty()) {
        bool allHandsEmpty = true;
        for (const auto& pPtr : m_players) {
            if (!pPtr->IsHandEmpty()) {
                allHandsEmpty = false;
                break;
            }
        }
        if (allHandsEmpty) {
            m_status = GameStatus::Won;
            return;
        }
    }

    // ar trebui la inceput
    Player& currentPlayer = *m_players[m_currentPlayerIndex];
    bool hasValidMove = false;

    if (currentPlayer.IsHandEmpty()) hasValidMove = true;

    for (const auto& card : currentPlayer.GetHand()) {
        for (const auto& stackPtr : m_stacks) {
            if (stackPtr->CanPlay(card)) {
                hasValidMove = true;
                break;
            }
        }
        if (hasValidMove) break;
    }

    if (!hasValidMove) {
        m_status = GameStatus::Lost;
    }
}
void GameLogic::Initialize() {
    m_deck.Initialize(); 
    m_deck.Shuffle();

    int cardsPerPlayer = 6;
    if (m_players.size() == 2) cardsPerPlayer = 8;
    else if (m_players.size() == 3) cardsPerPlayer = 7;

    for (auto& player : m_players) {
        for (int i = 0; i < cardsPerPlayer && !m_deck.IsEmpty(); ++i) {
            player->AddCardToHand(m_deck.Draw());
        }
    }

    
    for (int i = 0; i < 4; ++i) {
        m_stacks[i]->PlaceCard(Card(i < 2 ? 1 : 100));
    }
}

GameLogic::GameStatus GameLogic::GetStatus() const { return m_status; }

nlohmann::json GameLogic::GetStateForClient(int requestingPlayerIndex) const {
    nlohmann::json j;

    j["status"] = (m_status == GameStatus::Running ? "Running" : (m_status == GameStatus::Won ? "Won" : "Lost"));
    j["currentPlayerIndex"] = m_currentPlayerIndex;
    j["cardsPlayedThisTurn"] = m_cardsPlayedThisTurn;
    j["yourIndex"] = requestingPlayerIndex;

    j["stacks"] = nlohmann::json::array();
    for (const auto& stackPtr : m_stacks) {
        j["stacks"].push_back(stackPtr->ToJson());
    }

    j["deckCount"] = m_deck.GetCount();

    j["players"] = nlohmann::json::array();
    for (int i = 0; i < m_players.size(); ++i) {
        bool showHandContent = (i == requestingPlayerIndex);
        j["players"].push_back(m_players[i]->ToJson(showHandContent));
    }

    return j;
}
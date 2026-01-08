#pragma once

#include "Card.h"
#include "Stack.h"
#include "Player.h"
#include "Deck.h" 
#include <nlohmann/json.hpp>

#include <vector>
#include <string> 
#include <memory> 

class GameLogic
{
public:
    enum class GameStatus { Running, Won, Lost };

    GameLogic(const std::vector<std::string>& playerNames);

    bool ProcessPlayCard(int playerIndex, int handCardIndex, int stackIndex);

    bool ProcessEndTurn(int playerIndex);

    GameStatus GetStatus() const;
    int GetCurrentPlayerIndex() const;

    nlohmann::json GetStateForClient(int requestingPlayerIndex) const;

    void Initialize();

private:
    std::vector<std::unique_ptr<Player>> m_players;
    std::vector<std::unique_ptr<Stack>> m_stacks;
    

    Deck m_deck;
    GameStatus m_status;
    int m_currentPlayerIndex;
    int m_cardsPlayedThisTurn;

    void CheckGameStatus();
};


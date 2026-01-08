#pragma once

#include "Card.h"
#include "Stack.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

class ClientGame
{
public:
    ClientGame();

    void UpdateState(const nlohmann::json& stateJson);

    bool IsMyTurn() const;
    bool CanPlayCardLocally(int handIndex, int stackIndex) const;


    const std::vector<Card>& GetMyHand() const;
    const std::vector<Stack>& GetStacks() const;
    int GetDeckCount() const;
    int GetCurrentPlayerIndex() const;
    int GetMyPlayerIndex() const;
    std::string GetGameStatus() const;
    int GetCardsPlayedThisTurn() const;

private:
    std::vector<Card> m_myHand;
    std::vector<Stack> m_localStacks;

    int m_deckCount;
    int m_currentPlayerIndex;
    int m_myPlayerIndex;
    int m_cardsPlayedThisTurn;
    std::string m_gameStatus;
};
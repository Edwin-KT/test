#pragma once
#include "Card.h"

#include <stack>


enum StackType {
	Ascending,
	Descending
};

class Stack {
protected:
	StackType m_type;
	std::stack<Card> m_cards;
	int m_baseValue;

public:
	Stack(StackType type, int baseValue);
	void PlaceCard(Card&& card);
	int TopValue() const;
	bool CanPlay(const Card& card) const;
	StackType GetType() const;

	nlohmann::json ToJson() const;
};

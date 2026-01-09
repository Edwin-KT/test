#include "pch.h"
#include "Stack.h"

#include "Card.h"

Stack::Stack(StackType type, int baseValue) : m_type{ type }, m_baseValue{ baseValue }
{
	m_cards.push(Card(m_baseValue));
}

void Stack::PlaceCard(Card&& card)
{
	m_cards.push(std::move(card));
}

int Stack::TopValue() const
{
	if (m_cards.empty())
		return m_baseValue;
	return m_cards.top().GetValue();
}

bool Stack::CanPlay(const Card& card) const
{
	int currentTopValue = this->TopValue();
	int cardValue = card.GetValue();

	if (m_type == StackType::Ascending)
	{
		if (cardValue > currentTopValue) {
			return true;
		}

		if (cardValue == currentTopValue - 10) {
			return true;
		}
	}
	else
	{

		if (cardValue < currentTopValue) {
			return true;
		}

		if (cardValue == currentTopValue + 10) {
			return true;
		}
	}

	return false;
}

StackType Stack::GetType() const
{
	return m_type;
}

nlohmann::json Stack::ToJson() const
{
	return {
		{"type", (m_type == StackType::Ascending ? "Ascending" : "Descending")},
		{"topValue", TopValue()}
	};
}



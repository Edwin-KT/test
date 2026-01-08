#pragma once

#include <nlohmann/json.hpp>

class Card {
private:
    int m_value;
public:
    Card(int value);
    int GetValue() const;
    bool operator<(const Card& other) const;
    bool operator>(const Card& other) const;
    bool operator==(const Card& other) const;
    nlohmann::json ToJson() const;
};


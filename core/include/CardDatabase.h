#pragma once

#include <vector>
#include <string>

#include "Card.h"

namespace NeoDex
{

class CardDatabase
{
private:
    std::vector<Card> cardList;

public:
    void addCard(const Card& card);

    int getCardCount() const;
    Card getCard(int index) const;

    // All cards depicting a given Pal, across every set.
    std::vector<Card> getCardsForPal(int palNumber) const;

    // All cards belonging to a given set (by set code, e.g. "DOP").
    std::vector<Card> getCardsForSet(const std::string& setCode) const;

    void setCardOwned(const std::string& cardId, bool owned);
};

}

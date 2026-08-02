#include "../include/CardDatabase.h"

namespace NeoDex
{

void CardDatabase::addCard(const Card& card)
{
    cardList.push_back(card);
}


int CardDatabase::getCardCount() const
{
    return static_cast<int>(cardList.size());
}


Card CardDatabase::getCard(int index) const
{
    return cardList.at(index);
}


std::vector<Card> CardDatabase::getCardsForPal(int palNumber) const
{
    std::vector<Card> result;
    for(const Card& card : cardList)
    {
        if(card.getPalNumber() == palNumber)
        {
            result.push_back(card);
        }
    }
    return result;
}


std::vector<Card> CardDatabase::getCardsForSet(const std::string& setCode) const
{
    std::vector<Card> result;
    for(const Card& card : cardList)
    {
        if(card.getSetCode() == setCode)
        {
            result.push_back(card);
        }
    }
    return result;
}


void CardDatabase::setCardOwned(const std::string& cardId, bool owned)
{
    for(Card& card : cardList)
    {
        if(card.getCardId() == cardId)
        {
            if(owned)
                card.markOwned();
            else
                card.markUnowned();
            return;
        }
    }
}

}

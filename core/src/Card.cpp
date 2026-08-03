#include "../include/Card.h"

namespace NeoDex
{

Card::Card(
    const std::string& id,
    const std::string& name,
    const std::string& cardSubtitle,
    int pal,
    CardType type,
    const std::string& code,
    const std::string& setDisplayName,
    CardRarity cardRarity
)
:
cardId(id),
cardName(name),
subtitle(cardSubtitle),
palNumber(pal),
cardType(type),
setCode(code),
setName(setDisplayName),
rarity(cardRarity),
cost(-1),
power(-1),
strike(-1),
artPath(""),
owned(false)
{
}


std::string Card::getCardId() const { return cardId; }
std::string Card::getCardName() const { return cardName; }
std::string Card::getSubtitle() const { return subtitle; }
int Card::getPalNumber() const { return palNumber; }
CardType Card::getCardType() const { return cardType; }
std::string Card::getSetCode() const { return setCode; }
std::string Card::getSetName() const { return setName; }
CardRarity Card::getRarity() const { return rarity; }


void Card::setStats(int cardCost, int cardPower, int cardStrike)
{
    cost = cardCost;
    power = cardPower;
    strike = cardStrike;
}

int Card::getCost() const { return cost; }
int Card::getPower() const { return power; }
int Card::getStrike() const { return strike; }


void Card::setArtPath(const std::string& path)
{
    artPath = path;
}


std::string Card::getArtPath() const
{
    return artPath;
}


void Card::markOwned() { owned = true; }
void Card::markUnowned() { owned = false; }
bool Card::isOwned() const { return owned; }

}

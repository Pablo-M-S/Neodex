#include "../include/Card.h"

namespace NeoDex
{

Card::Card(
    const std::string& id,
    int pal,
    const std::string& code,
    const std::string& name,
    CardRarity cardRarity
)
:
cardId(id),
palNumber(pal),
setCode(code),
setName(name),
rarity(cardRarity),
artPath(""),
owned(false)
{
}


std::string Card::getCardId() const
{
    return cardId;
}


int Card::getPalNumber() const
{
    return palNumber;
}


std::string Card::getSetCode() const
{
    return setCode;
}


std::string Card::getSetName() const
{
    return setName;
}


CardRarity Card::getRarity() const
{
    return rarity;
}


void Card::setArtPath(const std::string& path)
{
    artPath = path;
}


std::string Card::getArtPath() const
{
    return artPath;
}


void Card::markOwned()
{
    owned = true;
}


void Card::markUnowned()
{
    owned = false;
}


bool Card::isOwned() const
{
    return owned;
}

}

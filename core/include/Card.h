#pragma once

#include <string>

namespace NeoDex
{

// Card rarity, matching real Palworld TCG rarities.
enum class CardRarity
{
    Common,
    Uncommon,
    Rare,
    SuperRare,
    UltraRare,
    SecretRare
};

// A single printed card. A Pal (species) can have many Cards across
// different sets/expansions (e.g. a normal print and a holo print in the
// same set, or reprints in later sets).
class Card
{
private:

    std::string cardId;      // unique id, e.g. "DOP-001"
    int palNumber;           // which Pal this card depicts
    std::string setCode;     // e.g. "DOP" for Dawn of Palpagos
    std::string setName;     // e.g. "Dawn of Palpagos"
    CardRarity rarity;
    std::string artPath;     // path to the card artwork
    bool owned;

public:

    Card(
        const std::string& id,
        int pal,
        const std::string& code,
        const std::string& name,
        CardRarity cardRarity
    );

    std::string getCardId() const;
    int getPalNumber() const;
    std::string getSetCode() const;
    std::string getSetName() const;
    CardRarity getRarity() const;

    void setArtPath(const std::string& path);
    std::string getArtPath() const;

    void markOwned();
    void markUnowned();
    bool isOwned() const;

};

}

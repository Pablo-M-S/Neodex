#pragma once

#include <string>

namespace NeoDex
{

enum class CardRarity
{
    Common,
    Uncommon,
    Rare,
    DoubleRare,
    Unknown
};

enum class CardType
{
    Pal,
    Gear,
    Structure,
    Event,
    Soul
};

// A single printed card. A Pal (species) can have many Cards across
// different sets/expansions. Non-Pal cards (Gear, Structure, Event, Soul)
// have palNumber == -1.
class Card
{
private:

    std::string cardId;      // e.g. "BP01-001"
    std::string cardName;    // e.g. "Jormuntide Ignis" or "Mounted Machine Gun"
    std::string subtitle;    // e.g. "Savage Lava Dragon" (Pal cards only)
    int palNumber;           // which Pal this card depicts, -1 if not a Pal card
    CardType cardType;
    std::string setCode;     // e.g. "BP01"
    std::string setName;     // e.g. "Dawn of Palpagos"
    CardRarity rarity;

    // TCG-specific stats. -1 means "not revealed/known yet".
    int cost;
    int power;
    int strike;

    std::string artPath;
    bool owned;

public:

    Card(
        const std::string& id,
        const std::string& name,
        const std::string& cardSubtitle,
        int pal,
        CardType type,
        const std::string& code,
        const std::string& setDisplayName,
        CardRarity cardRarity
    );

    std::string getCardId() const;
    std::string getCardName() const;
    std::string getSubtitle() const;
    int getPalNumber() const;
    CardType getCardType() const;
    std::string getSetCode() const;
    std::string getSetName() const;
    CardRarity getRarity() const;

    void setStats(int cardCost, int cardPower, int cardStrike);
    int getCost() const;
    int getPower() const;
    int getStrike() const;

    void setArtPath(const std::string& path);
    std::string getArtPath() const;

    void markOwned();
    void markUnowned();
    bool isOwned() const;

};

}

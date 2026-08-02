#include "../include/SetLoader.h"

#include "../include/JsonValue.h"
#include "../include/Pal.h"
#include "../include/PalElement.h"
#include "../include/PalStats.h"
#include "../include/Card.h"
#include "../include/Ability.h"

#include <unordered_map>
#include <stdexcept>

namespace NeoDex
{

namespace
{
    PalElement parseElement(const std::string& name)
    {
        static const std::unordered_map<std::string, PalElement> table = {
            {"None", PalElement::None},
            {"Neutral", PalElement::Neutral},
            {"Fire", PalElement::Fire},
            {"Water", PalElement::Water},
            {"Grass", PalElement::Grass},
            {"Electric", PalElement::Electric},
            {"Ice", PalElement::Ice},
            {"Earth", PalElement::Earth},
            {"Dark", PalElement::Dark},
            {"Dragon", PalElement::Dragon},
        };

        auto it = table.find(name);
        if(it == table.end())
        {
            throw std::runtime_error("SetLoader: unknown PalElement '" + name + "'");
        }
        return it->second;
    }

    CardRarity parseRarity(const std::string& name)
    {
        static const std::unordered_map<std::string, CardRarity> table = {
            {"Common", CardRarity::Common},
            {"Uncommon", CardRarity::Uncommon},
            {"Rare", CardRarity::Rare},
            {"SuperRare", CardRarity::SuperRare},
            {"UltraRare", CardRarity::UltraRare},
            {"SecretRare", CardRarity::SecretRare},
        };

        auto it = table.find(name);
        if(it == table.end())
        {
            throw std::runtime_error("SetLoader: unknown CardRarity '" + name + "'");
        }
        return it->second;
    }
}


void SetLoader::loadPals(
    const std::string& jsonPath,
    PalDatabase& database
)
{
    JsonValue root = JsonValue::parseFile(jsonPath);
    const JsonValue& palArray = root["pals"];

    for(size_t i = 0; i < palArray.size(); i++)
    {
        const JsonValue& entry = palArray[i];

        int number = entry["number"].asInt();
        std::string name = entry["name"].asString();

        std::vector<std::string> elementNames = entry["elements"].asStringArray();
        PalElement element1 = elementNames.size() > 0 ? parseElement(elementNames[0]) : PalElement::None;
        PalElement element2 = elementNames.size() > 1 ? parseElement(elementNames[1]) : PalElement::None;

        const JsonValue& statsJson = entry["stats"];
        PalStats stats(
            statsJson["hp"].asInt(),
            statsJson["attack"].asInt(),
            statsJson["defense"].asInt()
        );

        Pal pal(number, name, element1, element2, stats);

        pal.setHeight(static_cast<float>(entry["height"].asDouble()));
        pal.setWeight(static_cast<float>(entry["weight"].asDouble()));
        pal.setDescription(entry["description"].asString());
        pal.setSpritePath(entry["spritePath"].asString());
        pal.setCryPath(entry["cryPath"].asString());

        if(entry.hasKey("abilities"))
        {
            const JsonValue& abilitiesJson = entry["abilities"];
            Ability ability;
            ability.primary = abilitiesJson["primary"].asString();
            ability.secondary = abilitiesJson["secondary"].asString();
            ability.hidden = abilitiesJson["hidden"].asString();
            pal.setAbilities(ability);
        }

        database.addPal(pal);
    }
}


void SetLoader::loadSet(
    const std::string& jsonPath,
    CardDatabase& cards
)
{
    JsonValue root = JsonValue::parseFile(jsonPath);

    std::string setCode = root["setCode"].asString();
    std::string setName = root["setName"].asString();

    const JsonValue& cardArray = root["cards"];

    for(size_t i = 0; i < cardArray.size(); i++)
    {
        const JsonValue& entry = cardArray[i];

        std::string cardId = entry["cardId"].asString();
        int palNumber = entry["palNumber"].asInt();
        CardRarity rarity = parseRarity(entry["rarity"].asString());

        Card card(cardId, palNumber, setCode, setName, rarity);

        if(entry.hasKey("artPath"))
        {
            card.setArtPath(entry["artPath"].asString());
        }

        cards.addCard(card);
    }
}

}

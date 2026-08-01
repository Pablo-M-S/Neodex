#include "../include/RegionLoader.h"

#include "../include/JsonValue.h"
#include "../include/Pokemon.h"
#include "../include/PokemonType.h"
#include "../include/Generation.h"
#include "../include/Region.h"
#include "../include/BaseStats.h"
#include "../include/Ability.h"

#include <unordered_map>
#include <stdexcept>

namespace NeoDex
{

namespace
{
    PokemonType parseType(const std::string& name)
    {
        static const std::unordered_map<std::string, PokemonType> table = {
            {"None", PokemonType::None},
            {"Normal", PokemonType::Normal},
            {"Fire", PokemonType::Fire},
            {"Water", PokemonType::Water},
            {"Electric", PokemonType::Electric},
            {"Grass", PokemonType::Grass},
            {"Ice", PokemonType::Ice},
            {"Fighting", PokemonType::Fighting},
            {"Poison", PokemonType::Poison},
            {"Ground", PokemonType::Ground},
            {"Flying", PokemonType::Flying},
            {"Psychic", PokemonType::Psychic},
            {"Bug", PokemonType::Bug},
            {"Rock", PokemonType::Rock},
            {"Ghost", PokemonType::Ghost},
            {"Dragon", PokemonType::Dragon},
            {"Dark", PokemonType::Dark},
            {"Steel", PokemonType::Steel},
            {"Fairy", PokemonType::Fairy},
        };

        auto it = table.find(name);
        if(it == table.end())
        {
            throw std::runtime_error("RegionLoader: unknown PokemonType '" + name + "'");
        }
        return it->second;
    }

    Generation parseGeneration(const std::string& name)
    {
        static const std::unordered_map<std::string, Generation> table = {
            {"Generation_I", Generation::Generation_I},
            {"Generation_II", Generation::Generation_II},
            {"Generation_III", Generation::Generation_III},
            {"Generation_IV", Generation::Generation_IV},
            {"Generation_V", Generation::Generation_V},
            {"Generation_VI", Generation::Generation_VI},
            {"Generation_VII", Generation::Generation_VII},
            {"Generation_VIII", Generation::Generation_VIII},
            {"Generation_IX", Generation::Generation_IX},
        };

        auto it = table.find(name);
        if(it == table.end())
        {
            throw std::runtime_error("RegionLoader: unknown Generation '" + name + "'");
        }
        return it->second;
    }

    Region parseRegion(const std::string& name)
    {
        static const std::unordered_map<std::string, Region> table = {
            {"Kanto", Region::Kanto},
            {"Johto", Region::Johto},
            {"Hoenn", Region::Hoenn},
            {"Sinnoh", Region::Sinnoh},
            {"Unova", Region::Unova},
            {"Kalos", Region::Kalos},
            {"Alola", Region::Alola},
            {"Galar", Region::Galar},
            {"Hisui", Region::Hisui},
            {"Paldea", Region::Paldea},
        };

        auto it = table.find(name);
        if(it == table.end())
        {
            throw std::runtime_error("RegionLoader: unknown Region '" + name + "'");
        }
        return it->second;
    }
}


void RegionLoader::load(
    const std::string& jsonPath,
    PokedexDatabase& database,
    EvolutionDatabase& evolutions
)
{
    JsonValue root = JsonValue::parseFile(jsonPath);
    const JsonValue& pokemonArray = root["pokemon"];

    for(size_t i = 0; i < pokemonArray.size(); i++)
    {
        const JsonValue& entry = pokemonArray[i];

        int number = entry["number"].asInt();
        std::string name = entry["name"].asString();

        std::vector<std::string> typeNames = entry["types"].asStringArray();
        PokemonType type1 = typeNames.size() > 0 ? parseType(typeNames[0]) : PokemonType::None;
        PokemonType type2 = typeNames.size() > 1 ? parseType(typeNames[1]) : PokemonType::None;

        Generation generation = parseGeneration(entry["generation"].asString());
        Region region = parseRegion(entry["region"].asString());

        const JsonValue& statsJson = entry["stats"];
        BaseStats stats(
            statsJson["hp"].asInt(),
            statsJson["attack"].asInt(),
            statsJson["defense"].asInt(),
            statsJson["specialAttack"].asInt(),
            statsJson["specialDefense"].asInt(),
            statsJson["speed"].asInt()
        );

        Pokemon pokemon(number, name, type1, type2, generation, region, stats);

        pokemon.setHeight(static_cast<float>(entry["height"].asDouble()));
        pokemon.setWeight(static_cast<float>(entry["weight"].asDouble()));
        pokemon.setDescription(entry["description"].asString());
        pokemon.setSpritePath(entry["spritePath"].asString());
        pokemon.setCryPath(entry["cryPath"].asString());

        if(entry.hasKey("abilities"))
        {
            const JsonValue& abilitiesJson = entry["abilities"];
            Ability ability;
            ability.primary = abilitiesJson["primary"].asString();
            ability.secondary = abilitiesJson["secondary"].asString();
            ability.hidden = abilitiesJson["hidden"].asString();
            pokemon.setAbilities(ability);
        }

        database.addPokemon(pokemon);

        if(entry.hasKey("evolvesTo"))
        {
            std::vector<int> targets = entry["evolvesTo"].asIntArray();
            for(int target : targets)
            {
                evolutions.addEvolution(number, target);
            }
        }
    }
}

}

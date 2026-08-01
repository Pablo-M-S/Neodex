#include "../include/PokedexSearch.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace NeoDex
{

namespace
{
    std::string normalize(const std::string& text)
    {
        std::string result = text;

        std::transform(
            result.begin(),
            result.end(),
            result.begin(),
            [](unsigned char c)
            {
                return std::tolower(c);
            }
        );

        return result;
    }
}


Pokemon PokedexSearch::searchByNumber(
    const PokedexDatabase& database,
    int number
)
{
    return database.getPokemon(number);
}


std::vector<Pokemon> PokedexSearch::searchByType(
    const PokedexDatabase& database,
    PokemonType type
)
{
    std::vector<Pokemon> result;

    for(int i = 1; i <= database.getPokemonCount(); i++)
    {
        Pokemon pokemon = database.getPokemon(i);

        if(pokemon.getPrimaryType() == type || pokemon.getSecondaryType() == type)
        {
            result.push_back(pokemon);
        }
    }

    return result;
}


Pokemon PokedexSearch::searchByName(
    const PokedexDatabase& database,
    const std::string& name
)
{
    std::string searchName = normalize(name);

    for(int i = 1; i <= database.getPokemonCount(); i++)
    {
        Pokemon pokemon = database.getPokemon(i);

        if(normalize(pokemon.getName()) == searchName)
        {
            return pokemon;
        }
    }

    throw std::invalid_argument(
        "PokedexSearch::searchByName - pokemon not found: " + name
    );
}

}

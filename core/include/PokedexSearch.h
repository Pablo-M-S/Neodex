#pragma once

#include <vector>
#include <string>

#include "PokedexDatabase.h"
#include "Pokemon.h"
#include "PokemonType.h"

namespace NeoDex
{

class PokedexSearch
{

public:

    // Throws std::out_of_range if number is invalid.
    static Pokemon searchByNumber(
        const PokedexDatabase& database,
        int number
    );

    static std::vector<Pokemon> searchByType(
        const PokedexDatabase& database,
        PokemonType type
    );

    // Throws std::invalid_argument if no pokemon matches the name.
    static Pokemon searchByName(
        const PokedexDatabase& database,
        const std::string& name
    );

};

}

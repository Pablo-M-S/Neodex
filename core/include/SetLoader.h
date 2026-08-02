#pragma once

#include <string>

#include "PalDatabase.h"
#include "CardDatabase.h"

namespace NeoDex
{

// Loads Palworld TCG data from JSON files. This replaces the old
// RegionLoader: instead of one file per game region, we now have one file
// with all Pal species (the Paldeck) and one file per card set/expansion.
class SetLoader
{

public:

    // Loads every Pal species (name, elements, stats, abilities) from a
    // single Paldeck JSON file. Throws std::runtime_error on failure.
    static void loadPals(
        const std::string& jsonPath,
        PalDatabase& database
    );

    // Loads every Card in a set/expansion JSON file (card id, which Pal it
    // depicts, rarity, artwork). Throws std::runtime_error on failure.
    static void loadSet(
        const std::string& jsonPath,
        CardDatabase& cards
    );

};

}

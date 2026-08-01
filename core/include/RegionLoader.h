#pragma once

#include <string>

#include "PokedexDatabase.h"
#include "EvolutionDatabase.h"

namespace NeoDex
{

// Loads a region's Pokemon (stats, types, abilities) and evolution chains
// from a single JSON data file (see data/regions/*.json for the schema).
//
// This replaces the old pattern of one hand-written .cpp file per region
// (KantoData.cpp, KantoAbilityData.cpp, ...). Adding a new region is now a
// matter of adding a new JSON file, not writing thousands of lines of C++.
class RegionLoader
{

public:

    // Throws std::runtime_error if the file can't be read or parsed.
    static void load(
        const std::string& jsonPath,
        PokedexDatabase& database,
        EvolutionDatabase& evolutions
    );

};

}

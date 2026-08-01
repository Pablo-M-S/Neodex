#pragma once

#include <vector>
#include "Evolution.h"

namespace NeoDex
{

class EvolutionDatabase
{

private:

    std::vector<Evolution> evolutions;

public:

    EvolutionDatabase();

    void addEvolution(int from, int to);

    std::vector<Evolution> getEvolutionsFrom(int pokedexNumber) const;

};

}
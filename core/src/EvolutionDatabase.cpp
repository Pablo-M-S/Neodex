#include "../include/EvolutionDatabase.h"

namespace NeoDex
{

EvolutionDatabase::EvolutionDatabase()
{

}

void EvolutionDatabase::addEvolution(int from, int to)
{
    evolutions.push_back({from, to});
}

std::vector<Evolution> EvolutionDatabase::getEvolutionsFrom(int pokedexNumber) const
{
    std::vector<Evolution> result;

    for(const Evolution& evolution : evolutions)
    {
        if(evolution.fromPokedexNumber == pokedexNumber)
        {
            result.push_back(evolution);
        }
    }

    return result;
}

}
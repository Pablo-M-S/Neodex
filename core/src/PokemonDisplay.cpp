#include "../include/PokemonDisplay.h"
#include "../include/Ability.h"

#include <iostream>

namespace NeoDex
{

void PokemonDisplay::show(const Pokemon& pokemon)
{
    Ability abilities = pokemon.getAbilities();

    std::cout << "====================" << std::endl;

    std::cout << "#"
              << pokemon.getPokedexNumber()
              << " "
              << pokemon.getName()
              << std::endl;

    std::cout << "Captured: "
              << (pokemon.isCaught() ? "Yes" : "No")
              << std::endl;

    std::cout << "Ability: "
              << abilities.primary
              << std::endl;

    if (!abilities.secondary.empty())
    {
        std::cout << "Second Ability: "
                  << abilities.secondary
                  << std::endl;
    }

    if (!abilities.hidden.empty())
    {
        std::cout << "Hidden Ability: "
                  << abilities.hidden
                  << std::endl;
    }

    std::cout << "====================" << std::endl;
}

}


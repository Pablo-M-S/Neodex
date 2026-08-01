#pragma once

#include <vector>

#include "Pokemon.h"
#include "Ability.h"

namespace NeoDex
{

class PokedexDatabase
{
private:
    std::vector<Pokemon> pokemonList;

public:
    void addPokemon(const Pokemon& pokemon);

    void setPokemonAbilities(int number, const Ability& abilities);
    Ability getPokemonAbilities(int number) const;

    int getPokemonCount() const;

    // Throws std::out_of_range if id is not a valid pokedex number.
    Pokemon getPokemon(int id) const;
};

}

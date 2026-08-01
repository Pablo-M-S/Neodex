#include "../include/PokedexDatabase.h"
#include "../include/Ability.h"

#include <stdexcept>
#include <string>

namespace NeoDex
{

void PokedexDatabase::addPokemon(const Pokemon& pokemon)
{
    pokemonList.push_back(pokemon);
}


void PokedexDatabase::setPokemonAbilities(
    int number,
    const Ability& abilities
)
{
    for(Pokemon& pokemon : pokemonList)
    {
        if(pokemon.getPokedexNumber() == number)
        {
            pokemon.setAbilities(abilities);
            return;
        }
    }
}


Ability PokedexDatabase::getPokemonAbilities(int number) const
{
    for(const Pokemon& pokemon : pokemonList)
    {
        if(pokemon.getPokedexNumber() == number)
        {
            return pokemon.getAbilities();
        }
    }

    return Ability{};
}


void PokedexDatabase::setPokemonCaught(int number, bool caught)
{
    for(Pokemon& pokemon : pokemonList)
    {
        if(pokemon.getPokedexNumber() == number)
        {
            if(caught)
                pokemon.catchPokemon();
            else
                pokemon.releasePokemon();
            return;
        }
    }
}


int PokedexDatabase::getPokemonCount() const
{
    return static_cast<int>(pokemonList.size());
}


Pokemon PokedexDatabase::getPokemon(int id) const
{
    if(id < 1 || id > static_cast<int>(pokemonList.size()))
    {
        throw std::out_of_range(
            "PokedexDatabase::getPokemon - invalid pokedex number: " + std::to_string(id)
        );
    }

    return pokemonList[id - 1];
}

}

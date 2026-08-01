#include "../include/Pokemon.h"

namespace NeoDex
{

Pokemon::Pokemon(
    int number,
    const std::string& pokemonName,
    PokemonType type1,
    PokemonType type2,
    Generation gen,
    Region reg,
    BaseStats baseStats
)
:
pokedexNumber(number),
name(pokemonName),
primaryType(type1),
secondaryType(type2),
generation(gen),
region(reg),
stats(baseStats),
abilities({}),
spritePath(""),
cryPath(""),
caught(false),
description(""),
height(0.0f),
weight(0.0f)
{
}


int Pokemon::getPokedexNumber() const
{
    return pokedexNumber;
}


std::string Pokemon::getName() const
{
    return name;
}


PokemonType Pokemon::getPrimaryType() const
{
    return primaryType;
}


PokemonType Pokemon::getSecondaryType() const
{
    return secondaryType;
}


Generation Pokemon::getGeneration() const
{
    return generation;
}


Region Pokemon::getRegion() const
{
    return region;
}


BaseStats Pokemon::getBaseStats() const
{
    return stats;
}


void Pokemon::setSpritePath(const std::string& path)
{
    spritePath = path;
}


std::string Pokemon::getSpritePath() const
{
    return spritePath;
}


void Pokemon::setCryPath(const std::string& path)
{
    cryPath = path;
}


std::string Pokemon::getCryPath() const
{
    return cryPath;
}


void Pokemon::catchPokemon()
{
    caught = true;
}


void Pokemon::releasePokemon()
{
    caught = false;
}


bool Pokemon::isCaught() const
{
    return caught;
}

void Pokemon::setDescription(const std::string& text)
{
    description = text;
}


std::string Pokemon::getDescription() const
{
    return description;
}


void Pokemon::setHeight(float value)
{
    height = value;
}


float Pokemon::getHeight() const
{
    return height;
}


void Pokemon::setWeight(float value)
{
    weight = value;
}


float Pokemon::getWeight() const
{
    return weight;
}

void Pokemon::setAbilities(const Ability& value)
{
    abilities = value;
}


Ability Pokemon::getAbilities() const
{
    return abilities;
}

}
#pragma once

#include <string>
#include "PokemonType.h"
#include "Generation.h"
#include "Region.h"
#include "BaseStats.h"
#include "Ability.h"

namespace NeoDex
{

class Pokemon
{
private:

    int pokedexNumber;
    std::string name;

    PokemonType primaryType;
    PokemonType secondaryType;

    Generation generation;
    Region region;

    BaseStats stats;
    
    Ability abilities;

    std::string spritePath;
    std::string cryPath;
    bool caught;

    std::string description;

    float height;
    float weight;

public:

    Pokemon(
        int number,
        const std::string& pokemonName,
        PokemonType type1,
        PokemonType type2,
        Generation gen,
        Region reg,
        BaseStats baseStats
    );

    void setAbilities(const Ability& value);

    Ability getAbilities() const;

    int getPokedexNumber() const;
    std::string getName() const;

    PokemonType getPrimaryType() const;
    PokemonType getSecondaryType() const;

    Generation getGeneration() const;
    Region getRegion() const;

    BaseStats getBaseStats() const;

    void setSpritePath(const std::string& path);
    std::string getSpritePath() const;

    void setCryPath(const std::string& path);
    std::string getCryPath() const;

    void catchPokemon();
    void releasePokemon();

    bool isCaught() const;

    void setDescription(const std::string& text);
    std::string getDescription() const;

    void setHeight(float value);
    float getHeight() const;

    void setWeight(float value);
    float getWeight() const;

};

}
#include "../include/Pal.h"

namespace NeoDex
{

Pal::Pal(
    int number,
    const std::string& palName,
    PalElement element1,
    PalElement element2,
    PalStats baseStats
)
:
palNumber(number),
name(palName),
primaryElement(element1),
secondaryElement(element2),
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


int Pal::getPalNumber() const
{
    return palNumber;
}


std::string Pal::getName() const
{
    return name;
}


PalElement Pal::getPrimaryElement() const
{
    return primaryElement;
}


PalElement Pal::getSecondaryElement() const
{
    return secondaryElement;
}


PalStats Pal::getStats() const
{
    return stats;
}


void Pal::setSpritePath(const std::string& path)
{
    spritePath = path;
}


std::string Pal::getSpritePath() const
{
    return spritePath;
}


void Pal::setCryPath(const std::string& path)
{
    cryPath = path;
}


std::string Pal::getCryPath() const
{
    return cryPath;
}


void Pal::catchPal()
{
    caught = true;
}


void Pal::releasePal()
{
    caught = false;
}


bool Pal::isCaught() const
{
    return caught;
}

void Pal::setDescription(const std::string& text)
{
    description = text;
}


std::string Pal::getDescription() const
{
    return description;
}


void Pal::setHeight(float value)
{
    height = value;
}


float Pal::getHeight() const
{
    return height;
}


void Pal::setWeight(float value)
{
    weight = value;
}


float Pal::getWeight() const
{
    return weight;
}

void Pal::setAbilities(const Ability& value)
{
    abilities = value;
}


Ability Pal::getAbilities() const
{
    return abilities;
}

}

#pragma once

#include <string>
#include "PalElement.h"
#include "PalStats.h"
#include "Ability.h"

namespace NeoDex
{

class Pal
{
private:

    int palNumber; // Paldeck number
    std::string name;

    PalElement primaryElement;
    PalElement secondaryElement;

    PalStats stats;

    Ability abilities; // Palworld "Partner Skill" / passive skills

    std::string spritePath;
    std::string cryPath;
    bool caught;

    std::string description;

    float height;
    float weight;

public:

    Pal(
        int number,
        const std::string& palName,
        PalElement element1,
        PalElement element2,
        PalStats baseStats
    );

    void setAbilities(const Ability& value);
    Ability getAbilities() const;

    int getPalNumber() const;
    std::string getName() const;

    PalElement getPrimaryElement() const;
    PalElement getSecondaryElement() const;

    PalStats getStats() const;

    void setSpritePath(const std::string& path);
    std::string getSpritePath() const;

    void setCryPath(const std::string& path);
    std::string getCryPath() const;

    void catchPal();
    void releasePal();

    bool isCaught() const;

    void setDescription(const std::string& text);
    std::string getDescription() const;

    void setHeight(float value);
    float getHeight() const;

    void setWeight(float value);
    float getWeight() const;

};

}

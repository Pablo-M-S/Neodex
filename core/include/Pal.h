#pragma once

#include <string>
#include "PalElement.h"
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

    int recruitLevel; // minimum level at which this Pal is found/tamed

    Ability abilities; // Palworld "Partner Skill" / passive skills

    std::string spritePath;
    std::string cryPath;
    bool caught;

    std::string description;

public:

    Pal(
        int number,
        const std::string& palName,
        PalElement element1,
        PalElement element2,
        int minRecruitLevel
    );

    void setAbilities(const Ability& value);
    Ability getAbilities() const;

    int getPalNumber() const;
    std::string getName() const;

    PalElement getPrimaryElement() const;
    PalElement getSecondaryElement() const;

    int getRecruitLevel() const;

    void setSpritePath(const std::string& path);
    std::string getSpritePath() const;

    void setCryPath(const std::string& path);
    std::string getCryPath() const;

    void catchPal();
    void releasePal();

    bool isCaught() const;

    void setDescription(const std::string& text);
    std::string getDescription() const;

};

}


#include "../include/PalDatabase.h"
#include "../include/Ability.h"

#include <stdexcept>
#include <string>

namespace NeoDex
{

void PalDatabase::addPal(const Pal& pal)
{
    palList.push_back(pal);
}


void PalDatabase::setPalAbilities(
    int number,
    const Ability& abilities
)
{
    for(Pal& pal : palList)
    {
        if(pal.getPalNumber() == number)
        {
            pal.setAbilities(abilities);
            return;
        }
    }
}


Ability PalDatabase::getPalAbilities(int number) const
{
    for(const Pal& pal : palList)
    {
        if(pal.getPalNumber() == number)
        {
            return pal.getAbilities();
        }
    }

    return Ability{};
}


void PalDatabase::setPalCaught(int number, bool caught)
{
    for(Pal& pal : palList)
    {
        if(pal.getPalNumber() == number)
        {
            if(caught)
                pal.catchPal();
            else
                pal.releasePal();
            return;
        }
    }
}


int PalDatabase::getPalCount() const
{
    return static_cast<int>(palList.size());
}


Pal PalDatabase::getPal(int number) const
{
    if(number < 1 || number > static_cast<int>(palList.size()))
    {
        throw std::out_of_range(
            "PalDatabase::getPal - invalid Paldeck number: " + std::to_string(number)
        );
    }

    return palList[number - 1];
}

}

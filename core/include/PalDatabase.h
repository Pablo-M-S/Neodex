#pragma once

#include <vector>

#include "Pal.h"
#include "Ability.h"

namespace NeoDex
{

class PalDatabase
{
private:
    std::vector<Pal> palList;

public:
    void addPal(const Pal& pal);

    void setPalAbilities(int number, const Ability& abilities);
    Ability getPalAbilities(int number) const;

    // Updates a Pal's caught status by Paldeck number.
    // Does nothing if the number doesn't exist.
    void setPalCaught(int number, bool caught);

    int getPalCount() const;

    // Throws std::out_of_range if number is not a valid Paldeck number.
    Pal getPal(int number) const;
};

}

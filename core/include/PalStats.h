#pragma once

namespace NeoDex
{

// Combat stats for a Pal. Palworld uses a simpler model than Pokemon:
// just HP, Attack and Defense (no Special Attack/Defense or Speed as
// separate numeric stats).
class PalStats
{

private:

    int hp;
    int attack;
    int defense;


public:

    PalStats(
        int hp,
        int attack,
        int defense
    )
    :
    hp(hp),
    attack(attack),
    defense(defense)
    {}

    int getHp() const { return hp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }

    int getTotal() const
    {
        return hp + attack + defense;
    }

};

}

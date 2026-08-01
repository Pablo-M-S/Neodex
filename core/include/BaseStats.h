#pragma once

namespace NeoDex
{

class BaseStats
{

private:

    int hp;
    int attack;
    int defense;
    int specialAttack;
    int specialDefense;
    int speed;


public:

    BaseStats(
        int hp,
        int attack,
        int defense,
        int specialAttack,
        int specialDefense,
        int speed
    )
    :
    hp(hp),
    attack(attack),
    defense(defense),
    specialAttack(specialAttack),
    specialDefense(specialDefense),
    speed(speed)
    {}

    int getHp() const { return hp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getSpecialAttack() const { return specialAttack; }
    int getSpecialDefense() const { return specialDefense; }
    int getSpeed() const { return speed; }

    int getTotal() const
    {
        return hp + attack + defense + specialAttack + specialDefense + speed;
    }

};

}
#include <iostream>

#include "../include/PalDatabase.h"
#include "../include/CardDatabase.h"
#include "../include/SetLoader.h"

using namespace NeoDex;

int main()
{
    PalDatabase pals;
    CardDatabase cards;

    try
    {
        SetLoader::loadPals("data/pals.json", pals);
        SetLoader::loadSet("data/sets/dawn-of-palpagos.json", cards);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to load data: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "NeoDex loaded!" << std::endl;
    std::cout << "Pal count: " << pals.getPalCount() << std::endl;
    std::cout << "Card count: " << cards.getCardCount() << std::endl;

    Pal first = pals.getPal(1);
    std::cout << "First Pal: " << first.getName() << std::endl;

    Pal last = pals.getPal(pals.getPalCount());
    std::cout << "Last Pal: " << last.getName() << std::endl;

    std::cout << first.getName() << " stat total: " << first.getStats().getTotal() << std::endl;

    std::vector<Card> firstPalCards = cards.getCardsForPal(1);
    std::cout << first.getName() << " has " << firstPalCards.size() << " card(s)" << std::endl;

    first.catchPal();
    std::cout << first.getName() << " caught? " << (first.isCaught() ? "Yes" : "No") << std::endl;

    return 0;
}

#include <iostream>

#include "../include/PokedexDatabase.h"
#include "../include/EvolutionDatabase.h"
#include "../include/RegionLoader.h"
#include "../include/PokedexSearch.h"
#include "../include/PokemonDisplay.h"

using namespace NeoDex;

int main()
{
    PokedexDatabase database;
    EvolutionDatabase evolutions;

    try
    {
        RegionLoader::load("data/regions/kanto.json", database, evolutions);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to load region data: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "NeoDex loaded!" << std::endl;
    std::cout << "Pokemon count: " << database.getPokemonCount() << std::endl;

    Pokemon first = database.getPokemon(1);
    std::cout << "First Pokemon: " << first.getName() << std::endl;

    Pokemon last = database.getPokemon(database.getPokemonCount());
    std::cout << "Last Pokemon: " << last.getName() << std::endl;

    PokemonDisplay::show(PokedexSearch::searchByNumber(database, 1));

    std::vector<Pokemon> fireTypes = PokedexSearch::searchByType(database, PokemonType::Fire);
    std::cout << "Fire-type count: " << fireTypes.size() << std::endl;

    std::cout << first.getName() << " BST: " << first.getBaseStats().getTotal() << std::endl;
    std::cout << first.getName() << " cry: " << first.getCryPath() << std::endl;

    std::vector<Evolution> bulbasaurEvolutions = evolutions.getEvolutionsFrom(1);
    std::cout << "Bulbasaur evolves into pokedex #" << bulbasaurEvolutions[0].toPokedexNumber << std::endl;

    try
    {
        PokedexSearch::searchByName(database, "Missingno");
    }
    catch(const std::exception& e)
    {
        std::cout << "Expected error: " << e.what() << std::endl;
    }

    first.catchPokemon();
    std::cout << first.getName() << " caught? " << (first.isCaught() ? "Yes" : "No") << std::endl;

    return 0;
}

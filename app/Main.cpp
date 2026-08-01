#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <iostream>

#include "PokedexDatabase.h"
#include "EvolutionDatabase.h"
#include "RegionLoader.h"

using namespace NeoDex;

// Nome legivel para cada tipo de Pokemon (o enum PokemonType so tem o valor).
static const char* TypeName(PokemonType type)
{
    switch (type)
    {
        case PokemonType::None:     return "-";
        case PokemonType::Normal:   return "Normal";
        case PokemonType::Fire:     return "Fogo";
        case PokemonType::Water:    return "Agua";
        case PokemonType::Electric: return "Eletrico";
        case PokemonType::Grass:    return "Planta";
        case PokemonType::Ice:      return "Gelo";
        case PokemonType::Fighting: return "Lutador";
        case PokemonType::Poison:   return "Veneno";
        case PokemonType::Ground:   return "Terra";
        case PokemonType::Flying:   return "Voador";
        case PokemonType::Psychic:  return "Psiquico";
        case PokemonType::Bug:      return "Inseto";
        case PokemonType::Rock:     return "Pedra";
        case PokemonType::Ghost:    return "Fantasma";
        case PokemonType::Dragon:   return "Dragao";
        case PokemonType::Dark:     return "Sombrio";
        case PokemonType::Steel:    return "Aco";
        case PokemonType::Fairy:    return "Fada";
    }
    return "?";
}

// Barra de progresso simples para cada stat (0-255, faixa usual de base stats).
static void StatBar(const char* label, int value)
{
    ImGui::Text("%-16s %3d", label, value);
    ImGui::SameLine();
    float fraction = value / 255.0f;
    if (fraction > 1.0f) fraction = 1.0f;
    ImGui::ProgressBar(fraction, ImVec2(150, 0));
}

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Error: %s", SDL_GetError());
        return -1;
    }

    // OpenGL ES 3.0, necessario no Android/Termux
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Por padrao o SDL2 exige um contexto acelerado por hardware, o que
    // impede o fallback para renderizacao por software (LIBGL_ALWAYS_SOFTWARE)
    // em ambientes como o Termux:X11 onde a GPU pode nao estar disponivel.
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 0);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    // Descobre o tamanho real da tela disponivel (Termux:X11 pode ter uma
    // resolucao menor que 1280x720, ou a janela pode nao caber na area
    // visivel do split-screen). Usa um tamanho seguro baseado nisso.
    int windowWidth = 1280;
    int windowHeight = 720;
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
    {
        // Deixa uma margem de 90% para nao estourar a area visivel.
        windowWidth = (int)(displayMode.w * 0.9f);
        windowHeight = (int)(displayMode.h * 0.9f);
    }

    SDL_Window* window = SDL_CreateWindow(
        "NeoDex",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        window_flags
    );

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    // --- Carrega os dados reais do Pokedex (neodex_core) ---
    PokedexDatabase database;
    EvolutionDatabase evolutions;
    std::string loadError;

    try
    {
        RegionLoader::load("data/regions/kanto.json", database, evolutions);
    }
    catch (const std::exception& e)
    {
        loadError = e.what();
        std::cerr << "Failed to load region data: " << loadError << std::endl;
    }

    int selectedPokedexNumber = database.getPokemonCount() > 0 ? 1 : 0;

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("NeoDex", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        if (!loadError.empty())
        {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Erro ao carregar dados: %s", loadError.c_str());
        }
        else
        {
            // Coluna esquerda: lista de Pokemon
            ImGui::BeginChild("PokemonList", ImVec2(250, 0), true);
            for (int i = 1; i <= database.getPokemonCount(); ++i)
            {
                Pokemon p = database.getPokemon(i);
                std::string label = "#" + std::to_string(p.getPokedexNumber()) + " " + p.getName();
                bool isSelected = (i == selectedPokedexNumber);
                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    selectedPokedexNumber = i;
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();

            // Coluna direita: detalhes do Pokemon selecionado
            ImGui::BeginChild("PokemonDetails", ImVec2(0, 0), true);
            if (selectedPokedexNumber > 0)
            {
                Pokemon p = database.getPokemon(selectedPokedexNumber);

                ImGui::Text("#%d %s", p.getPokedexNumber(), p.getName().c_str());
                ImGui::Separator();

                if (p.getSecondaryType() == PokemonType::None)
                    ImGui::Text("Tipo: %s", TypeName(p.getPrimaryType()));
                else
                    ImGui::Text("Tipo: %s / %s", TypeName(p.getPrimaryType()), TypeName(p.getSecondaryType()));

                ImGui::Text("Altura: %.1f m    Peso: %.1f kg", p.getHeight(), p.getWeight());

                if (!p.getDescription().empty())
                {
                    ImGui::TextWrapped("%s", p.getDescription().c_str());
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Status base");
                ImGui::Spacing();

                BaseStats stats = p.getBaseStats();
                StatBar("HP", stats.getHp());
                StatBar("Ataque", stats.getAttack());
                StatBar("Defesa", stats.getDefense());
                StatBar("At. Especial", stats.getSpecialAttack());
                StatBar("Def. Especial", stats.getSpecialDefense());
                StatBar("Velocidade", stats.getSpeed());
                ImGui::Text("Total: %d", stats.getTotal());

                ImGui::Spacing();
                ImGui::Separator();

                bool caught = p.isCaught();
                if (ImGui::Checkbox("Capturado", &caught))
                {
                    // Nota: isso so afeta a copia local 'p'. Persistir o estado
                    // de captura de volta no banco de dados fica para uma
                    // proxima etapa (precisa de um setter/updatePokemon no
                    // PokedexDatabase).
                }

                // Evolucoes a partir deste Pokemon, se houver
                std::vector<Evolution> evos = evolutions.getEvolutionsFrom(p.getPokedexNumber());
                if (!evos.empty())
                {
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Text("Evolui para:");
                    for (const Evolution& evo : evos)
                    {
                        try
                        {
                            Pokemon next = database.getPokemon(evo.toPokedexNumber);
                            ImGui::BulletText("#%d %s", next.getPokedexNumber(), next.getName().c_str());
                        }
                        catch (const std::exception&)
                        {
                            ImGui::BulletText("#%d", evo.toPokedexNumber);
                        }
                    }
                }
            }
            ImGui::EndChild();
        }

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

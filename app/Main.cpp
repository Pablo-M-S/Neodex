#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <iostream>

#include "PalDatabase.h"
#include "CardDatabase.h"
#include "SetLoader.h"

using namespace NeoDex;

static const char* ElementName(PalElement element)
{
    switch (element)
    {
        case PalElement::None:     return "-";
        case PalElement::Neutral:  return "Neutral";
        case PalElement::Fire:     return "Fire";
        case PalElement::Water:    return "Water";
        case PalElement::Grass:    return "Grass";
        case PalElement::Electric: return "Electric";
        case PalElement::Ice:      return "Ice";
        case PalElement::Earth:    return "Earth";
        case PalElement::Dark:     return "Dark";
        case PalElement::Dragon:   return "Dragon";
    }
    return "?";
}

static const char* RarityName(CardRarity rarity)
{
    switch (rarity)
    {
        case CardRarity::Common:     return "Common";
        case CardRarity::Uncommon:   return "Uncommon";
        case CardRarity::Rare:       return "Rare";
        case CardRarity::SuperRare:  return "Super Rare";
        case CardRarity::UltraRare:  return "Ultra Rare";
        case CardRarity::SecretRare: return "Secret Rare";
    }
    return "?";
}

static void StatBar(const char* label, int value)
{
    ImGui::Text("%-10s %3d", label, value);
    ImGui::SameLine();
    float fraction = value / 150.0f;
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

    // OpenGL ES 3.0, required on Android/Termux
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // SDL2 requires a hardware-accelerated context by default, which blocks
    // software rendering fallback (LIBGL_ALWAYS_SOFTWARE) on setups like
    // Termux:X11 where the GPU driver path may not work.
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 0);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    int windowWidth = 1280;
    int windowHeight = 720;
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
    {
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

    // --- Load real Palworld TCG data ---
    PalDatabase pals;
    CardDatabase cards;
    std::string loadError;

    try
    {
        SetLoader::loadPals("data/pals.json", pals);
        SetLoader::loadSet("data/sets/dawn-of-palpagos.json", cards);
    }
    catch (const std::exception& e)
    {
        loadError = e.what();
        std::cerr << "Failed to load data: " << loadError << std::endl;
    }

    int selectedPalNumber = pals.getPalCount() > 0 ? 1 : 0;

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
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Failed to load data: %s", loadError.c_str());
        }
        else
        {
            // Left column: Pal list
            ImGui::BeginChild("PalList", ImVec2(250, 0), true);
            for (int i = 1; i <= pals.getPalCount(); ++i)
            {
                Pal p = pals.getPal(i);
                std::string label = "#" + std::to_string(p.getPalNumber()) + " " + p.getName();
                if (p.isCaught())
                    label += " [X]";
                bool isSelected = (i == selectedPalNumber);
                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    selectedPalNumber = i;
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();

            // Right column: selected Pal details + all its cards
            ImGui::BeginChild("PalDetails", ImVec2(0, 0), true);
            if (selectedPalNumber > 0)
            {
                Pal p = pals.getPal(selectedPalNumber);

                ImGui::Text("#%d %s", p.getPalNumber(), p.getName().c_str());
                ImGui::Separator();

                if (p.getSecondaryElement() == PalElement::None)
                    ImGui::Text("Element: %s", ElementName(p.getPrimaryElement()));
                else
                    ImGui::Text("Element: %s / %s", ElementName(p.getPrimaryElement()), ElementName(p.getSecondaryElement()));

                if (!p.getDescription().empty())
                {
                    ImGui::TextWrapped("%s", p.getDescription().c_str());
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Base Stats");
                ImGui::Spacing();

                PalStats stats = p.getStats();
                StatBar("HP", stats.getHp());
                StatBar("Attack", stats.getAttack());
                StatBar("Defense", stats.getDefense());
                ImGui::Text("Total: %d", stats.getTotal());

                bool caught = p.isCaught();
                ImGui::Spacing();
                if (ImGui::Checkbox("Caught", &caught))
                {
                    pals.setPalCaught(p.getPalNumber(), caught);
                }

                // Cards for this Pal, across every set.
                std::vector<Card> palCards = cards.getCardsForPal(p.getPalNumber());
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Cards (%d)", (int)palCards.size());
                ImGui::Spacing();

                for (const Card& card : palCards)
                {
                    ImGui::BulletText(
                        "%s - %s (%s)",
                        card.getCardId().c_str(),
                        card.getSetName().c_str(),
                        RarityName(card.getRarity())
                    );
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

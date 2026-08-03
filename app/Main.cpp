#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <iostream>
#include <algorithm>

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
        case PalElement::Earth:    return "Ground";
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
        case CardRarity::DoubleRare: return "Double Rare";
        case CardRarity::Unknown:    return "Unknown";
    }
    return "?";
}

static const char* CardTypeName(CardType type)
{
    switch (type)
    {
        case CardType::Pal:       return "Pal";
        case CardType::Gear:      return "Gear";
        case CardType::Structure: return "Structure";
        case CardType::Event:     return "Event";
        case CardType::Soul:      return "Soul";
    }
    return "?";
}

// Prints a card's stat line, e.g. "Cost 5  Power 900  Strike 2" or
// "Stats pending" when the card's numbers haven't been confirmed yet.
static void DrawCardStats(const Card& card)
{
    if (card.getCost() < 0)
    {
        ImGui::TextDisabled("Stats pending");
        return;
    }

    if (card.getCardType() == CardType::Pal)
    {
        ImGui::Text("Cost %d  Power %d  Strike %d", card.getCost(), card.getPower(), card.getStrike());
    }
    else
    {
        ImGui::Text("Cost %d", card.getCost());
    }
}

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Error: %s", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
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
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 300 es");

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
    int viewMode = 0; // 0 = Pals view, 1 = Cards view

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
            ImGui::End();
            ImGui::Render();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);
            continue;
        }

        // View switcher
        if (ImGui::RadioButton("Pals", viewMode == 0)) viewMode = 0;
        ImGui::SameLine();
        if (ImGui::RadioButton("Cards", viewMode == 1)) viewMode = 1;
        ImGui::Separator();

        if (viewMode == 0)
        {
            // --- Pals view: species list, click to see all its cards ---
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

                ImGui::Text("Min. recruit level: %d", p.getRecruitLevel());

                bool caught = p.isCaught();
                ImGui::Spacing();
                if (ImGui::Checkbox("Caught", &caught))
                {
                    pals.setPalCaught(p.getPalNumber(), caught);
                }

                std::vector<Card> palCards = cards.getCardsForPal(p.getPalNumber());
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Cards (%d)", (int)palCards.size());
                ImGui::Spacing();

                for (const Card& card : palCards)
                {
                    ImGui::BulletText(
                        "%s - %s (%s, %s)",
                        card.getCardId().c_str(),
                        card.getSubtitle().c_str(),
                        card.getSetName().c_str(),
                        RarityName(card.getRarity())
                    );
                    ImGui::Indent();
                    DrawCardStats(card);
                    ImGui::Unindent();
                }
            }
            ImGui::EndChild();
        }
        else
        {
            // --- Cards view: full catalog, ordered by card number, grouped by set ---
            ImGui::BeginChild("CardCatalog", ImVec2(0, 0), true);

            std::string currentSet = "";
            for (int i = 0; i < cards.getCardCount(); i++)
            {
                Card card = cards.getCard(i);

                if (card.getSetName() != currentSet)
                {
                    currentSet = card.getSetName();
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%s", currentSet.c_str());
                    ImGui::Separator();
                }

                std::string title = card.getCardId() + " - " + card.getCardName();
                if (!card.getSubtitle().empty())
                    title += " - " + card.getSubtitle();
                title += " [" + std::string(CardTypeName(card.getCardType())) + ", " + RarityName(card.getRarity()) + "]";

                ImGui::BulletText("%s", title.c_str());
                ImGui::Indent();
                DrawCardStats(card);
                ImGui::Unindent();
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

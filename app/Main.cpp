#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <SDL2/SDL_mixer.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <iostream>
#include <unordered_map>

#include "PalDatabase.h"
#include "CardDatabase.h"
#include "SetLoader.h"
#include "TextureLoader.h"
#include "AudioLoader.h"

using namespace NeoDex;

enum class AppState
{
    Splash,
    Menu,
    Collections,
    CollectionCards,
    Pals,
    PalDetail
};

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

static ImVec4 ElementColor(PalElement element)
{
    switch (element)
    {
        case PalElement::Neutral:  return ImVec4(0.75f, 0.72f, 0.60f, 1.0f);
        case PalElement::Fire:     return ImVec4(0.90f, 0.35f, 0.15f, 1.0f);
        case PalElement::Water:    return ImVec4(0.20f, 0.50f, 0.90f, 1.0f);
        case PalElement::Grass:    return ImVec4(0.30f, 0.70f, 0.25f, 1.0f);
        case PalElement::Electric: return ImVec4(0.90f, 0.80f, 0.15f, 1.0f);
        case PalElement::Ice:      return ImVec4(0.55f, 0.85f, 0.90f, 1.0f);
        case PalElement::Earth:    return ImVec4(0.60f, 0.45f, 0.30f, 1.0f);
        case PalElement::Dark:     return ImVec4(0.40f, 0.30f, 0.50f, 1.0f);
        case PalElement::Dragon:   return ImVec4(0.35f, 0.25f, 0.75f, 1.0f);
        default:                   return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    }
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

static void DrawCardStats(const Card& card)
{
    if (card.getCost() < 0)
    {
        ImGui::TextDisabled("Stats pending");
        return;
    }

    if (card.getCardType() == CardType::Pal)
        ImGui::Text("Cost %d  Power %d  Strike %d", card.getCost(), card.getPower(), card.getStrike());
    else
        ImGui::Text("Cost %d", card.getCost());
}

static bool DrawPalIcon(const Pal& pal, GLuint texture, bool isSelected, float cellSize)
{
    ImGui::PushID(pal.getPalNumber());
    ImGui::BeginGroup();

    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float radius = cellSize * 0.5f;
    ImVec2 center = ImVec2(cursor.x + radius, cursor.y + radius);

    bool clicked = ImGui::InvisibleButton("icon", ImVec2(cellSize, cellSize));

    if (isSelected)
        drawList->AddCircleFilled(center, radius + 3.0f, IM_COL32(255, 220, 80, 255));

    if (texture != 0)
    {
        drawList->AddImageRounded(
            (ImTextureID)(intptr_t)texture,
            cursor, ImVec2(cursor.x + cellSize, cursor.y + cellSize),
            ImVec2(0, 0), ImVec2(1, 1),
            IM_COL32_WHITE, radius
        );
    }
    else
    {
        ImVec4 color = ElementColor(pal.getPrimaryElement());
        drawList->AddCircleFilled(center, radius - 2.0f, ImGui::ColorConvertFloat4ToU32(color));
    }

    if (pal.isCaught())
        drawList->AddCircleFilled(ImVec2(cursor.x + cellSize - 8, cursor.y + 8), 6.0f, IM_COL32(80, 220, 100, 255));

    ImGui::TextWrapped("#%d", pal.getPalNumber());
    std::string name = pal.getName();
    if (name.size() > 10) name = name.substr(0, 9) + "...";
    ImGui::TextWrapped("%s", name.c_str());

    ImGui::EndGroup();
    ImGui::PopID();

    return clicked;
}

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Error: %s", SDL_GetError());
        return -1;
    }

    // Audio is optional - if it fails to open, sounds are just silently
    // skipped everywhere else in the app.
    bool audioAvailable = (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == 0);
    if (!audioAvailable)
        std::cerr << "Audio unavailable: " << Mix_GetError() << std::endl;

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

    std::unordered_map<int, GLuint> palTextures;
    for (int i = 1; i <= pals.getPalCount(); ++i)
    {
        Pal p = pals.getPal(i);
        palTextures[p.getPalNumber()] = LoadTexture(p.getSpritePath());
    }

    // Cache of loaded cry sounds, so we don't reload from disk every time
    // the same Pal's detail screen is opened.
    std::unordered_map<int, Mix_Chunk*> palCries;

    AppState state = AppState::Splash;
    int selectedPalNumber = pals.getPalCount() > 0 ? 1 : 0;
    std::string selectedSetCode;
    std::string selectedSetName;
    float splashTimer = 0.0f;
    Uint32 lastTicks = SDL_GetTicks();

    bool done = false;
    while (!done)
    {
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;
        splashTimer += deltaTime;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;

            if (state == AppState::Splash && event.type == SDL_MOUSEBUTTONDOWN)
            {
                state = AppState::Menu;
            }
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
        else if (state == AppState::Splash)
        {
            // Simple animated splash: a Pal Sphere-ish circle that pulses,
            // the title, and a blinking "Press to open" prompt.
            ImVec2 winSize = io.DisplaySize;
            ImVec2 center = ImVec2(winSize.x * 0.5f, winSize.y * 0.4f);
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            float pulse = 0.5f + 0.5f * sinf(splashTimer * 2.0f);
            float radius = 60.0f + pulse * 15.0f;

            drawList->AddCircleFilled(center, radius, IM_COL32(220, 60, 60, 255));
            drawList->AddCircle(center, radius, IM_COL32(255, 255, 255, 255), 0, 4.0f);
            drawList->AddCircleFilled(center, radius * 0.22f, IM_COL32(255, 255, 255, 255));
            drawList->AddCircle(center, radius * 0.22f, IM_COL32(20, 20, 20, 255), 0, 3.0f);

            ImGui::SetCursorPosY(winSize.y * 0.55f);
            ImGui::SetWindowFontScale(2.5f);
            std::string title = "NeoDex";
            ImVec2 textSize = ImGui::CalcTextSize(title.c_str());
            ImGui::SetCursorPosX((winSize.x - textSize.x) * 0.5f);
            ImGui::TextColored(ImVec4(1, 0.85f, 0.2f, 1), "%s", title.c_str());
            ImGui::SetWindowFontScale(1.0f);

            if (fmodf(splashTimer, 1.2f) < 0.8f)
            {
                std::string prompt = "Press to open";
                ImVec2 promptSize = ImGui::CalcTextSize(prompt.c_str());
                ImGui::SetCursorPosX((winSize.x - promptSize.x) * 0.5f);
                ImGui::SetCursorPosY(winSize.y * 0.75f);
                ImGui::TextDisabled("%s", prompt.c_str());
            }
        }
        else if (state == AppState::Menu)
        {
            ImVec2 winSize = io.DisplaySize;
            ImGui::SetCursorPosY(winSize.y * 0.35f);

            float buttonWidth = 260;
            float buttonHeight = 60;

            ImGui::SetCursorPosX((winSize.x - buttonWidth) * 0.5f);
            if (ImGui::Button("Collections", ImVec2(buttonWidth, buttonHeight)))
                state = AppState::Collections;

            ImGui::SetCursorPosX((winSize.x - buttonWidth) * 0.5f);
            ImGui::Dummy(ImVec2(0, 12));
            ImGui::SetCursorPosX((winSize.x - buttonWidth) * 0.5f);
            if (ImGui::Button("Pals", ImVec2(buttonWidth, buttonHeight)))
                state = AppState::Pals;
        }
        else if (state == AppState::Collections)
        {
            if (ImGui::Button("< Back")) state = AppState::Menu;
            ImGui::Separator();
            ImGui::Text("Expansions");
            ImGui::Spacing();

            std::set<std::string> seenSets;
            for (int i = 0; i < cards.getCardCount(); i++)
            {
                Card card = cards.getCard(i);
                if (seenSets.count(card.getSetCode()) == 0)
                {
                    seenSets.insert(card.getSetCode());
                    std::string label = card.getSetName() + " (" + card.getSetCode() + ")";
                    if (ImGui::Selectable(label.c_str(), false, 0, ImVec2(0, 40)))
                    {
                        selectedSetCode = card.getSetCode();
                        selectedSetName = card.getSetName();
                        state = AppState::CollectionCards;
                    }
                }
            }
        }
        else if (state == AppState::CollectionCards)
        {
            if (ImGui::Button("< Back")) state = AppState::Collections;
            ImGui::SameLine();
            ImGui::Text("%s", selectedSetName.c_str());
            ImGui::Separator();

            ImGui::BeginChild("SetCards", ImVec2(0, 0), true);
            std::vector<Card> setCards = cards.getCardsForSet(selectedSetCode);
            for (const Card& card : setCards)
            {
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
        else if (state == AppState::Pals)
        {
            if (ImGui::Button("< Back")) state = AppState::Menu;
            ImGui::Separator();

            float cellSize = 64.0f;
            float cellStride = cellSize + 16.0f;
            float availWidth = ImGui::GetContentRegionAvail().x;
            int columns = (int)(availWidth / cellStride);
            if (columns < 1) columns = 1;

            for (int i = 1; i <= pals.getPalCount(); ++i)
            {
                Pal p = pals.getPal(i);

                if (DrawPalIcon(p, palTextures[p.getPalNumber()], false, cellSize))
                {
                    selectedPalNumber = i;
                    state = AppState::PalDetail;

                    // Lazily load + play this Pal's cry the moment its
                    // detail screen opens.
                    if (audioAvailable)
                    {
                        if (palCries.find(p.getPalNumber()) == palCries.end())
                        {
                            palCries[p.getPalNumber()] = LoadSound(p.getCryPath());
                        }
                        PlaySound(palCries[p.getPalNumber()]);
                    }
                }

                if (i % columns != 0 && i != pals.getPalCount())
                    ImGui::SameLine();
            }
        }
        else if (state == AppState::PalDetail)
        {
            if (ImGui::Button("< Back")) state = AppState::Pals;
            ImGui::Separator();

            Pal p = pals.getPal(selectedPalNumber);
            GLuint texture = palTextures[p.getPalNumber()];

            ImVec2 winSize = io.DisplaySize;
            float bigSize = 200.0f;

            ImGui::SetCursorPosX((winSize.x - bigSize) * 0.5f);
            ImVec2 cursor = ImGui::GetCursorScreenPos();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 center = ImVec2(cursor.x + bigSize * 0.5f, cursor.y + bigSize * 0.5f);

            if (texture != 0)
            {
                drawList->AddImageRounded(
                    (ImTextureID)(intptr_t)texture,
                    cursor, ImVec2(cursor.x + bigSize, cursor.y + bigSize),
                    ImVec2(0, 0), ImVec2(1, 1),
                    IM_COL32_WHITE, bigSize * 0.5f
                );
            }
            else
            {
                drawList->AddCircleFilled(center, bigSize * 0.5f, ImGui::ColorConvertFloat4ToU32(ElementColor(p.getPrimaryElement())));
            }
            ImGui::Dummy(ImVec2(bigSize, bigSize));

            ImGui::SetWindowFontScale(1.6f);
            std::string title = "#" + std::to_string(p.getPalNumber()) + " " + p.getName();
            ImVec2 titleSize = ImGui::CalcTextSize(title.c_str());
            ImGui::SetCursorPosX((winSize.x - titleSize.x) * 0.5f);
            ImGui::Text("%s", title.c_str());
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Spacing();
            if (p.getSecondaryElement() == PalElement::None)
                ImGui::Text("Element: %s", ElementName(p.getPrimaryElement()));
            else
                ImGui::Text("Element: %s / %s", ElementName(p.getPrimaryElement()), ElementName(p.getSecondaryElement()));

            ImGui::Text("Min. recruit level: %d", p.getRecruitLevel());

            bool caught = p.isCaught();
            ImGui::Spacing();
            if (ImGui::Checkbox("Caught", &caught))
                pals.setPalCaught(p.getPalNumber(), caught);

            if (audioAvailable && ImGui::Button("Play cry"))
            {
                if (palCries.find(p.getPalNumber()) == palCries.end())
                    palCries[p.getPalNumber()] = LoadSound(p.getCryPath());
                PlaySound(palCries[p.getPalNumber()]);
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

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    for (auto& pair : palCries)
    {
        if (pair.second != nullptr)
            Mix_FreeChunk(pair.second);
    }

    if (audioAvailable)
        Mix_CloseAudio();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

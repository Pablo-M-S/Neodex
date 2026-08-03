cmake_minimum_required(VERSION 3.16)
project(NeoDex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# neodex_core: all Pokedex domain logic (Pokemon, stats, database, search,
# evolutions, region/generation data...) as a standalone static library with
# ZERO dependency on any UI framework.
#
# This is the piece that will eventually get reused by a real mobile UI:
#   - Android  -> wrapped via JNI and called from Kotlin/Compose
#   - iOS      -> wrapped via a thin Objective-C++/Swift bridge
# The "app/" folder (still empty) is where that platform-specific layer
# will live later; it will link against this same library instead of
# duplicating any Pokemon logic.
# ---------------------------------------------------------------------------

file(GLOB NEODEX_CORE_SOURCES CONFIGURE_DEPENDS "core/src/*.cpp")
list(REMOVE_ITEM NEODEX_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/core/src/Main.cpp")

add_library(neodex_core STATIC ${NEODEX_CORE_SOURCES})

target_include_directories(neodex_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)

# ---------------------------------------------------------------------------
# neodex_cli: temporary console entry point, used to test neodex_core while
# there is no graphical UI yet. Not meant to ship — once the Android/iOS
# app layer exists, this target can be dropped or kept just for dev testing.
# ---------------------------------------------------------------------------

add_executable(neodex_cli core/src/Main.cpp)
target_link_libraries(neodex_cli PRIVATE neodex_core)

# Copy data/ (region JSON files) next to the built binary so
# RegionLoader::load("data/regions/kanto.json", ...) finds them at runtime.
add_custom_command(TARGET neodex_cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:neodex_cli>/data
)

# ---------------------------------------------------------------------------
# neodex_gui: graphical entry point using Dear ImGui + SDL2 + OpenGL ES.
# Links against the same neodex_core library used by neodex_cli, so all the
# Pokedex domain logic stays shared between both entry points.
# ---------------------------------------------------------------------------

find_package(SDL2 REQUIRED)
# Nota: no Android/Termux nao existe OpenGL desktop (libGL), so OpenGL ES.
# Por isso linkamos direto com GLESv2/EGL em vez de usar find_package(OpenGL).

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(IMGUI_BACKENDS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui_backends)
set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

add_executable(neodex_gui app/Main.cpp app/TextureLoader.cpp app/AudioLoader.cpp ${IMGUI_SOURCES})

target_include_directories(neodex_gui PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR}
    ${STB_DIR}
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(neodex_gui PRIVATE
    neodex_core
    ${SDL2_LIBRARIES}
    SDL2_mixer
    GLESv2
    EGL
)

# Forca o backend do ImGui a usar o loader de OpenGL ES 3.0 (evita deteccao
# automatica errada, que por padrao assume OpenGL desktop).
target_compile_definitions(neodex_gui PRIVATE IMGUI_IMPL_OPENGL_ES3)

add_custom_command(TARGET neodex_gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:neodex_gui>/data
)

add_custom_command(TARGET neodex_gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/external/fonts
        $<TARGET_FILE_DIR:neodex_gui>/external/fonts
)

#pragma once

#include <string>
#include <SDL2/SDL_mixer.h>

namespace NeoDex
{

// Loads a sound effect (WAV/OGG/etc). Returns nullptr if the file doesn't
// exist or fails to load - callers should treat that as "no sound", not
// as a fatal error, since most Pal cry files don't exist yet.
Mix_Chunk* LoadSound(const std::string& path);

// Plays a previously loaded chunk on the first free channel. Safe to call
// with nullptr (does nothing).
void PlaySound(Mix_Chunk* chunk);

}

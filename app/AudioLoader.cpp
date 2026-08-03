#include "AudioLoader.h"

namespace NeoDex
{

Mix_Chunk* LoadSound(const std::string& path)
{
    if (path.empty())
        return nullptr;

    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    // Mix_LoadWAV returns nullptr on failure (missing file, unsupported
    // format, etc) - that's fine, caller just won't play a sound.
    return chunk;
}


void PlaySound(Mix_Chunk* chunk)
{
    if (chunk == nullptr)
        return;

    Mix_PlayChannel(-1, chunk, 0);
}

}

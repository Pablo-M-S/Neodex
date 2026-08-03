#pragma once

#include <string>
#include <SDL2/SDL_opengles2.h>

namespace NeoDex
{

// Loads an image file (PNG/JPG/etc, via stb_image) into an OpenGL texture.
// Returns 0 if the file doesn't exist or fails to load - callers should
// fall back to a placeholder in that case instead of treating it as fatal.
GLuint LoadTexture(const std::string& path, int* outWidth = nullptr, int* outHeight = nullptr);

}

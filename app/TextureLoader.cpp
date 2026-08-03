#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "TextureLoader.h"

namespace NeoDex
{

GLuint LoadTexture(const std::string& path, int* outWidth, int* outHeight)
{
    int width, height, channels;
    // Force 4 channels (RGBA) so we don't have to special-case RGB/greyscale PNGs.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr)
    {
        // File missing or unreadable - not fatal, caller falls back to a placeholder.
        return 0;
    }

    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    if (outWidth) *outWidth = width;
    if (outHeight) *outHeight = height;

    return textureId;
}

}

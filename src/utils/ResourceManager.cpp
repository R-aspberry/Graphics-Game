
#include "ResourceManager.h"

#include <iostream>
#include "stb_image.h"

std::unordered_map<std::string, unsigned int>
    ResourceManager::textures;

unsigned int ResourceManager::loadTexture(const std::string& path)
{
    // Reuse an existing texture if it has already been loaded.
    auto existing = textures.find(path);

    if (existing != textures.end())
    {
        return existing->second;
    }

    int width = 0;
    int height = 0;
    int channels = 0;

    // Force four channels so PNG transparency is preserved.
    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixels = stbi_load(
        path.c_str(),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (pixels == nullptr)
    {
        std::cerr << "Failed to load texture: " << path << '\n';
        return 0;
    }

    unsigned int texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    stbi_image_free(pixels);

    textures[path] = texture;

    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Loaded texture: " << path << '\n';

    return texture;
}

void ResourceManager::clear()
{
    for (const auto& entry : textures)
    {
        glDeleteTextures(1, &entry.second);
    }

    textures.clear();
}

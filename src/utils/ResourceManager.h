
#pragma once

#include <glad/gl.h>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    static unsigned int loadTexture(const std::string& path);

    static void clear();

private:
    static std::unordered_map<std::string, unsigned int> textures;
};

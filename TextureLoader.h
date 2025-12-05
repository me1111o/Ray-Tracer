#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <string>
#include <vector>
#include <map>
#include "PPMImageLoader.h"
#include <glm/glm.hpp>

struct Texture
{
    int width;
    int height;
    std::vector<unsigned char> data;
};

Texture loadTexturePPM(const std::string &filename);

glm::vec3 sampleTexture(const Texture &tex, const glm::vec2 &uv);

extern std::map<std::string, Texture> g_textures;

void loadAllTextures();

#endif

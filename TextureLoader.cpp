#include "TextureLoader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::map<std::string, Texture> g_textures;

Texture loadTexturePPM(const std::string &filename)
{
    PPMImageLoader loader;
    loader.load(filename);
    Texture tex;
    tex.width = loader.getWidth();
    tex.height = loader.getHeight();
    int totalBytes = 3 * tex.width * tex.height;
    tex.data = std::vector<unsigned char>(loader.getImageData(), loader.getImageData() + totalBytes);
    return tex;
}

void loadAllTextures()
{
    g_textures["earthmap"] = loadTexturePPM("textures/earthmap.ppm");
    g_textures["checkerboard"] = loadTexturePPM("textures/checkerboard.ppm");
    g_textures["die"] = loadTexturePPM("textures/moon.ppm");
}

glm::vec3 sampleTexture(const Texture &tex, const glm::vec2 &uv)
{
    float u = glm::clamp(uv.x, 0.0f, 1.0f);
    float v = glm::clamp(uv.y, 0.0f, 1.0f);
    int i = static_cast<int>(u * (tex.width - 1));
    int j = static_cast<int>((1.0f - v) * (tex.height - 1));
    int index = 3 * (j * tex.width + i);
    return glm::vec3(tex.data[index] / 255.0f,
                     tex.data[index + 1] / 255.0f,
                     tex.data[index + 2] / 255.0f);
}

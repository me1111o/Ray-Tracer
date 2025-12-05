#ifndef RAY_SCENE_PARSER_H
#define RAY_SCENE_PARSER_H

#include "RayTracer.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

struct RaySceneNode
{
    std::string name;
    glm::mat4 transform;
    std::string nodeType;
    std::string instanceType;
    Material material;
    std::vector<RaySceneNode *> children;

    RaySceneNode() : transform(glm::mat4(1.0f)) {}
    ~RaySceneNode()
    {
        for (auto child : children)
            delete child;
    }
};

class RaySceneParser
{
public:
    static RaySceneNode *parseSceneFile(const std::string &fileName, std::vector<Light> &lights);

    static std::vector<Hittable *> extractHittables(RaySceneNode *root);
};

#endif

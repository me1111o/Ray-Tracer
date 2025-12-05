#include "RaySceneParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

static std::map<std::string, Material> g_materials;
static std::map<std::string, RaySceneNode *> g_nodes;

RaySceneNode *RaySceneParser::parseSceneFile(const std::string &fileName, std::vector<Light> &lights)
{
    std::ifstream infile(fileName.c_str());
    if (!infile)
    {
        std::cerr << "Can't open the file: " << fileName << std::endl;
        return nullptr;
    }
    std::string line;
    RaySceneNode *rootNode = nullptr;
    while (std::getline(infile, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "instance")
        {

            std::string instName, filename;
            iss >> instName >> filename;

            if (g_nodes.find(instName) == g_nodes.end())
            {
                RaySceneNode *node = new RaySceneNode();
                node->name = instName;
                g_nodes[instName] = node;
            }
        }
        else if (cmd == "material")
        {

            std::string matName;
            iss >> matName;
            Material m;

            while (std::getline(infile, line))
            {
                if (line.empty() || line[0] == '#')
                    continue;
                std::istringstream mss(line);
                std::string key;
                mss >> key;
                if (key == "end-material")
                    break;
                else if (key == "emission")
                {
                    float r, g, b;
                    mss >> r >> g >> b;
                }
                else if (key == "ambient")
                {
                    float r, g, b;
                    mss >> r >> g >> b;
                    m.ambient = glm::vec3(r, g, b);
                }
                else if (key == "diffuse")
                {
                    float r, g, b;
                    mss >> r >> g >> b;
                    m.diffuse = glm::vec3(r, g, b);
                }
                else if (key == "specular")
                {
                    float r, g, b;
                    mss >> r >> g >> b;
                    m.specular = glm::vec3(r, g, b);
                }
                else if (key == "shininess")
                {
                    float shin;
                    mss >> shin;
                    m.shininess = shin;
                }
                else if (key == "absorption")
                {
                    float a;
                    mss >> a;
                    m.absorption = a;
                }
                else if (key == "reflection")
                {
                    float r;
                    mss >> r;
                    m.reflection = r;
                }
                else if (key == "transparency")
                {
                    float t;
                    mss >> t;
                    m.transparency = t;
                }
            }
            g_materials[matName] = m;
        }
        else if (cmd == "translate")
        {

            std::string nodeName1, nodeName2;
            float tx, ty, tz;
            iss >> nodeName1 >> nodeName2 >> tx >> ty >> tz;
            RaySceneNode *node = new RaySceneNode();
            node->name = nodeName1;
            node->nodeType = "translate";
            node->transform = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz));
            g_nodes[nodeName1] = node;
        }
        else if (cmd == "group")
        {
            std::string nodeName, dummy;
            iss >> nodeName >> dummy;
            RaySceneNode *node = new RaySceneNode();
            node->name = nodeName;
            node->nodeType = "group";
            node->transform = glm::mat4(1.0f);
            g_nodes[nodeName] = node;
        }
        else if (cmd == "scale")
        {

            std::string nodeName1, nodeName2;
            float sx, sy, sz;
            iss >> nodeName1 >> nodeName2 >> sx >> sy >> sz;
            RaySceneNode *node = new RaySceneNode();
            node->name = nodeName1;
            node->nodeType = "scale";
            node->transform = glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
            g_nodes[nodeName1] = node;
        }
        else if (cmd == "leaf")
        {

            std::string nodeName1, nodeName2, token, type;
            iss >> nodeName1 >> nodeName2 >> token >> type;
            RaySceneNode *node = new RaySceneNode();
            node->name = nodeName1;
            node->nodeType = "leaf";
            node->instanceType = type;
            g_nodes[nodeName1] = node;
        }
        else if (cmd == "assign-material")
        {

            std::string nodeName, matName;
            iss >> nodeName >> matName;
            if (g_nodes.find(nodeName) != g_nodes.end() && g_materials.find(matName) != g_materials.end())
            {
                g_nodes[nodeName]->material = g_materials[matName];
            }
        }
        else if (cmd == "assign-texture")
        {
        }
        else if (cmd == "add-child")
        {

            std::string childName, parentName;
            iss >> childName >> parentName;
            if (g_nodes.find(childName) != g_nodes.end() && g_nodes.find(parentName) != g_nodes.end())
            {
                g_nodes[parentName]->children.push_back(g_nodes[childName]);
            }
        }
        else if (cmd == "assign-root")
        {

            std::string rootName;
            iss >> rootName;
            if (g_nodes.find(rootName) != g_nodes.end())
                rootNode = g_nodes[rootName];
        }

        else if (cmd == "light")
        {
            std::string lightName;
            iss >> lightName;
            Light l;
            l.ambient = glm::vec3(0.0f);
            l.diffuse = glm::vec3(0.0f);
            l.specular = glm::vec3(0.0f);
            l.position = glm::vec3(0.0f);
            bool parsingLight = true;
            while (parsingLight && std::getline(infile, line))
            {
                if (line.empty())
                    continue;
                std::istringstream lightIss(line);
                std::string keyword;
                lightIss >> keyword;
                if (keyword == "end-light")
                {
                    parsingLight = false;
                    break;
                }
                else if (keyword == "ambient")
                {
                    float r, g, b;
                    lightIss >> r >> g >> b;
                    l.ambient = glm::vec3(r, g, b);
                }
                else if (keyword == "diffuse")
                {
                    float r, g, b;
                    lightIss >> r >> g >> b;
                    l.diffuse = glm::vec3(r, g, b);
                }
                else if (keyword == "specular")
                {
                    float r, g, b;
                    lightIss >> r >> g >> b;
                    l.specular = glm::vec3(r, g, b);
                }
                else if (keyword == "position")
                {
                    float x, y, z;
                    lightIss >> x >> y >> z;
                    l.position = glm::vec3(x, y, z);
                }
            }
            lights.push_back(l);
        }
    }
    return rootNode;
}

static void traverseScene(RaySceneNode *node, const glm::mat4 &accTransform, std::vector<Hittable *> &objects, const std::string &instanceName)
{
    glm::mat4 currentTransform = accTransform * node->transform;

    if (node->nodeType == "leaf")
    {
        if (node->instanceType == "sphere")
        {

            glm::vec4 center4 = currentTransform * glm::vec4(0, 0, 0, 1);
            glm::vec3 center(center4);
            glm::vec4 rVec = currentTransform * glm::vec4(1.2, 0, 0, 0);
            float radius = glm::length(glm::vec3(rVec));
            Sphere *s = new Sphere(center, radius, node->material);
            objects.push_back(s);
        }
        else if (node->instanceType == "box")
        {

            std::vector<glm::vec3> corners;
            for (int x = -1; x <= 1; x += 2)
                for (int y = -1; y <= 1; y += 2)
                    for (int z = -1; z <= 1; z += 2)
                    {
                        glm::vec4 pt = currentTransform * glm::vec4(0.5f * x, 0.5f * y, 0.5f * z, 1.0f);
                        corners.push_back(glm::vec3(pt));
                    }
            glm::vec3 minPt = corners[0], maxPt = corners[0];
            for (const auto &pt : corners)
            {
                minPt = glm::min(minPt, pt);
                maxPt = glm::max(maxPt, pt);
            }
            Box *b = new Box(minPt, maxPt, node->material);
            objects.push_back(b);
        }
        else if (node->instanceType == "cylinder")
        {
            glm::vec4 center4 = currentTransform * glm::vec4(0, 0, 0, 1);
            glm::vec3 center(center4);

            glm::vec4 xVec4 = currentTransform * glm::vec4(1, 0, 0, 0);
            glm::vec3 xVec(xVec4);
            float radius = glm::length(xVec);

            glm::vec4 yVec4 = currentTransform * glm::vec4(0, 1, 0, 0);
            glm::vec3 yVec(yVec4);
            float height = glm::length(yVec);

            Cylinder *c = new Cylinder(center, radius, height, node->material);
            objects.push_back(c);
        }
        else if (node->instanceType == "cone")
        {
            glm::vec4 center4 = currentTransform * glm::vec4(0, 0, 0, 1);
            glm::vec3 center(center4);

            glm::vec4 xVec4 = currentTransform * glm::vec4(1, 0, 0, 0);
            glm::vec3 xVec(xVec4);
            float radius = glm::length(xVec);

            glm::vec4 yVec4 = currentTransform * glm::vec4(0, 1, 0, 0);
            glm::vec3 yVec(yVec4);
            float height = glm::length(yVec);

            Cone *cone = new Cone(center, radius, height, node->material);
            objects.push_back(cone);
        }
    }

    for (auto child : node->children)
    {
        traverseScene(child, currentTransform, objects, instanceName);
    }
}

std::vector<Hittable *> RaySceneParser::extractHittables(RaySceneNode *root)
{
    std::vector<Hittable *> objects;
    if (!root)
        return objects;
    glm::mat4 identity = glm::mat4(1.0f);
    traverseScene(root, identity, objects, root->name);
    return objects;
}

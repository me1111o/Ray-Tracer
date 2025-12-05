#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"
#include "sgraph/TextScenegraphRenderer.h"

#include "RayTracer.h"
#include "RaySceneParser.h"
#include <fstream>
#include <vector>
#include <glm/gtx/string_cast.hpp>
#include "TextureLoader.h"

Controller::Controller(Model &m, View &v)
{
    model = m;
    view = v;

    dronePosition = glm::vec3(50, 100, 50);
    droneRotation = glm::vec3(0, 0, 0);
    movementScale = 0.5f;

    initScenegraph();
}

void Controller::initScenegraph()
{

    // read in the file of commands
    ifstream inFile("scenegraphmodels/simple.txt");
    // ifstream inFile("tryout.txt");
    sgraph::ScenegraphImporter importer;

    IScenegraph *scenegraph = importer.parse(inFile);
    // scenegraph->setMeshes(meshes);
    model.setScenegraph(scenegraph);

    auto nodesMap = scenegraph->getNodes();
    std::cout << "=== Nodes in Scenegraph ===" << std::endl;

    /*


    auto itL = nodesMap.find("r-blade-pivot-L");
    if (itL == nodesMap.end())
    {
        throw std::runtime_error("Can't find r-blade-pivot-L in nodes mapping");
    }
    propellerNodeLeft = dynamic_cast<sgraph::RotateTransform *>(itL->second);
    if (!propellerNodeLeft)
    {
        throw std::runtime_error("Found r-blade-pivot-L, but it's not a RotateTransform");
    }

    auto itR = nodesMap.find("r-blade-pivot-R");
    if (itR == nodesMap.end())
    {
        throw std::runtime_error("Can't find r-blade-pivot-R in nodes mapping");
    }
    propellerNodeRight = dynamic_cast<sgraph::RotateTransform *>(itR->second);
    if (!propellerNodeRight)
    {
        throw std::runtime_error("Found r-blade-pivot-R, but it's not a RotateTransform");
    }

    auto itRollX = nodesMap.find("roll-node-x");
    if (itRollX == nodesMap.end())
    {
        throw std::runtime_error("Can't find roll-node-x in nodes mapping");
    }
    rollNodeX = dynamic_cast<sgraph::RotateTransform *>(itRollX->second);
    if (!rollNodeX)
    {
        throw std::runtime_error("Found roll-node-x, but it's not a RotateTransform");
    }

    auto itRollY = nodesMap.find("roll-node-y");
    if (itRollY == nodesMap.end())
    {
        throw std::runtime_error("Can't find roll-node-y in nodes mapping");
    }
    rollNodeY = dynamic_cast<sgraph::RotateTransform *>(itRollY->second);
    if (!rollNodeY)
    {
        throw std::runtime_error("Found roll-node-y, but it's not a RotateTransform");
    }

    auto itRollZ = nodesMap.find("roll-node-z");
    if (itRollZ == nodesMap.end())
    {
        throw std::runtime_error("Can't find roll-node-z in nodes mapping");
    }
    rollNodeZ = dynamic_cast<sgraph::RotateTransform *>(itRollZ->second);
    if (!rollNodeZ)
    {
        throw std::runtime_error("Found roll-node-z, but it's not a RotateTransform");
    }

    auto itT = nodesMap.find("t-drone");
    if (itT == nodesMap.end())
    {
        throw std::runtime_error("Can't find t-drone in nodes mapping");
    }
    droneTranslate = dynamic_cast<sgraph::TranslateTransform *>(itT->second);
    if (!droneTranslate)
    {
        throw std::runtime_error("Found t-drone, but it's not a TranslateTransform");
    }
    */
    cout << "Scenegraph made" << endl;
}

Controller::~Controller()
{
}

void Controller::run()
{
    ifstream inFile("scenegraphmodels/simple.txt");
    sgraph::ScenegraphImporter importer;
    IScenegraph *scenegraph = importer.parse(inFile);
    model.setScenegraph(scenegraph);

    map<string, util::PolygonMesh<VertexAttrib>> meshes = scenegraph->getMeshes();

    std::map<std::string, std::string> textureMap = importer.getImagesMap();

    sgraph::TextScenegraphRenderer textRenderer;
    scenegraph->getRoot()->accept(&textRenderer);
    cout << textRenderer.getOutput() << endl;

    view.init(this, meshes, textureMap);

    double lastTime = glfwGetTime();

    while (!view.shouldWindowClose())
    {
        view.setDroneState(dronePosition, droneRotation);
        view.display(scenegraph);

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        propellerAngle += propellerSpeed * deltaTime;
        if (propellerNodeLeft)
            propellerNodeLeft->setAngleInRadians(propellerAngle);

        if (propellerNodeRight)
            propellerNodeRight->setAngleInRadians(propellerAngle);

        if (rollNodeX)
            rollNodeX->setAngleInRadians(droneRotation.x);

        if (rollNodeY)
            rollNodeY->setAngleInRadians(droneRotation.y);

        if (rollNodeZ)
            rollNodeZ->setAngleInRadians(droneRotation.z);

        if (droneTranslate)
        {
            droneTranslate->updateTranslate(dronePosition);
        }

        // update animation
        if (isRolling && rollNodeX)
        {
            rollAngle += rollSpeed * deltaTime;
            if (rollAngle >= 2 * M_PI)
            {
                rollAngle = 0.0;
                isRolling = false;
            }
            rollNodeX->setAngleInRadians(rollAngle);
        }
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if ((char)key == 'S' || (char)key == 's')
        {
            std::cout << "Transfer into RayTracing, creat a PPM file..." << std::endl;
            renderRayTracing();
        }
    }

    if (action == GLFW_PRESS)
    {
        if (key == '1')
        {
            cameraMode = 1;
            view.setCameraMode(cameraMode);
            std::cout << "Camera mode 1 selected" << std::endl;
        }
        else if (key == '2')
        {
            cameraMode = 2;
            view.setCameraMode(cameraMode);
            std::cout << "Camera mode 2 selected" << std::endl;
        }
        else if (key == '3')
        {
            cameraMode = 3;
            view.setCameraMode(cameraMode);
            std::cout << "Camera mode 3 selected" << std::endl;
        }
    }

    cout << (char)key << " pressed" << endl;
}

void Controller::startRollAnimation()
{
    isRolling = true;
    rollAngle = 0.0f;
    rollSpeed = glm::radians(180.0f);
}

void Controller::reshape(int width, int height)
{
    cout << "Window reshaped to width=" << width << " and height=" << height << endl;
    glViewport(0, 0, width, height);
}

void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void Controller::renderRayTracing()
{
    loadAllTextures();

    const int width = 800;
    const int height = 800;
    std::vector<unsigned char> image(width * height * 3, 0);

    glm::vec3 baseTarget(0.0f, 0.0f, 0.0f);

    glm::vec3 baseCameraPos(200.0f, 250.0f, 250.0f);

    glm::vec3 origPos = baseCameraPos - baseTarget;

    glm::mat4 R = glm::inverse(view.getTrackball()->getRotationMatrix());
    glm::vec3 rotatedPos = glm::vec3(R * glm::vec4(origPos, 1.0f));
    glm::vec3 cameraPos = baseTarget + rotatedPos;

    glm::vec3 dir = glm::normalize(baseTarget - cameraPos);
    float focalLength = glm::length(baseTarget - cameraPos);

    float fov = glm::radians(60.0f);
    float viewportHeight = 2.0f * focalLength * tan(fov / 2.0f);
    float aspect = float(width) / float(height);
    float viewportWidth = viewportHeight * aspect;

    glm::vec3 viewPlaneCenter = cameraPos + focalLength * dir;

    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 u = glm::normalize(glm::cross(dir, up));
    glm::vec3 v = glm::normalize(glm::cross(u, dir));

    glm::vec3 lowerLeftCorner = viewPlaneCenter - (viewportWidth / 2.0f) * u - (viewportHeight / 2.0f) * v;

    std::cout << "CameraPos: " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << std::endl;
    std::cout << "Target: " << baseTarget.x << ", " << baseTarget.y << ", " << baseTarget.z << std::endl;
    std::cout << "FocalLength: " << focalLength << std::endl;
    std::cout << "Viewport: " << viewportWidth << " x " << viewportHeight << std::endl;
    std::cout << "LowerLeftCorner: " << lowerLeftCorner.x << ", " << lowerLeftCorner.y << ", " << lowerLeftCorner.z << std::endl;

    std::vector<Light> lights;
    RaySceneNode *root = RaySceneParser::parseSceneFile("scenegraphmodels/simple.txt", lights);
    if (!root)
    {
        std::cerr << "Scene graph fail, root is empty." << std::endl;
        return;
    }
    std::cout << "Num of light: " << lights.size() << std::endl;
    std::vector<Hittable *> objects = RaySceneParser::extractHittables(root);

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            float u_coord = float(i) / (width - 1);
            float v_coord = float(j) / (height - 1);
            glm::vec3 pixelPos = lowerLeftCorner + u_coord * u * viewportWidth + v_coord * v * viewportHeight;
            Ray r;
            r.origin = cameraPos;
            r.direction = glm::normalize(pixelPos - cameraPos);
            glm::vec3 color = traceRay(r, objects, lights, 5);
            int idx = 3 * ((height - 1 - j) * width + i);
            image[idx] = static_cast<unsigned char>(255.99f * color.r);
            image[idx + 1] = static_cast<unsigned char>(255.99f * color.g);
            image[idx + 2] = static_cast<unsigned char>(255.99f * color.b);
        }
    }

    std::ofstream ofs("raytraced.ppm", std::ios::binary);
    ofs << "P6\n"
        << width << " " << height << "\n255\n";
    ofs.write(reinterpret_cast<char *>(image.data()), image.size());
    ofs.close();
    std::cout << "Saved as raytraced.ppm" << std::endl;

    for (auto obj : objects)
        delete obj;
    delete root;
}
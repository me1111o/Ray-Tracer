#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "VertexAttrib.h"
#include "Light.h"
#include <set>
#include "PPMImageLoader.h"

void collectLights(sgraph::SGNode *node, const glm::mat4 &currentTransform, std::vector<util::Light> &lightsOut)
{

    sgraph::AbstractSGNode *absNode = dynamic_cast<sgraph::AbstractSGNode *>(node);
    if (absNode)
    {
        const std::vector<util::Light> &nodeLights = absNode->getLights();
        for (const util::Light &light : nodeLights)
        {
            util::Light transformedLight = light;

            transformedLight.setPosition(currentTransform * light.getPosition());

            if (light.getSpotCutoff() < 180.0f)
            {
                glm::vec4 newDir = currentTransform * light.getSpotDirection();
                transformedLight.setSpotDirection(newDir.x, newDir.y, newDir.z);
            }
            lightsOut.push_back(transformedLight);
        }
    }

    sgraph::ParentSGNode *parentNode = dynamic_cast<sgraph::ParentSGNode *>(node);
    if (parentNode)
    {
        const std::vector<sgraph::SGNode *> &children = parentNode->getChildren();
        for (sgraph::SGNode *child : children)
        {
            glm::mat4 childTransform = glm::mat4(1.0f);
            sgraph::TransformNode *transNode = dynamic_cast<sgraph::TransformNode *>(child);
            if (transNode)
            {
                childTransform = transNode->getTransform();
            }
            collectLights(child, currentTransform * childTransform, lightsOut);
        }
    }
}

void View::handleMouseButton(int button, int action, int mods)
{
    // std::cout << "Mouse button event: " << button << " " << action << std::endl;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            trackball->mouseDown((float)x, (float)y);
        }
        else if (action == GLFW_RELEASE)
        {
            trackball->mouseUp();
        }
    }
}

void View::handleCursorPos(double xpos, double ypos)
{
    trackball->mouseMove((float)xpos, (float)ypos);
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    // std::cout << "MouseButton: button=" << button << " action=" << action << std::endl;
    AppData *appData = reinterpret_cast<AppData *>(glfwGetWindowUserPointer(window));
    if (appData && appData->view)
        appData->view->handleMouseButton(button, action, mods);
}

static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    // std::cout << "CursorPos: x=" << xpos << " y=" << ypos << std::endl;
    AppData *appData = reinterpret_cast<AppData *>(glfwGetWindowUserPointer(window));
    if (appData && appData->view)
        appData->view->handleCursorPos(xpos, ypos);
}

View::View()
{
}

View::~View()
{
}

void View::init(Callbacks *callbacks, std::map<std::string, util::PolygonMesh<VertexAttrib>> &meshes,
                const std::map<std::string, std::string> &imagesMap)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 800, "Hello GLFW: Per-vertex coloring", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    AppData *appData = new AppData();
    appData->view = this;
    appData->callbacks = callbacks;
    glfwSetWindowUserPointer(window, appData);

    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    // using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window,
                       [](GLFWwindow *window, int key, int scancode, int action, int mods)
                       {
                           AppData *appData = reinterpret_cast<AppData *>(glfwGetWindowUserPointer(window));
                           if (appData && appData->callbacks)
                               appData->callbacks->onkey(key, scancode, action, mods);
                       });

    glfwSetWindowSizeCallback(window,
                              [](GLFWwindow *window, int width, int height)
                              {
                                  AppData *appData = reinterpret_cast<AppData *>(glfwGetWindowUserPointer(window));
                                  if (appData && appData->callbacks)
                                      appData->callbacks->reshape(width, height);
                              });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // create the shader program
    program.createProgram(string("shaders/default.vert"),
                          string("shaders/default.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    /* In the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.

       We create such a shader variable -> vertex attribute mapping now
     */
    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";

    for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
         it != meshes.end();
         it++)
    {
        util::ObjectInstance *obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs, it->second);
        objects[it->first] = obj;
    }

    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    // prepare the projection matrix for perspective projection
    projection = glm::perspective(glm::radians(60.0f), (float)window_width / window_height, 0.1f, 10000.0f);
    glViewport(0, 0, window_width, window_height);

    frames = 0;
    time = glfwGetTime();

    for (const auto &kv : imagesMap)
    {
        const std::string &texName = kv.first;
        const std::string &filePath = kv.second;

        PPMImageLoader loader;
        try
        {
            loader.load(filePath);
        }
        catch (std::exception &e)
        {
            std::cerr << "Error loading texture: " << filePath << " (" << e.what() << ")" << std::endl;
            continue;
        }
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, loader.getWidth(), loader.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, loader.getImageData());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        textures[texName] = texID;
    }

    glUniform1i(shaderLocations.getLocation("textureSampler"), 0);

    if (textures.find("white") == textures.end())
    {
        GLuint whiteTex;
        glGenTextures(1, &whiteTex);
        glBindTexture(GL_TEXTURE_2D, whiteTex);
        unsigned char whitePixel[3] = {255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        textures["white"] = whiteTex;
    }

    renderer = new sgraph::GLScenegraphRenderer(modelview, objects, shaderLocations, textures);

    trackball = new Trackball((float)window_width, (float)window_height);
}

void View::display(sgraph::IScenegraph *scenegraph)
{

    program.enable();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT_FACE);

    modelview.push(glm::mat4(1.0));

    if (cameraMode == 1)
    {
        // original
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(200.0f, 250.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelview.top() = modelview.top() * trackball->getRotationMatrix();
    }
    else if (cameraMode == 2)
    {
        // topdown
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0.0f, 400.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        modelview.top() = modelview.top() * trackball->getRotationMatrix();
    }
    else if (cameraMode == 3)
    {
        glm::vec3 firstPersonCamPos = dronePosition + glm::vec3(0.0f, -10.0f, 0.0f);

        glm::vec3 forward = glm::vec3(cos(droneRotation.y), 0.0f, -sin(droneRotation.y));

        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), right);
        glm::vec4 rotated = rotMat * glm::vec4(forward, 0.0f);
        glm::vec3 tiltedForward = glm::normalize(glm::vec3(rotated));

        glm::vec3 firstPersonCamTarget = firstPersonCamPos + tiltedForward;
        modelview.top() = modelview.top() * glm::lookAt(firstPersonCamPos, firstPersonCamTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else
    {
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(200.0f, 250.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelview.top() = modelview.top() * trackball->getRotationMatrix();
    }
    // send projection matrix to GPU
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelview.top())));
    glUniformMatrix3fv(shaderLocations.getLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // lights
    std::vector<util::Light> lights;
    collectLights(scenegraph->getRoot(), modelview.top(), lights);

    glUniform1i(shaderLocations.getLocation("numLights"), lights.size());
    for (size_t i = 0; i < lights.size(); i++)
    {
        std::string baseName = "lights[" + std::to_string(i) + "]";
        glUniform3fv(shaderLocations.getLocation(baseName + ".ambient"), 1, glm::value_ptr(lights[i].getAmbient()));
        glUniform3fv(shaderLocations.getLocation(baseName + ".diffuse"), 1, glm::value_ptr(lights[i].getDiffuse()));
        glUniform3fv(shaderLocations.getLocation(baseName + ".specular"), 1, glm::value_ptr(lights[i].getSpecular()));
        glUniform4fv(shaderLocations.getLocation(baseName + ".position"), 1, glm::value_ptr(lights[i].getPosition()));
        glUniform1f(shaderLocations.getLocation(baseName + ".spotCutoff"), lights[i].getSpotCutoff());
        glUniform4fv(shaderLocations.getLocation(baseName + ".spotDirection"), 1, glm::value_ptr(lights[i].getSpotDirection()));
    }

    // draw scene graph here
    scenegraph->getRoot()->accept(renderer);

    modelview.pop();
    glFlush();
    program.disable();

    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime - time) > 1.0)
    {
        printf("Framerate: %2.0f\r", frames / (currenttime - time));
        frames = 0;
        time = currenttime;
    }
}

bool View::shouldWindowClose()
{
    return glfwWindowShouldClose(window);
}

void View::closeWindow()
{
    for (map<string, util::ObjectInstance *>::iterator it = objects.begin();
         it != objects.end();
         it++)
    {
        it->second->cleanup();
        delete it->second;
    }
    glfwDestroyWindow(window);
    AppData *appData = reinterpret_cast<AppData *>(glfwGetWindowUserPointer(window));
    if (appData)
        delete appData;
    glfwTerminate();
}

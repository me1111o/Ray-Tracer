#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"
#include "Trackball.h"

#include <stack>
using namespace std;

class View
{
public:
    View();
    ~View();
    void init(Callbacks *callbacks, std::map<std::string, util::PolygonMesh<VertexAttrib>> &meshes, const std::map<std::string, std::string> &imagesMap);
    void display(sgraph::IScenegraph *scenegraph);
    bool shouldWindowClose();
    void closeWindow();

    void handleMouseButton(int button, int action, int mods);
    void handleCursorPos(double xpos, double ypos);

    void setDroneState(const glm::vec3 &position, const glm::vec3 &rotation)
    {
        dronePosition = position;
        droneRotation = rotation;
    }

    void setCameraMode(int mode) { cameraMode = mode; }
    Trackball *getTrackball() { return trackball; }

private:
    GLFWwindow *window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string, util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    int frames;
    double time;

    Trackball *trackball;

    glm::vec3 dronePosition;
    glm::vec3 droneRotation;
    int cameraMode = 1;

    GLuint textureID = 0;
    std::map<std::string, GLuint> textures;
};

struct AppData
{
    View *view;
    Callbacks *callbacks;
};

#endif
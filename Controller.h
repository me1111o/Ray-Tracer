#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "View.h"
#include "Model.h"
#include "Callbacks.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Controller : public Callbacks
{
public:
    Controller(Model &m, View &v);
    ~Controller();
    void run();

    virtual void reshape(int width, int height);
    virtual void dispose();
    virtual void onkey(int key, int scancode, int action, int mods);
    virtual void error_callback(int error, const char *description);

private:
    float propellerAngle = 0.0;
    float propellerSpeed = 1.0;
    sgraph::RotateTransform *propellerNodeLeft = nullptr;
    sgraph::RotateTransform *propellerNodeRight = nullptr;

    bool isRolling = false;
    float rollAngle = 0.0f;
    float rollSpeed = 0.0f;
    sgraph::RotateTransform *rollNodeX = nullptr;
    sgraph::RotateTransform *rollNodeY = nullptr;
    sgraph::RotateTransform *rollNodeZ = nullptr;

    glm::vec3 dronePosition;
    glm::vec3 droneRotation;
    float movementScale;
    sgraph::TranslateTransform *droneTranslate = nullptr;

    int cameraMode = 1;

    void initScenegraph();
    void startRollAnimation();

    View view;
    Model model;

    bool rayTracingMode = false;
    void renderRayTracing();
};

#endif
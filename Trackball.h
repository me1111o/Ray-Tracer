#ifndef TRACKBALL_H
#define TRACKBALL_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Trackball
{
public:
    Trackball(float width, float height);

    void mouseDown(float x, float y);
    void mouseUp();
    void mouseMove(float x, float y);

    glm::mat4 getRotationMatrix() const;

    void setSensitivity(float s);

private:
    float screenWidth, screenHeight;
    bool dragging;
    float sensitivity;
    glm::vec2 lastMousePos;
    glm::quat currentRotation;
};

#endif

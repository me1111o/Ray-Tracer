#include "Trackball.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

Trackball::Trackball(float width, float height) : screenWidth(width), screenHeight(height), dragging(false), sensitivity(0.01f),
                                                  currentRotation(glm::quat(1, 0, 0, 0))
{
}

void Trackball::mouseDown(float x, float y)
{
    dragging = true;
    lastMousePos = glm::vec2(x, y);
}

void Trackball::mouseUp()
{
    dragging = false;
}

void Trackball::mouseMove(float x, float y)
{
    if (!dragging)
        return;

    glm::vec2 currentMousePos(x, y);
    glm::vec2 delta = currentMousePos - lastMousePos;
    float angle = glm::length(delta) * sensitivity;
    if (angle > 0.0f)
    {
        glm::vec3 axis(delta.y, delta.x, 0.0f);
        axis = glm::normalize(axis);
        glm::quat rot = glm::angleAxis(angle, axis);
        currentRotation = rot * currentRotation;

        // std::cout << "Rotating: angle=" << angle << " axis=(" << axis.x << "," << axis.y << "," << axis.z << ")\n";
    }
    lastMousePos = currentMousePos;
}

glm::mat4 Trackball::getRotationMatrix() const
{
    return glm::toMat4(currentRotation);
}

void Trackball::setSensitivity(float s)
{
    sensitivity = s;
}

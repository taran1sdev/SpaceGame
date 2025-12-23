#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"
#include "Shader.hpp"
#include "Spaceship.hpp"

Camera::Camera()
{
    position = glm::vec3(0.0f, 0.0f, 200.0f);

    fov = 60.0f;
    aspect = 16.0f / 9.0f;
    nearPlane = 0.1f;
    farPlane = 2000.0f;
}

glm::vec3 Camera::getForward() const {
    float cosPitch = cos(glm::radians(pitch));
    float sinPitch = sin(glm::radians(pitch));
    float cosYaw = cos(glm::radians(yaw));
    float sinYaw = sin(glm::radians(yaw));

    return glm::normalize(glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch)); 
}

glm::vec3 Camera::getRight() const {
    return glm::normalize(glm::cross(getForward(), glm::vec3(0.0, 1.0, 0.0)));
}

glm::vec3 Camera::getUp() const {
     return glm::normalize(glm::cross(getRight(), getForward()));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(
            position,
            target,
            up        
    );
}

void Camera::Follow(const Spaceship& ship)
{
    const glm::vec3 shipPos = ship.GetPosition();
    const glm::quat shipRot = ship.GetOrientation();

    // Rotate local offset into world space
    const glm::vec3 worldOffset = shipRot * localOffset;

    position = shipPos + worldOffset;
    target   = shipPos;

    up = shipRot * glm::vec3(0, 1, 0);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(
            glm::radians(fov),
            aspect,
            nearPlane,
            farPlane
    );
}

void Camera::uploadToShader(Shader& shader) const {
    shader.setVec3("camPos", position);
    shader.setVec3("camForward", getForward());
    shader.setVec3("camRight", getRight());
    shader.setVec3("camUp", getUp());

    glm::mat4 view = getViewMatrix();
    glm::mat4 proj = getProjectionMatrix();
    glm::mat4 invViewProj = glm::inverse(proj * view);

    shader.setMat4("invViewProj", invViewProj);
}

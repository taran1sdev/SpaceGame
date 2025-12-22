#include <glm/gtc/matrix_transform.hpp>

#include "Spaceship.hpp"

Spaceship::Spaceship(const std::string& meshPath) 
    : body(),
      mesh(meshPath),
      orientation(glm::identity<glm::quat>())
{
    body.mass = 1.0f;
    body.position = glm::vec3(0.0f, 0.0f, 40.0f);
    body.velocity = glm::vec3(0.0f);
}

// Keyboard Controls 

void Spaceship::ApplyThrust(float force) {
    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    body.addForce(forward * force);
}

void Spaceship::ApplyBrake(float damping) {
    body.velocity *= damping;
}

// Mouse Controls

void Spaceship::Rotate(float deltaYaw, float deltaPitch) {
    glm::quat yaw = glm::angleAxis(deltaYaw, glm::vec3(0, 1, 0));
    glm::quat pitch = glm::angleAxis(deltaPitch, glm::vec3(1, 0, 0));

    orientation = glm::normalize(yaw * pitch * orientation);
}

/// Physics

void Spaceship::Update(float deltaTime) {
    body.integrate(deltaTime);
}

// Camera Info

glm::vec3 Spaceship::GetPosition() const {
    return body.position;
}

glm::quat Spaceship::GetOrientation() const {
    return orientation;
}

// Rendering

glm::mat4 Spaceship::GetModelMatrix() const {
    glm::mat4 model(1.0f);

    model = glm::translate(model, body.position);
    model *= glm::toMat4(orientation);

    return model;
}

void Spaceship::Draw(Shader& shader) const {
    shader.setMat4("model", GetModelMatrix());

    mesh.draw(shader);
}


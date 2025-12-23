#include <glm/gtc/matrix_transform.hpp>

#include "Spaceship.hpp"

Spaceship::Spaceship(const std::string& meshPath) 
    : body(),
      mesh(meshPath),
      orientation(glm::identity<glm::quat>())
{
    body.mass = 1.0f;
    body.position = glm::vec3(0.0f, 0.0f, 200.0f);
    body.velocity = glm::vec3(0.0f);
}

// Controls 

void Spaceship::ApplyThrust(float force) {
    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    body.addForce(forward * force);
}

void Spaceship::ApplyBrake(float damping) {
    body.velocity *= damping;
}

void Spaceship::AddYawInput(float amount) {
    angularVelocity.y += amount;
}

void Spaceship::AddPitchInput(float amount) {
    angularVelocity.x += amount;
}

void Spaceship::AddRollInput(float amount) {
    angularVelocity.z += amount;
}

/// Physics

void Spaceship::Update(float deltaTime) {
    body.integrate(deltaTime);

    const float angularDamping = 4.0f;
    angularVelocity *= exp(-angularDamping * deltaTime);
    
    float speed = glm::length(body.velocity);
    //float authority = ControlAuthority(body);
    float authority = glm::clamp(speed / 5.0f, 0.1f, 1.0f);

    glm::vec3 forward = glm::normalize(orientation * glm::vec3(0, 0, -1));
    glm::vec3 right = glm::normalize(orientation * glm::vec3(1, 0, 0));
    glm::vec3 up = glm::normalize(orientation * glm::vec3(0, 1, 0));

    glm::vec3 delta = angularVelocity * authority * deltaTime;

    if (glm::length2(delta) > 0.0f) {
        glm ::quat dq = 
            glm::angleAxis(delta.y, up) *
            glm::angleAxis(delta.x, right ) *
            glm::angleAxis(delta.z, forward);

        orientation = glm::normalize(dq * orientation);
    }
    
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


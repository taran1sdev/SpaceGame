#include "RigidBody.hpp"
#include <algorithm>

void RigidBody::setMass(float m) {
    mass = std::max(m, 0.0001f);
    invMass = 1.0f / mass;
}

void RigidBody::addForce(const glm::vec3& force) {
    accumForce += force;
}

void RigidBody::clearForces() {
    accumForce = glm::vec3(0.0f);
} 

void RigidBody::integrate(float dt) {
    if (/*  !enabled || */ invMass <= 0.0f ) return;

    glm::vec3 acceleration = accumForce * invMass;

    velocity += acceleration * dt;

    if (linearDamping > 0.0f) {
        float damp = std::max(0.0f, 1.0f - linearDamping * dt);
        velocity *= damp;
    }

    position += velocity * dt;

    clearForces();
}

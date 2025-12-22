#include <glm/glm.hpp>

#include "BlackHoleGravity.hpp"
#include "RigidBody.hpp"

void BlackHoleGravity::apply(RigidBody& body, float dt) const {
    if (!body.enabled) return;

    glm::vec3 rVec = body.position - position;
    float r = glm::length(rVec);
    
    if (r < 1e-4f) return;

    glm::vec3 rHat = rVec / r;

    float denom = std::pow(r * r + softening * softening, 1.5f);
    glm::vec3 gravityAccel = -mass * rVec / denom;

    float accelMag = glm::length(gravityAccel);
    if (accelMag > maxAccel) {
        gravityAccel *= (maxAccel / accelMag);
    } 

    if (!body.captured && r < captureRadius)  body.captured = true;

    if (body.captured) {
        float t = glm::clamp(
                (captureRadius - r) / (captureRadius - rs),
                0.0f, 
                1.0f
        );
        
        glm::vec3 sinkAccel = -rHat * sinkStrength * (t * t);
        gravityAccel += sinkAccel;

        glm::vec3 v = body.velocity;
        glm::vec3 vRadial = glm::dot(v, rHat) * rHat;
        glm::vec3 vTan = v - vRadial;

        body.velocity -= vTan * (tangentialDamp * t * dt);
    }

    body.addForce(gravityAccel * body.mass);
}

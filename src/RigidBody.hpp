#pragma once

#include <glm/glm.hpp>

class RigidBody {
    public: 
        // current state
        glm::vec3 position {0.0f};
        glm::vec3 velocity {0.0f};

        // physical properties
        float mass = 1.0f;
        float invMass = 1.0f;
        float linearDamping = 0.0f;
        
        // captured in BH
        bool captured = false;
        // affected by BH
        bool enabled = true;

        void addForce(const glm::vec3& force);

        void integrate(float dt);

        void setMass(float m);
        void clearForces();

    private:
        glm::vec3 accumForce {0.0f};
};

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Mesh.hpp"
#include "RigidBody.hpp"
#include "Shader.hpp"

#include <string>

class Spaceship {
    public:
        explicit Spaceship(const std::string& meshPath);
        
        void ApplyThrust(float force);
        void ApplyBrake(float damping);
        void Rotate(float deltaYaw, float deltaPitch);

        void Update(float deltaTime);

        void Draw(Shader& shader) const;

        glm::vec3 GetPosition() const;
        glm::quat GetOrientation() const;


        RigidBody body;

    private:
        Mesh mesh;
        
        glm::quat orientation;
        
        glm::mat4 GetModelMatrix() const;       
};

#pragma once
#include <glm/glm.hpp>

class Spaceship;

class Camera {
    public:
        Camera();
        
        float yaw;
        float pitch;

        // Position
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;
    
        void Follow(const Spaceship& ship, float deltaTime);
        // Projection
        float fov;
        float aspect;
        float nearPlane;
        float farPlane;
        
        // Position helpers
        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;
        
        // Projection helpers 
        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix() const;

        // Communicate with GPU 
        void uploadToShader(class Shader& shader) const;
        void updateCameraVectors();

    private:
        float followDistance = 10.0f;
        float followHeight = 8.0f;
        float smoothing = 8.0f;
};

#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;

class Camera {
    public:
        Camera();
        
        // Position
        glm::vec3 position;
        float yaw;
        float pitch;
        
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

        // Get user input
        void getMouse(float x, float y);
        void getKeys(GLFWwindow* window, float deltaTime);
        
        // Communicate with GPU 
        void uploadToShader(class Shader& shader) const;
        void updateCameraVectors();
};

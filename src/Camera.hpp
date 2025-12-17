#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;

class Camera {
    public:
        glm::vec3 position;
        float yaw;
        float pitch;

        Camera();

        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;

        void getMouse(float x, float y);
        void getKeys(GLFWwindow* window, float deltaTime);
        
        void uploadToShader(class Shader& shader) const;
        void updateCameraVectors();
};

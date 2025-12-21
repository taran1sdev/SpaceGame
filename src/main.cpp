#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "BlackHole.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"

Camera camera;
float lastX = 400, lastY = 300, deltaTime = 0.0f, lastFrame = 0.0f;
bool firstMouse = true;

// We register this callback function with GLFW to process the mouse position
void mouseCallback(GLFWwindow* window, double x, double y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xOffset = x - lastX;
    float yOffset = lastY - y;

    lastX = x;
    lastY = y;

    camera.getMouse(xOffset, yOffset);
}

void framebufferCallback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);

    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (cam && h > 0) {
        cam->aspect = static_cast<float>(w) / static_cast<float>(h);
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Black Hole", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // Register callback for mouse 
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSetWindowUserPointer(window, &camera);

    glfwSetFramebufferSizeCallback(window, framebufferCallback);  
    // Use glad to load OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr <<"Failed to initialize GLAD\n";
        return -1;
    }
    
    BlackHole bH(glm::vec3(0.0f));
    Renderer renderer(camera, bH);
    renderer.init();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check every frame for pressed key - using a callback doesn't allow keys
        // to be held and results in choppy movements
        camera.getKeys(window, deltaTime);

        renderer.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

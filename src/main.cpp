#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "BlackHole.hpp"
#include "BlackHoleGravity.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Spaceship.hpp"

static float mouseDX = 0.0f;
static float mouseDY = 0.0f;
static bool firstMouse = true;

static double lastMouseX = 0.0;
static double lastMouseY = 0.0;

static bool keyW = false;
static bool keyS = false;

static float mouseSensitivity = 0.0025f;

void frameBufferCallback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    mouseDX = static_cast<float>(xpos - lastMouseX);
    // Invert the Y-axis - lego star wars style
    mouseDY = static_cast<float>(lastMouseY - ypos);

    lastMouseX = xpos;
    lastMouseY = ypos;
}

void PollInput(GLFWwindow* window) {
    keyW = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    keyS = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);            
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);

    //glEnable(GL_DEPTH_TEST);
    

    glfwSetFramebufferSizeCallback(window, frameBufferCallback);  
    // Use glad to load OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr <<"Failed to initialize GLAD\n";
        return -1;
    }
    
    Camera camera; 

    glfwSetWindowUserPointer(window, &camera);
    
    BlackHole bH(glm::vec3(0.0f));
    Spaceship ship("../assets/spaceship.obj");
    Renderer renderer(camera, bH);
    renderer.init();
    
    BlackHoleGravity bHG;
    
    float lastFrameTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        glfwPollEvents();
        PollInput(window);
        
        ship.Rotate(
                mouseDX * mouseSensitivity, 
                mouseDY * mouseSensitivity
        );

        mouseDX = 0.0f;
        mouseDY = 0.0f;

        // Check every frame for pressed key - using a callback doesn't allow keys
        // to be held and results in choppy movements
        
        if (keyW)
            ship.ApplyThrust(30.0f * deltaTime);

        if (keyS)
            ship.ApplyBrake(0.95f);

        bHG.apply(ship.body, deltaTime);
        ship.Update(deltaTime);

        camera.Follow(ship, deltaTime);

        renderer.render(ship);

        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}

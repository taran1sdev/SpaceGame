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
static bool keyA = false;
static bool keyD = false;

static float mouseSensitivity = 0.025f;

void frameBufferCallback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    mouseDX = static_cast<float>(lastMouseX - xpos);
    // Invert the Y-axis - lego star wars style
    mouseDY = static_cast<float>(ypos - lastMouseY);

    lastMouseX = xpos;
    lastMouseY = ypos;
}

void PollInput(GLFWwindow* window) {
    keyW = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    keyS = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    keyA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    keyD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

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
    Spaceship ship("../assets/ship.fbx");
    Renderer renderer(camera, bH, ship);
    renderer.init();
    
    BlackHoleGravity bHG;
    
    float lastFrameTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        glfwPollEvents();
        PollInput(window);
        
        ship.AddYawInput(mouseDX * mouseSensitivity);
        ship.AddPitchInput(mouseDY * mouseSensitivity);

        mouseDX = 0.0f;
        mouseDY = 0.0f;

        // Check every frame for pressed key - using a callback doesn't allow keys
        // to be held and results in choppy movements
        
        if (keyW)
            ship.ApplyThrust(3.0f);

        if (keyS)
            ship.ApplyBrake(0.95f);
        
        if (keyA)
            ship.AddRollInput(-2.0f * deltaTime);

        if (keyD)
            ship.AddRollInput(+2.0f * deltaTime);

        bHG.apply(ship.body, deltaTime);
        ship.Update(deltaTime);

        camera.Follow(ship);

        renderer.render();

        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}

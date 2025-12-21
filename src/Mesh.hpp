#pragma once 

#include <glad/glad.h>

// This class is for our cubemap texture 
// its my first time implementing anything like this so it may change over time
// this abstraction also allows us to easily change the background if we want

class Mesh {
    public:
        Mesh();
        ~Mesh();
        
        // This protects against copying / multiple instances - the GPU owns the resource
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
    
        // Function to creating our skybox cube
        void createCube();

        // Function for drawing the fullscreen quad (post-effects and collisions)
        void createFullscreenQuad();
        
        // exactly what you think
        void draw() const;
    private:
        // The class owns a Vertex Array Object / Vertex Buffer Object
        GLuint vao = 0;
        GLuint vbo = 0;
        // This is the number of vertices to draw
        GLsizei vertexCount = 0;  
};

#pragma once 
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <vector>

// This class is for our cubemap texture 
// its my first time implementing anything like this so it may change over time
// this abstraction also allows us to easily change the background if we want

class Shader;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
    public:
        Mesh() = default;
        explicit Mesh(const std::string& meshPath);
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
        void draw(Shader& shader) const;
    private:
        // The class owns a Vertex Array Object / Vertex Buffer Object
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        // This is the number of vertices to draw
        GLsizei vertexCount = 0; 

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        void loadFBX(const std::string& path);
        void setupMesh();
};

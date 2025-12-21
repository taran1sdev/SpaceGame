#include "Mesh.hpp"

// We don't need anything here for now - but nice to have in case we do later
Mesh::Mesh() {}

// Destructor ensures we manage memory correctly
Mesh::~Mesh() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

// This creates the skybox cube with 36 vertices... Copy pasted from tutorial
void Mesh::createCube() {
        float vertices[] = {
        -1,  1, -1,  -1, -1, -1,   1, -1, -1,
         1, -1, -1,   1,  1, -1,  -1,  1, -1,
        -1, -1,  1,  -1, -1, -1,  -1,  1, -1,
        -1,  1, -1,  -1,  1,  1,  -1, -1,  1,
         1, -1, -1,   1, -1,  1,   1,  1,  1,
         1,  1,  1,   1,  1, -1,   1, -1, -1,
        -1, -1,  1,  -1,  1,  1,   1,  1,  1,
         1,  1,  1,   1, -1,  1,  -1, -1,  1,
        -1,  1, -1,   1,  1, -1,   1,  1,  1,
         1,  1,  1,  -1,  1,  1,  -1,  1, -1,
        -1, -1, -1,  -1, -1,  1,   1, -1, -1,
         1, -1, -1,  -1, -1,  1,   1, -1,  1
        };

        vertexCount = 36;
        
        // We create the vao / vbo and store their ID's in our fields
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);
        
        // Create a new data store for our vbo
        glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        // Bind the vbo to our vertex array
        glVertexArrayVertexBuffer(
                vao, 0, // bind to vao at index 0
                vbo, 0, // bind vbo from offset 0
                3 * sizeof(float)  // distance between elements in the buffer
        );
        
        // Enable the vao 
        glEnableVertexArrayAttrib(vao, 0);
        
        // This specifies the format of the vao - number of values, type etc.
        glVertexArrayAttribFormat(
                vao, 0, // vao and attribute
                3, GL_FLOAT, // number of values per vertex and data type
                GL_FALSE, 0 // do not normalize (keep as fixed point values) and offset is 0
        ); 

        // associate vertex attribute and vertex buffer binding
        glVertexArrayAttribBinding(vao, 1, 0);
}

// This function draws our vertices stored in VAO
void Mesh::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

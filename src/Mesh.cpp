#include <glad/glad.h>

#include "Mesh.hpp"
#include "Shader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>

Mesh::Mesh(const std::string& meshPath) {
   loadFBX(meshPath);
   setupMesh(); 
}


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

void Mesh::draw(Shader& shader) const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); 
}

void Mesh::loadFBX(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices
    );

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Failed to load FBX: " << path << std::endl;
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return;
    }
    
    aiMesh* mesh = scene->mMeshes[0];

    vertices.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.Position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        vertex.Normal = mesh->HasNormals() 
            ? glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z)
            : glm::vec3(0.0f);

        vertex.TexCoords = mesh-> HasTextureCoords(0)
            ? glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y)
            : glm::vec2(0.0f);

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW
    );
    

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, Normal)
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
         2, 2, GL_FLOAT, GL_FALSE,
         sizeof(Vertex),
         (void*)offsetof(Vertex, TexCoords)
    );

    glBindVertexArray(0);
}

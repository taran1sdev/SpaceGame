#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Load the shader files
    std::string vertexSrc = loadFile(vertexPath);
    std::string fragmentSrc = loadFile(fragmentPath);
    
    // Attempt to compile the loaded files
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    
    // Create the program and link the shaders 
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Failed to link shader:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const std::string& computePath)
{
    std::string computeSrc = loadFile(computePath);
    unsigned int compute = compileShader(GL_COMPUTE_SHADER, computeSrc);
    
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Compute shader linking failed: \n" << infoLog << std::endl;
    }

    glDeleteShader(compute);
}

// Implement our public functions 
void Shader::use() const {
   glUseProgram(ID); 
}

void Shader::destroy() {
   glDeleteProgram(ID);   
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));    
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform2f(loc, x, y);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform2f(loc, value.x, value.y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::setFloat(const std::string& name, float val) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, val);
}

void Shader::setInt(const std::string&name, int val) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, val);
}

// Helper function to load file from path
std::string Shader::loadFile(const std::string& path) const {
    std::ifstream file(path);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

// Helper function to compile contents loaded from file
unsigned int Shader::compileShader(unsigned int type, const std::string& code) const {
    unsigned int shader = glCreateShader(type);
    const char* src = code.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }
    
    return shader;
}






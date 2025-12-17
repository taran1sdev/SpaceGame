#include "BlackHole.hpp"
#include "Shader.hpp"

BlackHole::BlackHole(const glm::vec3& position, float spin)
    : position(position), spin(glm::clamp(spin, -1.0f, 1.0f))
{}

glm::vec3 BlackHole::getPosition() const {
    return position;
}

float BlackHole::getSchildRadius() const { 
    return 2.0f; 
}

float BlackHole::getGravRadius() const { 
    return 1.0f; 
}

void BlackHole::uploadToShader(Shader& shader) const {
    shader.setVec3("bh_position", position);
    shader.setFloat("bh_rs", 2.0f);
    shader.setFloat("bh_rg", 1.0f);
    shader.setFloat("bh_spin", spin);
}

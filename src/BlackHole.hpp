#pragma once
#include <glm/glm.hpp>

class Camera;

class BlackHole {
    public:
        BlackHole(const glm::vec3& pos);
        
        // world pos / radius
        glm::vec3 position;
        float rs;
        
        // screen pos / radius
        glm::vec2 screenUV;
        float horizonRadius;
        
        // might not need this function anymore
        glm::vec3 getPosition() const;
        
        // updates screen pos 
        void updateScreenSpace(const Camera& cam, int w, int h);
        
        // Communicate with GPU
        void uploadToShader(class Shader& computeShader) const;
};

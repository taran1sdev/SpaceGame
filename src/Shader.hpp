#pragma once
#include <string>
#include <glm/glm.hpp>
/*
 * This class handles loading and compiling shader files
 */

class Shader {
    public:
        // constructor for vertex / fragment shaders
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
               
        // constructor for compute shader
        explicit Shader(const std::string& computePath);

        // functions to set uniform 
        void setMat4(const std::string& name, const glm::mat4& mat) const;

        void setVec2(const std::string& name, float x, float y) const;
        void setVec2(const std::string& name, const glm::vec2& value) const;

        void setVec3(const std::string& name, const glm::vec3& value) const;
        
        void setFloat(const std::string& name, float val) const;
        
        void setInt(const std::string& name, int val) const;

        void use() const;

        void destroy();

    private:
        unsigned int ID;        
        std::string loadFile(const std::string& path) const;
        unsigned int compileShader(unsigned int type, const std::string& source) const;
};

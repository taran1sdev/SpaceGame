#pragma once
#include "Shader.hpp"
#include "Camera.hpp"
#include "BlackHole.hpp"
#include "FullscreenQuad.hpp"

class Renderer {
    public:
        Renderer(Shader& screenShader, Camera& cam, BlackHole& blackHole);
        void init();
        void render();
    
    private:
        Shader& screenShader;
        Camera& camera;
        BlackHole& blackHole;

        Shader computeShader;
        Shader diskVolumeShader;
        FullscreenQuad quad;
        
        GLuint starCubemap = 0;


        GLuint outputTexture = 0;
        int texWidth = 0;
        int texHeight = 0;
        
        GLuint diskVolumeTexture = 0;

        int diskNR = 512;
        int diskNTheta = 1024;
        int diskNZ = 128;

        float diskRMin = 2.5f;
        float diskRMax = 12.0f;
        float diskZMin = -0.4f;
        float diskZMax = 0.4f;
        
        void initStarCubemap();
        void initTexture(int w, int h);
        void initDiskVolumeTexture();
};

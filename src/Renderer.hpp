#pragma once
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "BlackHole.hpp"
#include "FullscreenQuad.hpp"
#include "Mesh.hpp"
#include "Spaceship.hpp"

class Renderer {
    public:
        // This is the constructor we will use
        Renderer(Camera& cam, BlackHole& bh, Spaceship& ship);
        ~Renderer();
        void init();
        void render();
    private:
        Camera& camera;
        BlackHole& blackHole;
        Spaceship& ship;

        Shader diskImposterShader;
        Shader diskVolumeShader;
        Shader bhLensPostShader;
        
        Shader fullscreenShader;
        FullscreenQuad fullscreenQuad;

        Shader skyboxShader;
        Mesh skyboxMesh;       
        
        Shader shipShader;        
        GLuint shipTexture = 0;

        GLuint sceneFBO = 0;
        GLuint starCubemap = 0;

        GLuint diskVolumeTexture = 0;
        
        GLuint sceneColourTex = 0;

        GLuint diskOverlayBase = 0;
        GLuint diskOverlayTop = 0;
        GLuint diskOverlayBottom = 0;
        GLuint bhLensedTex = 0;

        // Accretion Disk Dimensions (We should refactor this away later)
        int diskNR = 128;
        int diskNTheta = 256;
        int diskNZ = 64;

        float diskRMin = 5.5f;
        float diskRMax = 20.0f;
        float diskZMax = 1.2f;
       
         
        void initStarCubemap();
        void initTexture(int w, int h);
        void initDiskVolumeTexture();
        void initDiskOverlayTextures(int w, int h);

        void renderScene();
};

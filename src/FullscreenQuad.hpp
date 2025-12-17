#pragma once

/* 
 * This class creates a Vertex Array Object and a Vertex Buffer Object
 * containing 6 vertices, belonging to 2 triangles that cover the entire
 * screen. This allows fragment shaders to process every pixel on screen
*/ 
class FullscreenQuad {
    public:
        FullscreenQuad();
        ~FullscreenQuad();

        void draw() const;

    private:
        unsigned int VAO, VBO;
};

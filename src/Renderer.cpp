#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ddsLoader.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "BlackHole.hpp"
#include "FullscreenQuad.hpp"
#include "Renderer.hpp"

#include <iostream>

Renderer::Renderer(Shader& screenShader, Camera& cam, BlackHole& bh)
    : screenShader(screenShader), camera(cam), blackHole(bh), computeShader("../shaders/blackhole.comp"), diskVolumeShader("../shaders/disk_init.comp")
{}

void Renderer::init() {
    initDiskVolumeTexture();

    diskVolumeShader.use();

    glBindImageTexture(
            0, diskVolumeTexture,
            0, GL_TRUE, 0,
            GL_WRITE_ONLY,
            GL_RGBA16F
    );

    diskVolumeShader.setFloat("rMin", diskRMin);
    diskVolumeShader.setFloat("rMax", diskRMax);
    diskVolumeShader.setFloat("zMin", diskZMin);
    diskVolumeShader.setFloat("zMax", diskZMax);
    
    float t = static_cast<float>(glfwGetTime());
    
    diskVolumeShader.setFloat("uTime", t);

    GLuint gx = (diskNTheta + 3) / 4;
    GLuint gy = (diskNR + 3) / 4;
    GLuint gz = (diskNZ + 3) / 4;

    glDispatchCompute(gx, gy, gz);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    initStarCubemap();
}

void Renderer::initStarCubemap() {
    starCubemap = loadDDSCubemap("../assets/stars.dds");
    
    if (!starCubemap) {
        std::cerr << "Failed to load star cubemap\n";
    }

    computeShader.use();
    computeShader.setInt("starCube", 4);
    glUseProgram(0);
}

void Renderer::initTexture(int w, int h) {
    if (outputTexture != 0 && (w == texWidth && h == texHeight)) return;
    
    if (outputTexture != 0) glDeleteTextures(1, &outputTexture);

    texWidth = w;
    texHeight = h;

    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_RGBA32F, w, h,
            0,
            GL_RGBA, GL_FLOAT,
            nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::initDiskVolumeTexture() {
    glGenTextures(1, &diskVolumeTexture);
    glBindTexture(GL_TEXTURE_3D, diskVolumeTexture);

    glTexStorage3D(
            GL_TEXTURE_3D,
            1,
            GL_RGBA16F,
            diskNTheta,
            diskNR,
            diskNZ
    );
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);    
}

void Renderer::render()
{
    int w, h;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);    
    initTexture(w, h);
    

    float t = static_cast<float>(glfwGetTime());
    
    computeShader.use();
    computeShader.setVec2("resolution", (float)w, (float)h);
    computeShader.setFloat("uTime", t);

    camera.uploadToShader(computeShader); 
    blackHole.uploadToShader(computeShader);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, diskVolumeTexture);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, starCubemap);

    computeShader.setInt("diskVolume", 1);

    computeShader.setFloat("disk_rMin", diskRMin);
    computeShader.setFloat("disk_rMax", diskRMax);
    computeShader.setFloat("disk_zMin", diskZMin);
    computeShader.setFloat("disk_zMax", diskZMax);

    computeShader.setInt("diskNR", diskNR);
    computeShader.setInt("diskNTheta", diskNTheta);
    computeShader.setInt("diskNZ", diskNZ);

    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    const GLuint localSizeX = 8;
    const GLuint localSizeY = 8;
    GLuint groupsX = (w + localSizeX - 1) / localSizeX;
    GLuint groupsY = (h + localSizeY - 1) / localSizeY;

    glDispatchCompute(groupsX, groupsY, 1);
   
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT); 
    
    screenShader.use();
    screenShader.setInt("renderedImage", 0);
    screenShader.setFloat("uTime", t);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);

    quad.draw();
}

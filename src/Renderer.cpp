#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ddsLoader.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "BlackHole.hpp"
#include "Renderer.hpp"

#include <iostream>

// Create the renderer with references to the camera object and BH object
Renderer::Renderer(Camera& cam, BlackHole& bh)
    : camera(cam),
      blackHole(bh),
      diskImposterShader("../shaders/bh_disk_imposter.comp"),
      diskVolumeShader("../shaders/disk_init.comp"),
      bhLensPostShader("../shaders/bh_lens_post.comp"),
      fullscreenShader("../shaders/fullscreen.vert", "../shaders/fullscreen.frag"),
      skyboxShader("../shaders/skybox.vert", "../shaders/skybox.frag")
{
}

Renderer::~Renderer()
{
    glDeleteFramebuffers(1, &sceneFBO);
    glDeleteTextures(1, &sceneColourTex);
    glDeleteTextures(1, &diskOverlayTop);
    glDeleteTextures(1, &diskOverlayBottom);
    glDeleteTextures(1, &bhLensedTex);
}

void Renderer::init()
{
    skyboxMesh.createCube();
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
    diskVolumeShader.setFloat("zMax", diskZMax);

    float t = static_cast<float>(glfwGetTime());

    diskVolumeShader.setFloat("uTime", t);

    GLuint gx = (diskNTheta + 7) / 8;
    GLuint gy = (diskNR + 7) / 8;
    GLuint gz = (diskNZ + 3) / 4;

    glDispatchCompute(gx, gy, gz);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    initStarCubemap();

    glCreateFramebuffers(1, &sceneFBO);
}

void Renderer::initStarCubemap() {
    starCubemap = loadDDSCubemap("../assets/stars.dds");

    if (!starCubemap) {
        std::cerr << "Failed to load star cubemap\n";
    }
}

void Renderer::initTexture(int w, int h)
{
    static int lastW = 0;
    static int lastH = 0;

    if (w == lastW && h == lastH)
        return;

    lastW = w;
    lastH = h;

    if (sceneColourTex)
        glDeleteTextures(1, &sceneColourTex);

    glCreateTextures(GL_TEXTURE_2D, 1, &sceneColourTex);
    glTextureStorage2D(sceneColourTex, 1, GL_RGBA16F, w, h);
    glTextureParameteri(sceneColourTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(sceneColourTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(sceneColourTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(sceneColourTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glNamedFramebufferTexture(
        sceneFBO,
        GL_COLOR_ATTACHMENT0,
        sceneColourTex,
        0
    );

    if (diskOverlayTop) glDeleteTextures(1, &diskOverlayTop);
    if (diskOverlayBottom) glDeleteTextures(1, &diskOverlayBottom);

    initDiskOverlayTextures(w, h);

    if (glCheckNamedFramebufferStatus(sceneFBO, GL_FRAMEBUFFER)
        != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Scene framebuffer incomplete!" << std::endl;
    }

    if (bhLensedTex) glDeleteTextures(1, &bhLensedTex);

    glCreateTextures(GL_TEXTURE_2D, 1, &bhLensedTex);
    glTextureStorage2D(bhLensedTex, 1, GL_RGBA16F, w, h);
    glTextureParameteri(bhLensedTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(bhLensedTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTextureParameteri(bhLensedTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(bhLensedTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

void Renderer::initDiskOverlayTextures(int w, int h) {
    auto makeTex = [&](GLuint& tex)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &tex);
        glTextureStorage2D(tex, 1, GL_RGBA16F, w, h);
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    };

    makeTex(diskOverlayTop);
    makeTex(diskOverlayBottom);
}

void Renderer::render()
{
    int w, h;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);

    initTexture(w, h);

    // Update BH screen-space data
    blackHole.updateScreenSpace(camera, w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderScene(); // stars, ships, skybox, etc.

    diskImposterShader.use();
    
    // shared uniforms
    camera.uploadToShader(diskImposterShader);
    blackHole.uploadToShader(diskImposterShader);

    diskImposterShader.setVec2("resolution", glm::vec2(w, h));
    diskImposterShader.setFloat("disk_rMin", diskRMin);
    diskImposterShader.setFloat("disk_rMax", diskRMax);
    diskImposterShader.setFloat("disk_zMax", diskZMax);

    glBindTextureUnit(1, diskVolumeTexture);
    
    diskImposterShader.setFloat("disk_alphaScale", 0.9f);
    diskImposterShader.setFloat("disk_emissiveScale", 0.2f);
    diskImposterShader.setFloat("disk_thicknessTaps", 8.0f);
    diskImposterShader.setFloat("disk_thicknessJitter", 0.65f);

    GLuint gx = (w + 7) / 8;
    GLuint gy = (h + 7) / 8;
    
    // top disk
    glBindImageTexture(
          0, diskOverlayTop,
          0, GL_FALSE,
          0, GL_WRITE_ONLY, GL_RGBA16F
    );
    
    diskImposterShader.setFloat("disk_planeY", 0.0f);
    diskImposterShader.setFloat("disk_yMin", 0.0f);
    diskImposterShader.setFloat("disk_yMax", diskZMax);
    diskImposterShader.setFloat("far_warp_strength", 0.0f);

    glDispatchCompute(gx, gy, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  
    
    glBindImageTexture(
            0, diskOverlayBottom,
            0, GL_FALSE,
            0, GL_WRITE_ONLY, GL_RGBA16F
    );

    diskImposterShader.setFloat("disk_planeY", 0.0f);
    diskImposterShader.setFloat("disk_yMin", -diskZMax);
    diskImposterShader.setFloat("disk_yMax", 0.0f);
    diskImposterShader.setFloat("far_warp_strength", 0.0f);

    glDispatchCompute(gx, gy, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);

    bhLensPostShader.use();
    
    glBindImageTexture(
            0, bhLensedTex,
            0, GL_FALSE,
            0, GL_WRITE_ONLY, GL_RGBA16F
    );

    glBindTextureUnit(1, sceneColourTex);
    glBindTextureUnit(3, diskOverlayTop);
    glBindTextureUnit(4, diskOverlayBottom);
    
    blackHole.uploadToShader(bhLensPostShader);

    bhLensPostShader.setVec2("resolution", (float)w, (float)h);

    bhLensPostShader.setFloat("lensInnerRadius", blackHole.horizonRadius * 1.05f);
    bhLensPostShader.setFloat("lensOuterRadius", blackHole.horizonRadius * 4.0f);
    bhLensPostShader.setFloat("lensStrength", 1.2f);

    bhLensPostShader.setFloat("ringRadius", blackHole.horizonRadius * 1.02f);
    bhLensPostShader.setFloat("ringWidth", blackHole.horizonRadius * 0.01f);
    bhLensPostShader.setFloat("ringIntensity", 2.5f);
    bhLensPostShader.setVec3("ringColour", glm::vec3(1.0f, 0.9f, 0.7f));
    
    glDispatchCompute(gx, gy, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    fullscreenShader.use();

    glBindTextureUnit(0, bhLensedTex);

    fullscreenQuad.draw();
}


void Renderer::renderScene()
{
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    skyboxShader.use();

    glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
    glm::mat4 proj = camera.getProjectionMatrix();

    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", proj);
    
    glBindTextureUnit(0, starCubemap);
    skyboxShader.setInt("starCubemap", 0);

    skyboxMesh.draw();

    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
}



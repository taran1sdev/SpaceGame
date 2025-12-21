#include "BlackHole.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"

BlackHole::BlackHole(const glm::vec3& pos)
    : position(pos)
{
    position = pos;
    rs = 2.0f;
}

glm::vec3 BlackHole::getPosition() const {
    return position;
}

void BlackHole::updateScreenSpace(const Camera& cam, int w, int h) {
    // get projection from camera
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 proj = cam.getProjectionMatrix();
    glm::mat4 vp = proj * view;

    // project the BH
    glm::vec4 clip = vp * glm::vec4(position, 1.0f);

    // safe division - push if screen if <= 0
    if (clip.w <= 0.0f) {
        screenUV = glm::vec2(-1.0f);
        horizonRadius = 0.0f;
        return;
    }

    glm::vec3 ndc = glm::vec3(clip) / clip.w;

    screenUV.x = ndc.x * 0.5f + 0.5f;
    screenUV.y = ndc.y * 0.5f + 0.5f;

    // screen space event horizon size
    float distance = glm::length(cam.position - position);

    horizonRadius = (rs / distance) * 1.2f;

    float aspect = static_cast<float>(w) / static_cast<float>(h);
    horizonRadius *= aspect;
}

void BlackHole::uploadToShader(Shader& shader) const {
    shader.setVec3("bh_position", position);
    shader.setFloat("bh_rs", 2.0f);
}

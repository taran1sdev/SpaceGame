#version 460 core

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColour;

uniform vec3 cameraPos;
uniform vec3 lightDir;

uniform sampler2D diffuseMap;

void main() {
    vec3 shipColour = texture(diffuseMap, TexCoord).rgb;

    vec3 N = normalize(Normal);
    vec3 L = normalize(-lightDir);

    float diff = max(dot(N, L), 0.0);

    vec3 colour = shipColour * (0.2 + 0.8 * diff);

    FragColour = vec4(colour, 1.0);
}

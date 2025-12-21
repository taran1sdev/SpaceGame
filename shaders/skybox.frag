#version 460 core 

in vec3 vDir;
out vec4 FragColour;

uniform samplerCube starCubemap;

void main() {
    FragColour = texture(starCubemap, normalize(vDir));
}

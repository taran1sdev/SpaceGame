#version 460 core

in vec2 vUV;
out vec4 FragColour;

layout(binding = 0) uniform sampler2D renderedImage;

const float ASPECT_RATIO = 16.0 / 9.0;

vec3 toneMapACES(vec3 colour) {
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    colour = clamp(colour, 0.0, 1e6);
    return clamp((colour * (A * colour + B)) / (colour * (C * colour + D) + E), 0.0, 1.0);
}

float luminance(vec3 c) {
    return dot(c, vec3(0.299, 0.587, 0.114));
}

void main() {
    vec3 colour = texture(renderedImage, vUV).rgb;

    vec3 blur = vec3(0.0);
    float weights[5] = float[5](0.204, 0.304, 0.193, 0.081, 0.018);
    for (int i = -2; i <= 2; ++i) {
        blur += texture(renderedImage, vUV + vec2(i) * 0.002).rgb * weights[i + 2];
    }

    float bright = luminance(colour);
    float boost = 0.18 * exp(-pow((bright - 0.25) / 0.18, 2.0));
    colour += colour * boost;
    vec3 bloom = blur * smoothstep(0.3, 1.0, bright) * 0.15;

    float flare = exp(-abs(vUV.y - 0.5) * 15.0) * 0.8;
    vec3 flareColour = vec3(0.3, 0.5, 1.0) * flare * dot(bloom, vec3(0.333));

    vec2 centered = vUV * 2.0 - 1.0;
    //float vignette = 1.0 - (centered.x * centered.x + centered.y * centered.y * ASPECT_RATIO * ASPECT_RATIO) * 0.4;
    //vignette = clamp(vignette, 0.0, 1.0);

    float grain = (fract(sin(dot(vUV * 0.5 + vec2(0.07, 0.13), vec2(12.9898, 78.233))) * 43758.5453) - 0.5) * 0.02;

    colour = colour + bloom + flareColour;
    //colour *= vignette;
    colour += grain;

    colour = toneMapACES(colour * 1.65);  
    colour = pow(colour, vec3(1.0 / 2.2));

    FragColour = vec4(colour, 1.0);
    
}

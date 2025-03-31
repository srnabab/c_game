#version 460

layout(location = 0) in vec2 uv; // Texture coordinates from full-screen quad

layout(binding = 0, set = 0) uniform sampler2D sceneColorSampler;
layout(binding = 1, set = 0) uniform sampler2D shadowSampler;

layout(location = 0) out vec4 finalColor;

void main() 
{
    vec4 sceneColor = texture(sceneColorSampler, uv);
    float shadow = texture(shadowSampler, uv).r;

    finalColor = vec4(sceneColor * shadow);
}
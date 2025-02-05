#version 450

layout(push_constant) uniform PushConstantBlock1 {
    mat4 modelMatrix;
    vec4 color;
} pushConstants;

layout(set = 0, binding = 1) uniform sampler2D texSampler[3];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragDepth;

layout(location = 0) out vec4 outColor;
void main(){
    outColor = vec4(fragColor, 1.0);
}
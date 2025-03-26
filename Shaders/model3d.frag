#version 460

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 outWorldPos;
layout(location = 3) in vec3 outWorldNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormalBuffer;

void main() 
{
    outNormalBuffer = vec4(outWorldNormal * 0.5 + 0.5, 1.0);

    outColor = texture(texSampler, fragTexCoord);
}
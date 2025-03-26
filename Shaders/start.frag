#version 460

layout(set = 0, binding = 1) uniform sampler2D texSampler[4];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragDepth;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(texSampler[0], fragTexCoord) + 
                texture(texSampler[1], fragTexCoord) +
                texture(texSampler[2], fragTexCoord) +
                texture(texSampler[3], fragTexCoord);
}
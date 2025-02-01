#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler0;
layout(set = 0, binding = 2) uniform sampler2D texSampler1;
layout(set = 0, binding = 3) uniform sampler2D texSampler2;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragDepth;

layout(location = 0) out vec4 outColor;

void main() 
{
    if (fragDepth == 0.0)
    {
        outColor = texture(texSampler0, fragTexCoord);
    }
    else if (fragDepth == 0.1)
    {
        outColor = vec4(vec3(1.0), texture(texSampler2, fragTexCoord).r);
    }
    else
    {
        outColor = texture(texSampler1, fragTexCoord);
    }
}
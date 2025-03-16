#version 450

layout(push_constant) uniform _PushConstans{
    float rotation;
    float height_to_fix_height_ratio;
} PushConstants;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float fragDepth;

void main() 
{
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragDepth = inPosition.z;

    if (inPosition.z == 0.2)
    {
        mat2 rotationMatrix = mat2(
            cos(PushConstants.rotation), -sin(PushConstants.rotation),
            sin(PushConstants.rotation), cos(PushConstants.rotation)
        );
        vec2 rotatedPosition = rotationMatrix * inPosition.xy;

        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(rotatedPosition, inPosition.z, 1.0);
    }
    else if (inPosition.z == 0.1)
    {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    }
    else
    {
        mat4 temp = ubo.proj;
        temp[0].r = ubo.proj[0].r / PushConstants.height_to_fix_height_ratio;
        temp[1].g = ubo.proj[1].g / PushConstants.height_to_fix_height_ratio;

        gl_Position = temp * ubo.view * ubo.model * vec4(inPosition, 1.0);
    }
}
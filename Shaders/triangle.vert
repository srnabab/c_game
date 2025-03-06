#version 450

layout(push_constant) uniform _ImageRotation {
    float rotation;
} ImageRotation;

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
    if (inPosition.z == 0.1)
    {
        mat2 rotationMatrix = mat2(
            cos(ImageRotation.rotation), -sin(ImageRotation.rotation),
            sin(ImageRotation.rotation), cos(ImageRotation.rotation)
        );
        vec2 rotatedPosition = rotationMatrix * inPosition.xy;

        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(rotatedPosition, inPosition.z, 1.0);
    }
    else
    {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    }
}
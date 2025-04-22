#version 460

layout(location = 0) in vec2 uv; // Texture coordinates from full-screen quad

layout(set = 0, binding = 0) uniform sampler2D sceneColorSampler;
layout(set = 0, binding = 1) uniform sampler2D ssgiResultSampler;
layout(set = 0, binding = 2) uniform sampler2D scene2dColorSampler;

layout(location = 0) out vec4 finalColor;

void main() {
    vec3 directLightAndEmissive = texture(sceneColorSampler, uv).rgb;
    vec3 indirectLight = texture(ssgiResultSampler, uv).rgb;

    vec4 scene2dColor = texture(scene2dColorSampler, uv);
    // vec4 scene2dColor = vec4(0.0);

    // Additive blending: Combine direct and indirect lighting
    vec3 combinedLight = directLightAndEmissive + indirectLight;

    // Optional: Apply tonemapping, gamma correction etc. here or in a later pass
    // combinedLight = Tonemap(combinedLight);
    // combinedLight = pow(combinedLight, vec3(1.0/2.2)); // Gamma Correction Example

    // finalColor = vec4(combinedLight, 1.0);
    // finalColor = vec4(uv, 0.0, 1.0); // For testing: just output the UV coordinates
    finalColor = texture(sceneColorSampler, uv) * step(abs(scene2dColor.a), 0.0) + scene2dColor; // For testing: just output the scene color
}
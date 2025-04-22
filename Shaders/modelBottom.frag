#version 460

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform sampler2DShadow shadowSampler;

layout(set = 0, binding = 3) uniform directionLight
{
    mat4 lightSapceMatrix;
    vec3 lightDirection;
    vec3 lightColor;
    float lightIntensity;
} sun;

layout(set = 1, binding = 4) uniform sampler2DArray offscreenSamplers;
// layout(set = 1, binding = 4) uniform sampler2D offscreenSamplers;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inWorldNormal;
layout(location = 4) flat in uint ID;
layout(location = 5) flat in int instanceIndex;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormalBuffer;
layout(location = 2) out float outShadowFactor;

float shadowFactor(vec3 N, float NdotL)
{
    float shadowBias = 0.005;
    int pcfSamples = 9;
    float pcfRadius = 1.5;

    vec3 offsetWorldPos = inWorldPos + N * 0.0035;

    vec4 fragPosLightSpace = sun.lightSapceMatrix * vec4(offsetWorldPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    vec2 shadowCoord = projCoords.xy * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - NdotL), shadowBias);

    vec2 texelSize = 1.0 / textureSize(shadowSampler, 0);

    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x) 
    {
        for (int y = -1; y <= 1; ++y) 
        {
            vec2 offset = vec2(x, y) * texelSize * pcfRadius;
            vec2 sampleUV = shadowCoord + offset;

            if (sampleUV.x >= 0.0 && sampleUV.x <= 1.0 && sampleUV.y >= 0.0 && sampleUV.y <= 1.0) 
            {
                float shadowMapDepth = texture(shadowSampler, vec3(sampleUV, currentDepth));

                if (currentDepth <= shadowMapDepth + bias) 
                {
                    shadow += 1.0;
                }
            } 
            else 
            {
                shadow += 1.0;
            }
        }
    }

    shadow /= float(pcfSamples);

    float minShadowIntensity = 0.1;
    shadow = mix(minShadowIntensity, 1.0, shadow);

    return shadow;
}
void main() 
{
    vec3 L = normalize(-sun.lightDirection);

    vec3 N = normalize(inWorldNormal.rgb);
    float NdotL = max(dot(N, L), 0.0);

    float shadow = shadowFactor(N, NdotL);

    vec4 textureColor;
    float val = float(instanceIndex) / 48.0;

    textureColor = texture(texSampler, fragTexCoord) * abs(ID - 1) + vec4(vec2(val), 1.0 - val, 1.0) * ID;
    // textureColor = texture(texSampler, fragTexCoord) * abs(ID - 1) + texture(offscreenSamplers, vec3(fragTexCoord, float(instanceIndex))) * ID;


    vec3 diffuse = textureColor.rgb * sun.lightColor * sun.lightIntensity * NdotL;

    // vec3 V = normalize(viewDir);
    // vec3 H = normalize(L + V); // 半程向量
    // float NdotH = max(dot(N, H), 0.0);
    // float specFactor = pow(NdotH, shininess); // shininess 是材质光泽度
    // vec3 specular = specularColor * sun.lightColor * sun.lightIntensity * specFactor;

    vec3 finalColor = shadow * (diffuse); // + specular;
    
    outNormalBuffer = vec4(inWorldNormal, 1.0);

    outShadowFactor = shadow * NdotL * sun.lightIntensity;

    outColor = vec4(finalColor, textureColor.a);
}
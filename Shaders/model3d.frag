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

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inWorldNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormalBuffer;
layout(location = 2) out float outShadowFactor;

float shadowFactor(vec3 N, float NdotL)
{
    vec3 offsetWorldPos = inWorldPos + N * 0.0035;

    vec4 fragPosLightSpace = sun.lightSapceMatrix * vec4(offsetWorldPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    vec2 shadowCoord = projCoords.xy * 0.5 + 0.5;
    // shadowCoord.y = 1.0 - shadowCoord.y;
    float currentDepth = projCoords.z;

    float shadowMapMinDepth = texture(shadowSampler, vec3(shadowCoord, currentDepth));
    float bias = max(0.05 * (1.0 - NdotL), 0.055);
    float shadow = 1.0;

    if (shadowCoord.x > 1.0 || shadowCoord.x < 0.0 || shadowCoord.y > 1.0 || shadowCoord.y < 0.0 || currentDepth > shadowMapMinDepth + bias)
    {
        shadow = 0.04;
    }

    return shadow;
}
void main() 
{
    vec3 L = normalize(-sun.lightDirection);

    vec3 N = normalize(inWorldNormal.rgb);
    float NdotL = max(dot(N, L), 0.0);

    float shadow = shadowFactor(N, NdotL);

    // vec3 albedoColor = fragColor;
    vec4 textureColor = texture(texSampler, fragTexCoord);

    vec3 diffuse = textureColor.rgb * sun.lightColor * sun.lightIntensity * NdotL;

    // vec3 V = normalize(viewDir);
    // vec3 H = normalize(L + V); // 半程向量
    // float NdotH = max(dot(N, H), 0.0);
    // float specFactor = pow(NdotH, shininess); // shininess 是材质光泽度
    // vec3 specular = specularColor * sun.lightColor * sun.lightIntensity * specFactor;

    vec3 finalColor = shadow * (diffuse); // + specular;
    
    outNormalBuffer = vec4(inWorldNormal * 0.5 + 0.5, 1.0);

    outShadowFactor = shadow * NdotL * sun.lightIntensity;

    outColor = vec4(finalColor, textureColor.a);
    // outColor = vec4(fragPosLightSpace);
    // outColor = vec4(projCoords, 1.0);
    // outColor = vec4(shadowMapMinDepth);
    // outColor = vec4(vec3(bias), 1.0);
    // outColor = vec4(vec3(inWorldPos), 1.0);
    // outColor = vec4(vec3(currentDepth), 1.0);
    // outColor = vec4(textureColor.rgb * sun.lightIntensity, 1.0);
    // outColor = vec4(vec3(NdotL), 1.0);// debug
    // outColor = vec4(L * 0.5 + 0.5, 1.0);// debug
}
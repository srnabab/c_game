#version 460

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 2) uniform directionLight
{
    vec3 lightDirection;
    vec3 lightColor;
    float lightIntensity;
} sun;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 outWorldPos;
layout(location = 3) in vec3 outWorldNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormalBuffer;

void main() 
{
    vec3 L = normalize(-sun.lightDirection);

    outNormalBuffer = vec4(outWorldNormal * 0.5 + 0.5, 1.0);
    vec3 N = normalize(outNormalBuffer.rgb);
    float NdotL = max(dot(N, L), 0.0);


    // vec3 albedoColor = fragColor;
    vec4 textureColor = texture(texSampler, fragTexCoord);

    vec3 diffuse = textureColor.rgb * sun.lightColor * sun.lightIntensity * NdotL;

    // vec3 V = normalize(viewDir);
    // vec3 H = normalize(L + V); // 半程向量
    // float NdotH = max(dot(N, H), 0.0);
    // float specFactor = pow(NdotH, shininess); // shininess 是材质光泽度
    // vec3 specular = specularColor * sun.lightColor * sun.lightIntensity * specFactor;

    vec3 finalColor = diffuse; // + specular;
    

    outColor = vec4(textureColor.rgb * finalColor, textureColor.a);
    // outColor = vec4(textureColor.rgb * sun.lightIntensity, 1.0);
    // outColor = vec4(vec3(NdotL), 1.0);// debug
    // outColor = vec4(L * 0.5 + 0.5, 1.0);// debug
}
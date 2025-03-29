#include "G_constants.h"

#include "SDL3/SDL_stdinc.h"

#include "vulkan/vulkan.h"

#include "G_resource.h"

#include "cglm/cglm.h"

#ifndef VK_STRUCT_H
#define VK_STRUCT_H 1

#include "SDL3/SDL_begin_code.h"

typedef struct _QueueFamily
{
    Uint32 familyIndice;
    Uint32 queueCount;
} QueueFamily;

typedef struct _QueueFamilyIndices{
    QueueFamily graphicsFamily;
    QueueFamily presentFamily;
    QueueFamily computeFamily;
    QueueFamily transferFamily;
}QueueFamilyIndices;

//same as layout in vertex shader code
typedef struct _Vertex 
{
    vec3 pos;
    vec3 color;
    vec2 texCoord;
}Vertex;

typedef struct _Vertex4
{
    vec3 pos;
    vec3 color;
    vec2 texCoord;
    vec3 normal;
} Vertex4;

typedef struct _UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
}UniformBufferObject;

typedef struct _ComputeUniformBufferObject
{
    float deltaTime;//4 bytes
    char align[50];
}ComputeUniformBufferObject;

struct _SSGIUniformBufferObject
{
    mat4 projectionMatrix;
    mat4 inverseProjectionMatrix;
    vec3 cameraPosition;
    float rayStepSize;
    int32_t maxRaySteps;
    float ssgiStrength;
};
typedef struct _SSGIUniformBufferObject SSGIUniformBufferObject;

struct _DirectionLight
{
    vec3 lightDirection;
    char align[4];
    vec3 lightColor;
    float lightIntensity;
};
typedef struct _DirectionLight DirectionLight;

typedef struct _Particle
{
    vec2 position;
    vec2 velocity;
    vec4 color;
}Particle;

typedef struct _PushConstants
{
    float rotation;
    // float height_to_fix_height_ratio;
} PushConstants;

enum _FormatQualifier
{
    UNORM = 0,
    SNORM = 1,
    USCALED = 2,
    SSCALED = 3,
    UINT = 4,
    SINT = 5,
    SRGB = 6
};
typedef enum _FormatQualifier FormatQualifier;

#include "SDL3/SDL_close_code.h"

#endif
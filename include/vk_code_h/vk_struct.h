#include "G_constants.h"

#include "SDL3/SDL_stdinc.h"

#include "vulkan/vulkan.h"

#include "G_texture.h"

#include "cglm/cglm.h"

#ifndef VK_STRUCT_H
#define VK_STRUCT_H 1

#include "SDL3/SDL_begin_code.h"

typedef struct _QueueFamily
{
    int32_t familyIndice;
    int32_t queueCount;
} QueueFamily;

typedef struct _QueueFamilyIndices{
    QueueFamily graphicsFamily;
    QueueFamily presentFamily;
    QueueFamily computeFamily;
    QueueFamily transferFamily;
}QueueFamilyIndices;

//same as layout in vertex shader code
typedef struct _Vertex23_
{
    vec2 pos;
    vec3 color;
    char align[4];
} Vertex23_;

typedef struct _Vertex33
{
    vec3 pos;
    vec3 color;
} Vertex33_;

typedef struct _Vertex3_2
{
    vec3 pos;
    char align[4];
    vec2 texCoord;
} Vertex3_2_;

typedef struct _Vertex332
{
    vec3 pos;
    vec3 color;
    vec2 texCoord;
} Vertex332_;

typedef struct _Vertex3_23
{
    vec3 pos;
    vec2 texCoord;
    vec3 normal;
} Vertex3_23;

typedef struct _Vertex3323
{
    vec3 pos;
    vec3 color;
    vec2 texCoord;
    vec3 normal;
    char align[4];
} Vertex3323;

typedef struct _UniformBufferObject 
{
    // mat4 model;
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
    mat4 lightSpace;
    vec3 lightDirection;
    char align[4];
    vec3 lightColor;
    float lightIntensity;
};
typedef struct _DirectionLight DirectionLight;

struct _LightSpace
{
    mat4 lightSpace;
};
typedef struct _LightSpace LightSpace;

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

struct _ShapeConstants
{
    vec2 pos;
    vec2 scale;
};
typedef struct _ShapeConstants ShapeConstants;

#include "SDL3/SDL_close_code.h"

#endif
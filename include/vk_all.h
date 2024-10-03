//useful common structs, functions and headers for the others
#ifndef VK_ALL_H
#define VK_ALL_H

#define MAX_FRAMES_IN_FLIGHT 2
#define PARTICLE_COUNT 8192

#include "std_c.h"
#include "vulkan.h"
#include "SDL3/SDL.h"

#include "cglm.h"
#include "judge.h"
#include "file.h"
#include "debug.h"

typedef struct _QueueFamilyIndices{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    uint32_t computeFamily;
}QueueFamilyIndices;

//same as layout in vertex shader code
typedef struct _Vertex 
{
    vec3 pos;
    vec3 color;
    vec2 texCoord;
}Vertex;

typedef struct _UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
}UniformBufferObject;

typedef struct _ComputeUniformBufferObject
{
    float deltaTime;
}ComputeUniformBufferObject;

typedef struct _Particle
{
    vec2 position;
    vec2 velocity;
    vec4 color;
}Particle;

typedef struct _DevicePack
{
    VkDevice * pDevice;
    VkPhysicalDevice * pPhysicalDevice;
}DevicePack;

typedef struct _Recreate
{
    DevicePack DevicePack;

    VkSurfaceCapabilitiesKHR * pSurfaceCapabilities;
    VkSurfaceFormatKHR * pSurfaceFormat;
    VkPresentModeKHR * pPresentMode;

    VkSurfaceKHR * pSurface;
    VkExtent2D * pExtent2D;

    QueueFamilyIndices * pIndices;
    VkQueue * pGraphicQueue;
    
    VkFormat swapchainFormat;
    VkSwapchainKHR * pSwapchain;
    VkCommandPool * pSwapchainCommandPool;

    uint32_t * imageCount;
    VkImage ** ppSwapchainImages;
    VkImageView ** ppSwapchainImageViews;

    VkFramebuffer ** ppSwapchainFramebuffer;

    VkImage * pDepthImage;
    VkImageView * pDepthImageView;
    VkDeviceMemory * pDepthImageMem;

    VkRenderPass * pRenderPass;

}Recreate;

typedef struct _VK_ALL
{
    VkDevice * pDevice;
    VkQueue * pGraphicQueue;
    VkQueue * pPresentQueue;

    VkQueue * pComputeQueue;

    VkExtent2D * pExtent2D;
    VkSwapchainKHR * pSwapchain;

    VkPipelineLayout * pGraphicPipelineLayout;

    VkPipelineLayout * pParticlePipelineLayout;

    VkPipelineLayout * pComputePipelineLayout;

    VkRenderPass * pRenderPass;

    VkPipeline * pGraphicPipeline;

    VkPipeline * pParticlePipeline;

    VkPipeline * pComputePipeline;

    VkFramebuffer ** ppSwapchainFramebuffer;
    VkCommandPool * pSwapchainCommandPool;
    
    VkBuffer * pVertexBuffer;
    Vertex ** ppVertices;
    uint32_t * pVerticesCount;

    VkBuffer * pIndexBuffer;
    uint32_t * pIndicesCount;

    void *** pppGraphicUniformBufferMapped;

    UniformBufferObject * pGraphicUbo;

    VkDescriptorSet ** ppGraphicDescriptorSets;

    VkDescriptorSet ** ppParticleDescriptorSets;

    VkBuffer ** ppShaderStorageBuffers;

    ComputeUniformBufferObject * pComputeUbo;

    void *** pppComputeUniformBufferMapped;

    VkDescriptorSet ** ppComputeDescriptorSets;

    VkCommandBuffer ** ppCommandBuffer;

    VkCommandBuffer ** ppComputeCommandBuffer;

    VkSemaphore ** ppImageAvailableSemaphore;
    VkSemaphore ** ppRenderFinishedSemaphore;

    VkFence ** ppInFlightFence;

    VkSemaphore ** ppComputeFinishedSemaphore;

    VkFence ** ppComputeInFlightFence;

    uint32_t * pCurrentFrame;

    VkBuffer * pMovingStagingBuffer;

    void ** ppMovingBufferMapped;

    float * pCamera_X;
    float * pCamera_Y;
    
    float * pPictureX;
    float * pPictureY;
} VK_ALL;

static inline void debug_printf(char * str)
{
    if (DEBUG)
        puts(str);
}

#endif //vk_all.h
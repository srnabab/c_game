#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"
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
    float deltaTime;//4 bytes
    char align[50];
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

typedef struct _ImageRotate
{
    float rotation;
} ImageRotate;

typedef struct _VK_ALL
{
    VkPhysicalDevice * pPhysicalDevice;

    VkDevice * pDevice;

    VkSurfaceCapabilitiesKHR * pSurfaceCapabilities;
    VkSurfaceFormatKHR * pSurfaceFormat;
    VkPresentModeKHR * pPresentMode;

    VkSurfaceKHR * pSurface;
    
    QueueFamilyIndices * pQueueFamilyIndices;
    VkQueue * pGraphicQueue;
    VkQueue * pPresentQueue;
    VkQueue * pComputeQueue;

    VkExtent2D * pExtent2D;
    VkExtent2D * pOldExtent2D;
    Uint32 * imageCount;

    VkFormat swapchainFormat;
    VkSwapchainKHR * pSwapchain;

    VkPipelineLayout * pGraphicPipelineLayout;

    VkPipelineLayout * pParticlePipelineLayout;

    VkPipelineLayout * pComputePipelineLayout;

    VkRenderPass * pRenderPass;

    VkPipeline * pGraphicPipeline;

    VkPipeline * pParticlePipeline;

    VkPipeline * pComputePipeline;

    VkImage ** ppSwapchainImages;
    VkImageView ** ppSwapchainImageViews;
    VkFramebuffer ** ppSwapchainFramebuffer;
    VkCommandPool * pSwapchainCommandPool;
    
    VkImage * pDepthImage;
    VkImageView * pDepthImageView;
    VkDeviceMemory * pDepthImageMem;

    VkBuffer * pVertexBuffer;
    Vertex ** ppVertices;
    Uint32 * pVerticesCount;
    VkDeviceMemory * pVertexBufferMem;
    void ** ppVertexBufferMemMapped;

    VkBuffer * pIndexBuffer;
    Uint16 ** ppIndices;
    Uint32 * pIndicesCount;
    VkDeviceMemory * pIndexBufferMem;
    void ** ppIndexBufferMemMapped;

    VkSampler * pTextureSampler;

    VkBuffer ** ppGraphicUniformBuffer;
    void *** pppGraphicUniformBufferMapped;

    UniformBufferObject * pGraphicUbo;

    VkDescriptorSet ** ppGraphicDescriptorSets;

    VkDescriptorSet ** ppParticleDescriptorSets;

    VkBuffer ** ppShaderStorageBuffers;

    struct _ComputeUniformBufferObject * pComputeUbo;

    void *** pppComputeUniformBufferMapped;

    VkDescriptorSet ** ppComputeDescriptorSets;

    VkCommandBuffer ** ppCommandBuffer;

    VkCommandBuffer ** ppComputeCommandBuffer;

    VkSemaphore ** ppImageAvailableSemaphore;
    VkSemaphore ** ppRenderFinishedSemaphore;

    VkFence ** ppInFlightFence;

    VkSemaphore ** ppComputeFinishedSemaphore;

    VkFence ** ppComputeInFlightFence;

    Uint32 * pCurrentFrame;

    float * pCamera_X;
    float * pCamera_Y;
    
    float * pPictureX;
    float * pPictureY;

    ImageRotate * pImageRotate;
} VK_ALL;

#include "SDL3/SDL_close_code.h"

#endif
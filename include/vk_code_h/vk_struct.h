#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#include "G_constants.h"
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

typedef struct _PushConstants
{
    float rotation;
    float height_to_fix_height_ratio;
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

struct _VK_ALL
{
    VkAllocationCallbacks * pAllocationCallbacks;

    VkInstance * pInstance;

    VkPhysicalDevice * pPhysicalDevice;

    VkDevice * pDevice;

    VkSurfaceCapabilitiesKHR * pSurface2DCapabilities;
    VkSurfaceFormatKHR * pSurface2DFormat;
    VkPresentModeKHR * pPresentMode2D;

    VkSurfaceKHR * pSurface2D;
    
    QueueFamilyIndices * pQueueFamilyIndices;
    VkQueue * pGraphicQueue;
    VkQueue * pPresentQueue;
    VkQueue * pComputeQueue;
    VkQueue * pTransferQueue;
    
    VkCommandPool * pGraphicCommandPool;
    VkCommandPool * pPresentCommandPool;
    VkCommandPool * pComputeCommandPool;
    VkCommandPool * pTransferCommandPool;

    VkExtent2D * pExtent2D;
    VkExtent2D * pOldExtent2D;
    Uint32 * pImageCount2D;

    VkSwapchainKHR * pSwapchain2D;

    VkPipelineLayout * pGraphicPipelineLayout;

    VkPipelineLayout * pParticlePipelineLayout;

    VkPipelineLayout * pComputePipelineLayout;

    VkRenderPass * pRenderPass;

    VkPipeline * pGraphicPipeline;

    VkPipeline * pParticlePipeline;

    VkPipeline * pComputePipeline;

    VkImage ** ppSwapchain2DImages;
    VkImageView ** ppSwapchain2DImageViews;
    VkFramebuffer ** ppSwapchain2DFramebuffer;

#if WINDOW_3D_DEBUG
    
    VkSurfaceKHR * pSurface3D;

    VkSurfaceCapabilitiesKHR * pSurface3DCapabilities;
    VkSurfaceFormatKHR * pSurface3DFormat;
    VkPresentModeKHR * pPresentMode3D;

    VkSwapchainKHR * pSwapchain3D;
    Uint32 * pImageCount3D;

    VkImage ** ppSwapchain3DImages;
    VkImageView ** ppSwapchain3DImageViews;
    VkFramebuffer ** ppSwapchain3DFramebuffer;
#endif

    VkBuffer (*pVertexBuffer2D)[MAX_FRAMES_IN_FLIGHT];
    Uint32 maxVertices2DCount;
    Vertex ** ppVertices2D;
    Uint32 * pVertices2DCount;
    VkDeviceMemory (*pVertexBuffer2DMem)[MAX_FRAMES_IN_FLIGHT];
    void* (*ppVertexBuffer2DMemMapped)[MAX_FRAMES_IN_FLIGHT];

    VkBuffer (*pIndexBuffer2D)[1];
    Uint16 ** ppIndices2D;
    Uint32 * pIndices2DCount;
    VkDeviceMemory (*pIndexBuffer2DMem)[1];
    void* (*ppIndexBuffer2DMemMapped)[1];

    VkBuffer (*pVertexBuffer3D)[MAX_FRAMES_IN_FLIGHT];
    Uint32 maxVertices3DCount;
    Vertex ** ppVertices3D;
    Uint32 * pVertices3DCount;
    VkDeviceMemory (*pVertexBuffer3DMem)[MAX_FRAMES_IN_FLIGHT];
    void* (*ppVertexBuffer3DMemMapped)[MAX_FRAMES_IN_FLIGHT];

    VkBuffer (*pIndexBuffer3D)[MAX_FRAMES_IN_FLIGHT];
    Uint32 ** ppIndices3D;
    Uint32 * pIndices3DCount;
    VkDeviceMemory (*pIndexBuffer3DMem)[MAX_FRAMES_IN_FLIGHT];
    void* (*ppIndexBuffer3DMemMapped)[MAX_FRAMES_IN_FLIGHT];

    VkSampler * pTextureSampler;

    VkBuffer (*ppGraphicUniformBuffer)[MAX_FRAMES_IN_FLIGHT];
    void * (*pppGraphicUniformBufferMapped)[MAX_FRAMES_IN_FLIGHT];

    UniformBufferObject * pGraphicUbo;

    VkDescriptorSet ** ppGraphicDescriptorSets;

    VkDescriptorSet ** ppParticleDescriptorSets;

    VkBuffer (*ppShaderStorageBuffers)[MAX_FRAMES_IN_FLIGHT];

    struct _ComputeUniformBufferObject * pComputeUbo;

    void* (*pppComputeUniformBufferMapped)[MAX_FRAMES_IN_FLIGHT];

    VkDescriptorSet ** ppComputeDescriptorSets;

    VkCommandBuffer (*ppGraphicCommandBuffer)[MAX_FRAMES_IN_FLIGHT];
    VkCommandBuffer (*ppPresentCommandBuffer)[MAX_FRAMES_IN_FLIGHT];
    VkCommandBuffer (*ppComputeCommandBuffer)[MAX_FRAMES_IN_FLIGHT];
    VkCommandBuffer (*ppTransferCommandBuffer)[MAX_FRAMES_IN_FLIGHT];

    VkSemaphore (*ppImageAvailableSemaphore)[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore (*ppRenderFinishedSemaphore)[MAX_FRAMES_IN_FLIGHT];

    VkFence (*ppGraphicInFlightFence)[MAX_FRAMES_IN_FLIGHT];

    VkSemaphore (*ppComputeFinishedSemaphore)[MAX_FRAMES_IN_FLIGHT];

    VkFence (*ppComputeInFlightFence)[MAX_FRAMES_IN_FLIGHT];

    Uint32 * pCurrentFrame;

    float * pCamera_X;
    float * pCamera_Y;
    
    float * pPictureX;
    float * pPictureY;

    PushConstants * pPushConstants;

    G_Texture_P * pGlobalTexture;
};
typedef struct _VK_ALL VK_ALL;

#include "SDL3/SDL_close_code.h"

#endif
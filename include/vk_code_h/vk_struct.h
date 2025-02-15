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

typedef struct _ImageRotate
{
    float rotation;
} ImageRotate;

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

typedef struct _VK_ALL
{
    VkAllocationCallbacks * pAllocationCallbacks;

    VkInstance * pInstance;

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
    VkQueue * pTransferQueue;
    
    VkCommandPool * pGraphicCommandPool;
    VkCommandPool * pPresentCommandPool;
    VkCommandPool * pComputeCommandPool;
    VkCommandPool * pTransferCommandPool;

    VkExtent2D * pExtent2D;
    VkExtent2D * pOldExtent2D;
    Uint32 * pImageCount;

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

    // VkFormat * pDepthFormat;
    // VkImage * pDepthImage;
    // VkImageView * pDepthImageView;
    // VkDeviceMemory * pDepthImageMem;

    VkBuffer * pVertexBuffer;
    Uint32 maxVerticesCount;
    void ** ppVertices;
    vec3 ** ppVertices_Pos;
    vec3 ** ppVertices_Color;
    vec2 ** ppVertices_TexCoord;
    Uint32 * pVerticesCount;
    VkDeviceMemory * pVertexBufferMem;
    void ** ppVertexBufferMemMapped;

    VkBuffer * pIndexBuffer;
    Uint16 ** ppIndices;
    Uint32 * pIndicesCount;
    VkDeviceMemory * pIndexBufferMem;
    void ** ppIndexBufferMemMapped;

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

    ImageRotate * pImageRotate;

    G_Texture * pGlobalTexture;
} VK_ALL;

#include "SDL3/SDL_close_code.h"

#endif
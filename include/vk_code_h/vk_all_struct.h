#include "G_constants.h"

#include "vk_code_h/vk_struct.h"
#include "G_staticModel.h"
#include "G_stack.h"

#ifndef VK_ALL_STRUCT
#define VK_ALL_STRUCT 1

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
    VkImage ** ppSwapchain2DImages;
    VkImageView ** ppSwapchain2DImageViews;
    VkFramebuffer ** ppSwapchain2DFramebuffer;


    VkRenderPass * pRenderPass;

    VkPipelineLayout * pGraphicPipelineLayout;
    VkPipeline * pGraphicPipeline;

    VkPipelineLayout * pParticlePipelineLayout;
    VkPipeline * pParticlePipeline;

    VkPipelineLayout * pComputePipelineLayout;
    VkPipeline * pComputePipeline;

    VkPipelineLayout * pModelPipelineLayout;
    VkPipeline * pModelPipeline;


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

    G_StaticModelPool * pStaticModelPool;

    VkSampler * pTextureSampler;

    VkBuffer (*ppGraphicUniformBuffer)[MAX_FRAMES_IN_FLIGHT];
    void * (*pppGraphicUniformBufferMapped)[MAX_FRAMES_IN_FLIGHT];
    UniformBufferObject * pGraphicUbo;

    VkBuffer (*ppGraphic3DUniformBuffer)[MAX_FRAMES_IN_FLIGHT];
    void * (*pppGraphic3DUniformBufferMapped)[MAX_FRAMES_IN_FLIGHT];
    UniformBufferObject * pGraphic3DUbo;

    VkBuffer (*ppUIUniformBuffer)[MAX_FRAMES_IN_FLIGHT];
    void * (*pppUIUniformBufferMapped)[MAX_FRAMES_IN_FLIGHT];
    UniformBufferObject * pUIUbo;


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

    G_Stack shaderModuleStack;
};
typedef struct _VK_ALL VK_ALL;

#endif
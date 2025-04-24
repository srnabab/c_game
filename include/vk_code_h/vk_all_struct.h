#include "G_constants.h"

#include "vk_code_h/vk_struct.h"
#include "G_staticModel.h"
#include "G_stack.h"

#ifndef VK_ALL_STRUCT
#define VK_ALL_STRUCT 1

struct _VK_ALL
{
    VkAllocationCallbacks * pAllocationCallbacks;

    VkInstance instance;

    VkPhysicalDevice physicalDevice;

    VkDevice device;

    // VkSurfaceCapabilitiesKHR * pSurface2DCapabilities;
    // VkSurfaceFormatKHR * pSurface2DFormat;
    // VkPresentModeKHR * pPresentMode2D;

    // VkSurfaceKHR * pSurface2D;
    
    QueueFamilyIndices queueFamilyIndices;
    VkQueue pGraphicQueue[16];
    VkQueue pPresentQueue[16];
    VkQueue pComputeQueue[16];
    VkQueue pTransferQueue[16];
    
    VkCommandPool graphicCommandPool;
    VkCommandPool presentCommandPool;
    VkCommandPool computeCommandPool;
    VkCommandPool transferCommandPool;

    VkExtent2D extent2D;
    VkExtent2D oldExtent2D;

    // Uint32 * pImageCount2D;
    // VkSwapchainKHR * pSwapchain2D;
    // VkImage ** ppSwapchain2DImages;
    // VkImageView ** ppSwapchain2DImageViews;
    // VkFramebuffer ** ppSwapchain2DFramebuffer;


    VkRenderPass renderPass;
    // VkRenderPass * pCombine2DRenderPass;
    VkRenderPass modelRenderPass;
    VkRenderPass offscreenRenderPass;
    VkRenderPass shadowRenderPass;
    VkRenderPass combineRenderPass;

    VkPipelineLayout graphicPipelineLayout;
    VkPipeline graphicPipeline;

    VkPipelineLayout particlePipelineLayout;
    VkPipeline particlePipeline;

    VkPipelineLayout computePipelineLayout;
    VkPipeline computePipeline;

    VkPipelineLayout modelPipelineLayout;
    VkPipeline modelPipeline;

    VkPipelineLayout bottomPipelineLayout;
    VkPipeline bottomPipeline;

    VkPipelineLayout shadowPipelineLayout;
    VkPipeline shadowPipeline;

    VkPipelineLayout SSGIPipelineLayout;
    VkPipeline SSGIPipeline;

    VkPipelineLayout combinePipelineLayout;
    VkPipeline combinePipeline;

    // VkPipelineLayout * pCombine2DPipelineLayout;
    // VkPipeline * pCombine2DPipeline;

    VkSurfaceKHR surface3D;

    VkSurfaceCapabilitiesKHR surface3DCapabilities;
    VkSurfaceFormatKHR surface3DFormat;
    VkPresentModeKHR presentMode3D;

    VkSwapchainKHR swapchain3D;
    Uint32 imageCount3D;

    VkImage * pSwapchain3DImages;
    VkImageView * pSwapchain3DImageViews;

    VkFramebuffer * pGraphic2dFramebuffer;
    VkFramebuffer * pSwapchain3DFramebuffer;
    VkFramebuffer * pShadowFramebuffer;
    VkFramebuffer * pDirectColorFramebuffer;
    VkFramebuffer * pCombineFramebuffer;
    VkFramebuffer * pBottomImageArrayFramebuffers;

    VkBuffer vertexBuffer2D[MAX_FRAMES_IN_FLIGHT];
    Uint32 maxVertices2DCount;
    Vertex * pVertices2D;
    Uint32 vertices2DCount;
    VkDeviceMemory pVertexBuffer2DMem[MAX_FRAMES_IN_FLIGHT];
    void* pVertexBuffer2DMemMapped[MAX_FRAMES_IN_FLIGHT];

    VkBuffer indexBuffer2D;
    Uint16 * pIndices2D;
    Uint32 indices2DCount;
    VkDeviceMemory indexBuffer2DMem;
    void* pIndexBuffer2DMemMapped;

    VkBuffer vertexBuffer3D[MAX_FRAMES_IN_FLIGHT];
    Uint32 maxVertices3DCount;
    Vertex4 * pVertices3D;
    Uint32 vertices3DCount;
    VkDeviceMemory vertexBuffer3DMem[MAX_FRAMES_IN_FLIGHT];
    void* pVertexBuffer3DMemMapped[MAX_FRAMES_IN_FLIGHT];

    VkBuffer indexBuffer3D[MAX_FRAMES_IN_FLIGHT];
    Uint32 * pIndices3D;
    Uint32 indices3DCount;
    VkDeviceMemory indexBuffer3DMem[MAX_FRAMES_IN_FLIGHT];
    void* pIndexBuffer3DMemMapped[MAX_FRAMES_IN_FLIGHT];

    G_StaticModelPool * pStaticModelPool;

    VkSampler textureSampler;
    VkSampler normalSampler;
    VkSampler depthSampler;
    VkSampler shadowSampler;

    VkBuffer pGraphicUniformBuffer[MAX_FRAMES_IN_FLIGHT];
    void * ppGraphicUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
    UniformBufferObject * pGraphicUbo;

    VkBuffer pGraphic3DUniformBuffer[MAX_FRAMES_IN_FLIGHT];
    void * ppGraphic3DUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
    UniformBufferObject * pGraphic3DUbo;

    VkBuffer pUIUniformBuffer[MAX_FRAMES_IN_FLIGHT];
    void * ppUIUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
    UniformBufferObject * pUIUbo;

    VkBuffer pComputeUniformBuffer[MAX_FRAMES_IN_FLIGHT];
    void* ppComputeUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
    struct _ComputeUniformBufferObject * pComputeUbo;

    VkBuffer pSSGIUniformBuffer[MAX_FRAMES_IN_FLIGHT];
    void * ppSSGIUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
    SSGIUniformBufferObject * pSSGIubo;

    VkBuffer pLightSpaceUniformBuffer[MAX_FRAMES_IN_FLIGHT];
    void * ppLightSpaceUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
    LightSpace * pLightSpaceUbo;

    VkBuffer pSunUniformBuffer[MAX_FRAMES_IN_FLIGHT];
    void * ppSunUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
    DirectionLight * pSunubo;

    VkBuffer pShaderStorageBuffer[MAX_FRAMES_IN_FLIGHT];

    VkDescriptorSet * pGraphicDescriptorSets;
    VkDescriptorSet * pParticleDescriptorSets;
    VkDescriptorSet * pComputeDescriptorSets;
    VkDescriptorSet * pModelDescriptorSets;
    VkDescriptorSet * pBottomDescriptorSets;

    VkDescriptorSet * pShadowDescriptorSets;
    VkDescriptorSet * pSSGIDescriptorSets;
    VkDescriptorSet * pCombineDescriptorSets;
    // VkDescriptorSet ** ppCombine2dDescriptorSets;

    VkCommandBuffer pGraphicCommandBuffer[MAX_FRAMES_IN_FLIGHT];
    VkCommandBuffer pPresentCommandBuffer[MAX_FRAMES_IN_FLIGHT];
    VkCommandBuffer pComputeCommandBuffer[MAX_FRAMES_IN_FLIGHT];
    VkCommandBuffer pTransferCommandBuffer[MAX_FRAMES_IN_FLIGHT];

    VkSemaphore pTimelineSemaphore2d[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore pTimelineSemaphore3d[MAX_FRAMES_IN_FLIGHT];

    VkSemaphore pImageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore pRenderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore pCopyFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore pComputeSemaphore[MAX_FRAMES_IN_FLIGHT];

    VkFence pGraphicInFlightFence[MAX_FRAMES_IN_FLIGHT];
    VkFence pComputeInFlightFence[MAX_FRAMES_IN_FLIGHT];

    Uint32 currentFrame;

    float * pCamera_X;
    float * pCamera_Y;
    
    float * pPictureX;
    float * pPictureY;

    PushConstants * pPushConstants;

    G_Texture_Head * pGlobalTexture;

    G_Stack shaderModuleStack;

    G_Stack bottomImageMoveStack;
    G_Stack bottomImageDrawStack;

    Uint32 timelineSemaphoreSignalValue;
};
typedef struct _VK_ALL VK_ALL;

#endif
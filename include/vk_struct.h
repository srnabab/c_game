#include "vulkan/vulkan.h"
#include "cglm/cglm.h"

#ifndef VK_STRUCT_H
#define VK_STRUCT_H 1

typedef struct _QueueFamily
{
    uint32_t familyIndice;
    uint32_t queueCount;
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

typedef struct _Recreate
{
    DevicePack DevicePack;

    VkSurfaceCapabilitiesKHR * pSurfaceCapabilities;
    VkSurfaceFormatKHR * pSurfaceFormat;
    VkPresentModeKHR * pPresentMode;

    VkSurfaceKHR * pSurface;
    VkExtent2D * pOldExtent2D;
    VkExtent2D * pExtent2D;
    Vertex ** ppVertices;

    QueueFamilyIndices * pIndices;
    VkQueue * pGraphicQueue;
    
    VkFormat swapchainFormat;
    VkSwapchainKHR * pSwapchain;
    VkCommandPool * pSwapchainCommandPool;

    uint32_t * imageCount;
    VkImage ** ppSwapchainImages;
    VkImageView ** ppSwapchainImageViews;

    VkImageView ** ppSwapchainImageViews2;

    VkFramebuffer ** ppSwapchainFramebuffer;

    VkImage * pDepthImage;
    VkImageView * pDepthImageView;
    VkDeviceMemory * pDepthImageMem;

    VkRenderPass * pRenderPass;

}Recreate;

typedef struct _VK_ALL
{
    VkPhysicalDevice * pPhysicalDevice;

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
    VkDeviceMemory * pVertexBufferMem;
    void ** ppVertexBufferMemMapped;

    VkBuffer * pIndexBuffer;
    uint16_t ** ppIndices;
    uint32_t * pIndicesCount;
    VkDeviceMemory * pIndexBufferMem;
    void ** ppIndexBufferMemMapped;

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

    uint32_t * pCurrentFrame;

    float * pCamera_X;
    float * pCamera_Y;
    
    float * pPictureX;
    float * pPictureY;

    ImageRotate * pImageRotate;
} VK_ALL;

#endif
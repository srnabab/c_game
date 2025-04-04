#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "vulkan/vulkan.h"

#ifndef VK_COLLECTION_H
#define VK_COLLECTION_H 1

struct _BUFFER_PACK
{
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void* cpuMem;
    bool mapped;
};
typedef struct _BUFFER_PACK BUFFER_PACK;

struct _VK_COLLECTION
{
    SDL_Window * windows[2];
    Uint32 windowCount;

    VkInstance instance;

    VkSurfaceKHR surface[2];
    Uint32 surfaceCount;

    VkDevice device;

    VkSwapchainKHR swapchains[2];
    Uint32 swapchainCount;
    void* swapchainImageMem[2];
    Uint32 swapchainImageMemCount;

    VkImageView * swapchainImageViews;
    Uint32 swapchainImageViewCount;
    void ** swapchainImageViewMem;
    Uint32 swapchainImageViewMemCount;

    VkRenderPass * renderPasses;
    Uint32 renderPassCount;

    VkCommandPool * commandPools;
    Uint32 commandPoolCount;

    VkFramebuffer * frameBuffers;
    Uint32 frameBufferCount;
    void** frameBufferMem;
    Uint32 frameBufferMemCount;

    VkSampler * samplers;
    Uint32 samplerCount;

    VkShaderModule * shaderModules;
    Uint32 shaderModuleCount;

    VkPipelineShaderStageCreateInfo ** shaderStageCreateInfos;
    Uint32 shaderStageCreateInfoCount;

    VkDescriptorSetLayout * descriptorSetLayouts;
    Uint32 descriptorSetLayoutCount;
    void ** descriptorSetLayoutMem;
    Uint32 descriptorSetLayoutMemCount;

    VkPipelineLayout * pipelineLayouts;
    Uint32 pipelineLayoutCount;

    VkPipeline * pipelines;
    Uint32 pipelineCount;

    VkDescriptorPool * descriptorPool;
    Uint32 descriptorPoolCount;

    VkSemaphore * semaphores;
    Uint32 semaphoreCount;

    VkFence * fences;
    Uint32 fenceCount;

    BUFFER_PACK * buffers;
    Uint32 bufferCount;
};
typedef struct _VK_COLLECTION VK_COLLECTION;

#include "SDL3/SDL_begin_code.h"

extern bool SDLCALL CO_addWindow(SDL_Window * window);
extern void SDLCALL initCollection(void);
extern bool SDLCALL CO_addInstance(VkInstance instance);
extern bool SDLCALL CO_addSurface(VkSurfaceKHR surface);
extern bool SDLCALL CO_addDevice(VkDevice device);
extern bool SDLCALL CO_addSwapchain(VkSwapchainKHR swapchain);
extern bool SDLCALL CO_addSwapchainImage(void * swapchainImage);
extern bool SDLCALL CO_addSwapchainImageView(Uint32 count, VkImageView * swapchainImageView);
extern bool SDLCALL CO_addRenderPass(VkRenderPass renderPass);
extern bool SDLCALL CO_addFrameBuffer(Uint32 count, VkFramebuffer * pFrameBuffer);
extern bool SDLCALL CO_addCommandPool(VkCommandPool commandPool);
extern bool SDLCALL CO_addSampler(VkSampler sampler);
extern bool SDLCALL CO_addShaderModule(VkShaderModule shaderModule);
extern bool SDLCALL CO_addShaderStageCreateInfo(VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo);
extern bool SDLCALL CO_addDescriptorSetLayout(Uint32 count, VkDescriptorSetLayout * pDescriptorSetLayout);
extern bool SDLCALL CO_addPieplineLayout(VkPipelineLayout pipelineLayout);
extern bool SDLCALL CO_addPiepline(VkPipeline pipeline);
extern bool SDLCALL CO_addBuffer(bool mapped, VkBuffer buffer, VkDeviceMemory bufferMemory, void* cpuMem);
extern bool SDLCALL CO_addDescriptorPool(VkDescriptorPool descriptorPool);
extern bool SDLCALL CO_addSemaphore(VkSemaphore semaphore);
extern bool SDLCALL CO_addFence(VkFence fence);
extern void SDLCALL CO_CleanAllVkResource(void);

#include "SDL3/SDL_close_code.h"

#endif // vk_collection.h
#include "G_begin_code.h"
#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_JUDGE_H
#define VK_JUDGE_H 1

//function code, named by add a 'F' at the end of function name
typedef enum _FuncCode {
    vulkanVersionF,
    createInstanceF,
    createSurfaceF,
    pickPhysicalDeviceF,
    findQueueFamiliesF,
    checkDeviceExtensionSupportF,
    createLogicalDeviceF,
    getSurfaceFormatsF,
    getPresentModesF,
    getSurfaceCapabilitiesF,
    createSwapchainF,
    getSwapchainNumberF,
    createSwapchainImageF,
    createSwapchainImageViewsF,
    createShaderModuleF,
    createDescriptorSetLayoutF,
    createPipelineLayoutF,
    createRenderPassF,
    createGraphicsPipelineF,
    createCommandPoolF,
    createDepthResouresF,
    createFrameBufferF,
    createTextureImageF,
    createTextureImageViewF,
    createTextureSamplerF,
    createVertexBufferF,
    createIndexBufferF,
    createUniformBuffersF,
    createDescriptorPoolF,
    createDescriptorSetsF,
    createCommandbufferF,
    createSemaphoreF,
    createFenceF,
    recordCommandBufferF,
    drawFrameF,
    waitForFencesF,
    resetFencesF,
    acquireNextImageF,
    resetCommandBufferF,
    endCommandBufferF,
    queueSumbitF,
    queuePresentF,
    recreateSwapchainF,
    initializedF,
    FuncCodeMax
} FuncCode;
//base function code to clean up resources created
extern void G_CALL cleanup(FuncCode code);
//judge VK_RESULT for vulkan
extern void G_CALL resultVulkan(VkResult result, FuncCode code, Uint32 num, ...);

#endif //clean.h
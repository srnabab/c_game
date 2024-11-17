#include "std_c.h"
#include "vulkan/vulkan.h"

#ifndef JUDGE_H
#define JUDGE_H

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
    initializeMovingBufferF,
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
void cleanup(FuncCode code);
//judge VK_RESULT for vulkan
void resultVulkan(VkResult result, FuncCode code, uint32_t num, ...);

#endif //clean.h
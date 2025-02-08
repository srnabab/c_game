#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_recreate.h"

#include "G_log.h"

extern bool game_is_running;

void resultVulkan(VkResult result, FuncCode code, uint32_t num, ...)
{
    switch (result)
    {
        case VK_SUCCESS:
        break;

        case VK_SUBOPTIMAL_KHR:
        if (code == queuePresentF)
        {
            recreateSwapchain();
            //SDL_Log("VK_SUBOPTIMAL_KHR(queuePresent)\n");
        }
        else if (code == acquireNextImageF)
        {
            // logMessage("VK_SUBOPTIMAL_KHR(acquireNext)");
        }
        break;

        case VK_ERROR_OUT_OF_DATE_KHR:
        if (code == acquireNextImageF)
        {
            //SDL_Log("VK_ERROR_OUT_OF_DATE_KHR(acquireNExt)\n");
            recreateSwapchain();
        }
        if (code == queuePresentF)
        {
            recreateSwapchain();
            //SDL_Log("VK_ERROR_OUT_OF_DATE_KHR(queuePresent)\n");
        }
        break;

        case VK_ERROR_SURFACE_LOST_KHR:
        if (code == acquireNextImageF)
        {
            logMessage("VK_ERROR_SURFACE_LOST_KHR(acquireNExt)");
            recreateSwapchain();
        }
        if (code == queuePresentF)
        {
            recreateSwapchain();
            logMessage("VK_ERROR_SURFACE_LOST_KHR(queuePresent)");
        }
        break;

        default:
        logMessage("result: %d", result);
        if (num > 0)
        {
            va_list ap;
            va_start(ap, num);
            for (uint32_t i = 0;i < num;i++)
            {
                SDL_free(va_arg(ap, void *));
                logMessage("freed(%u)", i);
            }
            va_end(ap);
        }
        switch(code)
        {
            case vulkanVersionF:
            logMessage("\ncan't detect vulkan support");
            goto end;

            case createInstanceF:
            logMessage("\ncreate instance error");
            goto end;

            case createSurfaceF:
            logMessage("\ncreate surface error");
            goto end;

            case pickPhysicalDeviceF:
            logMessage("\npick physical device error");
            goto end;

            case findQueueFamiliesF:
            logMessage("\nget SurfaceSupport error");
            goto end;

            case checkDeviceExtensionSupportF:
            logMessage("\nenumerate device extension properties error");
            goto end;

            case createLogicalDeviceF:
            logMessage("\ncreate logical device failed");
            goto end;

            case getSurfaceFormatsF:
            logMessage("\nget surface format failed");
            goto end;

            case getPresentModesF:
            logMessage("\nget present modes failed");
            goto end;

            case getSurfaceCapabilitiesF:
            logMessage("\nget surface capabilitues failed");
            goto end;

            case createSwapchainF:
            logMessage("\ncreate swapchain failed");
            goto end;

            case getSwapchainNumberF:
            logMessage("\nget swapchain number failed");
            goto end;

            case createSwapchainImageF:
            logMessage("\ncreate swapchain image failed");
            goto end;

            case createSwapchainImageViewsF:
            logMessage("\ncreate image view failed");
            goto end;

            case createShaderModuleF:
            logMessage("\nshaderModule created failed");
            goto end;

            case createDescriptorSetLayoutF:
            logMessage("\ncreate descriptoy set layout failed");
            goto end;

            case createPipelineLayoutF:
            logMessage("\ncreate pipeline layout failed");
            goto end;

            case createRenderPassF:
            logMessage("\ncreate render pass failed");
            goto end;

            case createGraphicsPipelineF:
            logMessage("\ncreate graphic pipeline failed");
            goto end;

            case createCommandPoolF:
            logMessage("\ncreate command pool failed");
            goto end;

            case createDepthResouresF:
            logMessage("\ncreate depth resoures failed");
            goto end;

            case createFrameBufferF:
            logMessage("\ncreate frame buffer failed");
            goto end;

            case createTextureImageF:
            logMessage("\ncreate texture image failed");
            goto end;

            case createTextureImageViewF:
            logMessage("\ncreate texture image view failed");
            goto end;

            case createTextureSamplerF:
            logMessage("\ncreate texture sampler failed");
            goto end;

            case createVertexBufferF:
            logMessage("\ncreate vertex buffer failed");
            goto end;

            case createIndexBufferF:
            logMessage("\ncreate index buffer failed");
            goto end;

            case createUniformBuffersF:
            logMessage("\ncreate uniform buffers failed");
            goto end;

            case createDescriptorPoolF:
            logMessage("\ncreate descriptor poo l failed");
            goto end;

            case createDescriptorSetsF:
            logMessage("\ncreate descriptor sets failed");
            goto end;

            case createCommandbufferF:
            logMessage("\ncreate command buffer failed");
            goto end;

            case createSemaphoreF:
            logMessage("\ncreate semaphore failed");
            goto end;

            case createFenceF:
            logMessage("\ncreate fence failed");
            goto end;

            case recordCommandBufferF:
            logMessage("\nrecord command buffer failed");
            goto end;

            case drawFrameF:
            logMessage("\ndraw error");
            goto end;

            case waitForFencesF:
            logMessage("\nwait for fences failed");
            goto end;

            case resetFencesF:
            logMessage("\nreset fences failed");
            goto end;

            case acquireNextImageF:
            logMessage("\nacquire next images failed");
            goto end;

            case resetCommandBufferF:
            logMessage("\nreset command buffer failed");
            goto end;

            case endCommandBufferF:
            logMessage("\nend command buffer failed");
            goto end;

            case queueSumbitF:
            logMessage("\nqueue submit failed");
            goto end;

            case queuePresentF:
            logMessage("\nqueue present failed");
            goto end;

            default:
            logMessage("\nunprocessed value");
        }
    }

// Quick Quit Needed
    if (0)
    {
end:
        game_is_running = false;
    }
}
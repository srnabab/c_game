#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_recreate.h"

#include "G_log.h"

#include "SDL3/SDL_log.h"

extern bool game_is_running;

void resultVulkan(VkResult result, FuncCode code, uint32_t num, ...)
{
    switch (result)
    {
        case VK_SUCCESS:
        break;

        case VK_SUBOPTIMAL_KHR:
        SDL_Log("result: %d", result);
        recreateSwapchain();
        break;

        case VK_ERROR_OUT_OF_DATE_KHR:
        SDL_Log("result: %d", result);
        recreateSwapchain();
        break;

        case VK_ERROR_SURFACE_LOST_KHR:
        SDL_Log("result: %d", result);
        recreateSwapchain();
        break;

        default:
        SDL_Log("result: %d", result);
        if (num > 0)
        {
            va_list ap;
            va_start(ap, num);
            for (uint32_t i = 0;i < num;i++)
            {
                SDL_free(va_arg(ap, void *));
                SDL_Log("freed(%u)", i);
            }
            va_end(ap);
        }
        switch(code)
        {
            case vulkanVersionF:
            SDL_Log("\ncan't detect vulkan support");
            goto end;

            case createInstanceF:
            SDL_Log("\ncreate instance error");
            goto end;

            case createSurfaceF:
            SDL_Log("\ncreate surface error");
            goto end;

            case pickPhysicalDeviceF:
            SDL_Log("\npick physical device error");
            goto end;

            case findQueueFamiliesF:
            SDL_Log("\nget SurfaceSupport error");
            goto end;

            case checkDeviceExtensionSupportF:
            SDL_Log("\nenumerate device extension properties error");
            goto end;

            case createLogicalDeviceF:
            SDL_Log("\ncreate logical device failed");
            goto end;

            case getSurfaceFormatsF:
            SDL_Log("\nget surface format failed");
            goto end;

            case getPresentModesF:
            SDL_Log("\nget present modes failed");
            goto end;

            case getSurfaceCapabilitiesF:
            SDL_Log("\nget surface capabilitues failed");
            goto end;

            case createSwapchainF:
            SDL_Log("\ncreate swapchain failed");
            goto end;

            case getSwapchainNumberF:
            SDL_Log("\nget swapchain number failed");
            goto end;

            case createSwapchainImageF:
            SDL_Log("\ncreate swapchain image failed");
            goto end;

            case createSwapchainImageViewsF:
            SDL_Log("\ncreate image view failed");
            goto end;

            case createShaderModuleF:
            SDL_Log("\nshaderModule created failed");
            goto end;

            case createDescriptorSetLayoutF:
            SDL_Log("\ncreate descriptoy set layout failed");
            goto end;

            case createPipelineLayoutF:
            SDL_Log("\ncreate pipeline layout failed");
            goto end;

            case createRenderPassF:
            SDL_Log("\ncreate render pass failed");
            goto end;

            case createGraphicsPipelineF:
            SDL_Log("\ncreate graphic pipeline failed");
            goto end;

            case createCommandPoolF:
            SDL_Log("\ncreate command pool failed");
            goto end;

            case createDepthResouresF:
            SDL_Log("\ncreate depth resoures failed");
            goto end;

            case createFrameBufferF:
            SDL_Log("\ncreate frame buffer failed");
            goto end;

            case createTextureImageF:
            SDL_Log("\ncreate texture image failed");
            goto end;

            case createTextureImageViewF:
            SDL_Log("\ncreate texture image view failed");
            goto end;

            case createTextureSamplerF:
            SDL_Log("\ncreate texture sampler failed");
            goto end;

            case createVertexBufferF:
            SDL_Log("\ncreate vertex buffer failed");
            goto end;

            case createIndexBufferF:
            SDL_Log("\ncreate index buffer failed");
            goto end;

            case createUniformBuffersF:
            SDL_Log("\ncreate uniform buffers failed");
            goto end;

            case createDescriptorPoolF:
            SDL_Log("\ncreate descriptor poo l failed");
            goto end;

            case createDescriptorSetsF:
            SDL_Log("\ncreate descriptor sets failed");
            goto end;

            case createCommandbufferByBufferingF:
            SDL_Log("\ncreate command buffer failed");
            goto end;

            case createSemaphoreF:
            SDL_Log("\ncreate semaphore failed");
            goto end;

            case createFenceF:
            SDL_Log("\ncreate fence failed");
            goto end;

            case recordCommandBufferF:
            SDL_Log("\nrecord command buffer failed");
            goto end;

            case drawFrameF:
            SDL_Log("\ndraw error");
            goto end;

            case waitForFencesF:
            SDL_Log("\nwait for fences failed");
            goto end;

            case resetFencesF:
            SDL_Log("\nreset fences failed");
            goto end;

            case acquireNextImageF:
            SDL_Log("\nacquire next images failed");
            goto end;

            case resetCommandBufferF:
            SDL_Log("\nreset command buffer failed");
            goto end;

            case endCommandBufferF:
            SDL_Log("\nend command buffer failed");
            goto end;

            case queueSumbitF:
            SDL_Log("\nqueue submit failed");
            goto end;

            case queuePresentF:
            SDL_Log("\nqueue present failed");
            goto end;

            default:
            SDL_Log("\nunprocessed value");
        }
    }

// Quick Quit Needed
    if (0)
    {
end:
        game_is_running = false;
    }
}
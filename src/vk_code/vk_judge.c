#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_recreate.h"

#include "G_log.h" 

extern bool game_is_running;

void resultVulkan(VkResult result, FuncCode code, uint32_t num, ...)
{
    switch (result)
    {
        case VK_SUCCESS:
        return;

        case VK_SUBOPTIMAL_KHR:
        print("result: %d", result);
        // recreateSwapchain();
        break;

        case VK_ERROR_OUT_OF_DATE_KHR:
        print("result: %d", result);
        // recreateSwapchain();
        break;

        case VK_ERROR_SURFACE_LOST_KHR:
        print("result: %d", result);
        // recreateSwapchain();
        break;

        default:
        print("result: %d", result);
        if (num > 0)
        {
            va_list ap;
            va_start(ap, num);
            for (uint32_t i = 0;i < num;i++)
            {
                SDL_free(va_arg(ap, void *));
                print("freed(%u)", i);
            }
            va_end(ap);
        }
        switch(code)
        {
            case vulkanVersionF:
            print("\ncan't detect vulkan support");
            goto end;

            case createInstanceF:
            print("\ncreate instance error");
            goto end;

            case createSurfaceF:
            print("\ncreate surface error");
            goto end;

            case pickPhysicalDeviceF:
            print("\npick physical device error");
            goto end;

            case findQueueFamiliesF:
            print("\nget SurfaceSupport error");
            goto end;

            case checkDeviceExtensionSupportF:
            print("\nenumerate device extension properties error");
            goto end;

            case createLogicalDeviceF:
            print("\ncreate logical device failed");
            goto end;

            case getSurfaceFormatsF:
            print("\nget surface format failed");
            goto end;

            case getPresentModesF:
            print("\nget present modes failed");
            goto end;

            case getSurfaceCapabilitiesF:
            print("\nget surface capabilitues failed");
            goto end;

            case createSwapchainF:
            print("\ncreate swapchain failed");
            goto end;

            case getSwapchainNumberF:
            print("\nget swapchain number failed");
            goto end;

            case createSwapchainImageF:
            print("\ncreate swapchain image failed");
            goto end;

            case createSwapchainImageViewsF:
            print("\ncreate image view failed");
            goto end;

            case createShaderModuleF:
            print("\nshaderModule created failed");
            goto end;

            case createDescriptorSetLayoutF:
            print("\ncreate descriptoy set layout failed");
            goto end;

            case createPipelineLayoutF:
            print("\ncreate pipeline layout failed");
            goto end;

            case createGraphicRenderPassF:
            print("\ncreate render pass failed");
            goto end;

            case createGraphicsPipelineF:
            print("\ncreate graphic pipeline failed");
            goto end;

            case createCommandPoolF:
            print("\ncreate command pool failed");
            goto end;

            case createDepthResouresF:
            print("\ncreate depth resoures failed");
            goto end;

            case createFrameBufferF:
            print("\ncreate frame buffer failed");
            goto end;

            case createTextureImageF:
            print("\ncreate texture image failed");
            goto end;

            case createTextureImageViewF:
            print("\ncreate texture image view failed");
            goto end;

            case createTextureSamplerF:
            print("\ncreate texture sampler failed");
            goto end;

            case createVertexBufferF:
            print("\ncreate vertex buffer failed");
            goto end;

            case createIndexBufferF:
            print("\ncreate index buffer failed");
            goto end;

            case createUniformBuffersF:
            print("\ncreate uniform buffers failed");
            goto end;

            case createDescriptorPoolF:
            print("\ncreate descriptor poo l failed");
            goto end;

            case createDescriptorSetsF:
            print("\ncreate descriptor sets failed");
            goto end;

            case createCommandbufferByBufferingF:
            print("\ncreate command buffer failed");
            goto end;

            case createSemaphoreF:
            print("\ncreate semaphore failed");
            goto end;

            case createFenceF:
            print("\ncreate fence failed");
            goto end;

            case recordCommandBufferF:
            print("\nrecord command buffer failed");
            goto end;

            case drawFrameF:
            print("\ndraw error");
            goto end;

            case waitForFencesF:
            print("\nwait for fences failed");
            goto end;

            case resetFencesF:
            print("\nreset fences failed");
            goto end;

            case acquireNextImageF:
            print("\nacquire next images failed");
            goto end;

            case resetCommandBufferF:
            print("\nreset command buffer failed");
            goto end;

            case endCommandBufferF:
            print("\nend command buffer failed");
            goto end;

            case queueSumbitF:
            print("\nqueue submit failed");
            goto end;

            case queuePresentF:
            print("\nqueue present failed");
            goto end;

            default:
            print("\nunprocessed value");
        }
    }

// Quick Quit Needed
    if (0)
    {
end:
        game_is_running = false;
    }
}
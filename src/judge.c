#include "core.h"
#include "judge.h"
#include "vulkan_code.h"

extern Recreate recreateSwap;

static bool initialize = false;

void resultVulkan(VkResult result, FuncCode code, uint32_t num, ...)
{
    bool clean = false;
    switch (result)
    {
        case VK_SUCCESS:
        if (code == initializedF)
            initialize = true;
        break;

        case VK_SUBOPTIMAL_KHR:
        if (code == queuePresentF)
        {
            recreateSwapchain(&recreateSwap);
            //printf("VK_SUBOPTIMAL_KHR(queuePresent)\n");
        }
        else if (code == acquireNextImageF)
        {
            printf("VK_SUBOPTIMAL_KHR(acquireNext)\n");
        }
        break;

        case VK_ERROR_OUT_OF_DATE_KHR:
        if (code == acquireNextImageF)
        {
            //printf("VK_ERROR_OUT_OF_DATE_KHR(acquireNExt)\n");
            recreateSwapchain(&recreateSwap);
        }
        if (code == queuePresentF)
        {
            recreateSwapchain(&recreateSwap);
            //printf("VK_ERROR_OUT_OF_DATE_KHR(queuePresent)\n");
        }
        break;

        case VK_ERROR_SURFACE_LOST_KHR:
        if (code == acquireNextImageF)
        {
            printf("VK_ERROR_SURFACE_LOST_KHR(acquireNExt)\n");
            recreateSwapchain(&recreateSwap);
        }
        if (code == queuePresentF)
        {
            recreateSwapchain(&recreateSwap);
            printf("VK_ERROR_SURFACE_LOST_KHR(queuePresent)\n");
        }
        break;

        default:
        printf("result: %d\n", result);
        clean = true;
        if (num > 0)
        {
            va_list ap;
            va_start(ap, num);
            for (uint32_t i = 0;i < num;i++)
            {
                free(va_arg(ap, void *));
                printf("freed(%u)\n", i);
            }
            va_end(ap);
        }
        switch(code)
        {
            case vulkanVersionF:
            fprintf(stderr, "\ncan't detect vulkan support\n");
            goto end;

            case createInstanceF:
            fprintf(stderr, "\ncreate instance error\n");
            goto end;

            case createSurfaceF:
            fprintf(stderr, "\ncreate surface error\n");
            goto end;

            case pickPhysicalDeviceF:
            fprintf(stderr, "\npick physical device error\n");
            goto end;

            case findQueueFamiliesF:
            fprintf(stderr, "\nget SurfaceSupport error\n");
            goto end;

            case checkDeviceExtensionSupportF:
            fprintf(stderr, "\nenumerate device extension properties error\n");
            goto end;

            case createLogicalDeviceF:
            fprintf(stderr, "\ncreate logical device failed\n");
            goto end;

            case getSurfaceFormatsF:
            fprintf(stderr, "\nget surface format failed\n");
            goto end;

            case getPresentModesF:
            fprintf(stderr, "\nget present modes failed\n");
            goto end;

            case getSurfaceCapabilitiesF:
            fprintf(stderr, "\nget surface capabilitues failed\n");
            goto end;

            case createSwapchainF:
            fprintf(stderr, "\ncreate swapchain failed\n");
            goto end;

            case getSwapchainNumberF:
            fprintf(stderr, "\nget swapchain number failed\n");
            goto end;

            case createSwapchainImageF:
            fprintf(stderr, "\ncreate swapchain image failed\n");
            goto end;

            case createSwapchainImageViewsF:
            fprintf(stderr, "\ncreate image view failed\n");
            goto end;

            case createShaderModuleF:
            fprintf(stderr, "\nshaderModule created failed\n");
            goto end;

            case createDescriptorSetLayoutF:
            fprintf(stderr, "\ncreate descriptoy set layout failed\n");
            goto end;

            case createPipelineLayoutF:
            fprintf(stderr, "\ncreate pipeline layout failed\n");
            goto end;

            case createRenderPassF:
            fprintf(stderr, "\ncreate render pass failed\n");
            goto end;

            case createGraphicsPipelineF:
            fprintf(stderr, "\ncreate graphic pipeline failed\n");
            goto end;

            case createCommandPoolF:
            fprintf(stderr, "\ncreate command pool failed\n");
            goto end;

            case createDepthResouresF:
            fprintf(stderr, "\ncreate depth resoures failed\n");
            goto end;

            case createFrameBufferF:
            fprintf(stderr, "\ncreate frame buffer failed\n");
            goto end;

            case createTextureImageF:
            fprintf(stderr, "\ncreate texture image failed\n");
            goto end;

            case createTextureImageViewF:
            fprintf(stderr, "\ncreate texture image view failed\n");
            goto end;

            case createTextureSamplerF:
            fprintf(stderr, "\ncreate texture sampler failed\n");
            goto end;

            case createVertexBufferF:
            fprintf(stderr, "\ncreate vertex buffer failed\n");
            goto end;

            case createIndexBufferF:
            fprintf(stderr, "\ncreate index buffer failed\n");
            goto end;

            case createUniformBuffersF:
            fprintf(stderr, "\ncreate uniform buffers failed\n");
            goto end;

            case createDescriptorPoolF:
            fprintf(stderr, "\ncreate descriptor poo l failed\n");
            goto end;

            case createDescriptorSetsF:
            fprintf(stderr, "\ncreate descriptor sets failed\n");
            goto end;

            case createCommandbufferF:
            fprintf(stderr, "\ncreate command buffer failed\n");
            goto end;

            case createSemaphoreF:
            fprintf(stderr, "\ncreate semaphore failed\n");
            goto end;

            case createFenceF:
            fprintf(stderr, "\ncreate fence failed\n");
            goto end;

            case initializeMovingBufferF:
            fprintf(stderr, "\ninitialize moving buffer failed\n");
            goto end;

            case recordCommandBufferF:
            fprintf(stderr, "\nrecord command buffer failed\n");
            goto end;

            case drawFrameF:
            fprintf(stderr, "\ndraw error\n");
            goto end;

            case waitForFencesF:
            fprintf(stderr, "\nwait for fences failed\n");
            goto end;

            case resetFencesF:
            fprintf(stderr, "\nreset fences failed\n");
            goto end;

            case acquireNextImageF:
            fprintf(stderr, "\nacquire next images failed\n");
            goto end;

            case resetCommandBufferF:
            fprintf(stderr, "\nreset command buffer failed\n");
            goto end;

            case endCommandBufferF:
            fprintf(stderr, "\nend command buffer failed\n");
            goto end;

            case queueSumbitF:
            fprintf(stderr, "\nqueue submit failed\n");
            goto end;

            case queuePresentF:
            fprintf(stderr, "\nqueue present failed\n");
            goto end;

            default:
            fprintf(stderr, "\nunprocessed value\n");
        }
    }

    end:
    if (clean)
    {
        cleanup(code);
        exit(code + 1000);
    }
}
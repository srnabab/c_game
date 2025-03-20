#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

#include "G_custom_math.h"
#include "G_log.h"

extern VK_ALL allInOne;

void recreateSwapchain(void)
{
    FuncCode code = recreateSwapchainF; 

    VkDevice * pDevice = allInOne.pDevice;

    VkFramebuffer * oldFrameBuffer = *allInOne.ppSwapchainFramebuffer;
    VkImageView * oldSwapchainImageView = *allInOne.ppSwapchainImageViews;
    Uint32 oldImageCount = *allInOne.pImageCount;
    VkSwapchainKHR oldSwapchain = *allInOne.pSwapchain;

    resultVulkan(vkDeviceWaitIdle(*pDevice), code, 0);

    unloadTexture("depth");
    
    getSurfaceCapabilities(allInOne.pSurfaceCapabilities);
    getSurfaceFormats(allInOne.pSurfaceFormat);

    createSwapchain(*allInOne.pSurface, *allInOne.pSurfaceCapabilities, *allInOne.pSurfaceFormat, *allInOne.pPresentMode, allInOne.pSwapchain, oldSwapchain);
    
    getSwapchainNumber();

    createSwapchainImage();
    createImageViews(allInOne.ppSwapchainImages, *allInOne.pImageCount, allInOne.pSurfaceFormat->format, VK_IMAGE_ASPECT_COLOR_BIT, allInOne.ppSwapchainImageViews);

    loadDepthResource("depth");
    createFrameBuffer(*allInOne.pImageCount, *allInOne.ppSwapchainImageViews, &getTexture("depth")->imageView, allInOne.pRenderPass, allInOne.ppSwapchainFramebuffer);

    destroyedFrameBuffer(oldImageCount, oldFrameBuffer);
    destroyImageViews(oldSwapchainImageView, oldImageCount);
    vkDestroySwapchainKHR(*pDevice, oldSwapchain, allInOne.pAllocationCallbacks);
    
    logMessage("recreate swapchain");
}
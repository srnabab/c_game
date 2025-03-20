#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

#include "G_log.h"

extern VK_ALL allInOne;

void recreateSwapchain(void)
{
    FuncCode code = recreateSwapchainF; 

    VkDevice * pDevice = allInOne.pDevice;

    VkFramebuffer * oldFrameBuffer = *allInOne.ppSwapchain2DFramebuffer;
    VkImageView * oldSwapchainImageView = *allInOne.ppSwapchain2DImageViews;
    Uint32 oldImageCount = *allInOne.pImageCount2D;
    VkSwapchainKHR oldSwapchain = *allInOne.pSwapchain2D;

    resultVulkan(vkDeviceWaitIdle(*pDevice), code, 0);

    unloadTexture("depth");
    
    getSurfaceCapabilities(*allInOne.pSurface2D, allInOne.pSurface2DCapabilities);
    getSurfaceFormats(*allInOne.pSurface2D, allInOne.pSurface2DFormat);

    createSwapchain(*allInOne.pSurface2D, *allInOne.pSurface2DCapabilities, *allInOne.pSurface2DFormat, *allInOne.pPresentMode2D, allInOne.pSwapchain2D, oldSwapchain);
    
    getSwapchainNumber(*allInOne.pSwapchain2D, allInOne.pImageCount2D);

    createSwapchainImage(*allInOne.pSwapchain2D, allInOne.pImageCount2D, allInOne.ppSwapchain2DImages);
    createImageViews(*allInOne.ppSwapchain2DImages, *allInOne.pImageCount2D, allInOne.pSurface2DFormat->format, VK_IMAGE_ASPECT_COLOR_BIT, allInOne.ppSwapchain2DImageViews);

    loadDepthResource("depth");
    createFrameBuffer(*allInOne.pImageCount2D, *allInOne.ppSwapchain2DImageViews, &getTexture("depth")->imageView, allInOne.pRenderPass, allInOne.ppSwapchain2DFramebuffer);

    destroyedFrameBuffer(oldImageCount, oldFrameBuffer);
    destroyImageViews(oldSwapchainImageView, oldImageCount);
    vkDestroySwapchainKHR(*pDevice, oldSwapchain, allInOne.pAllocationCallbacks);
    
    print("recreate swapchain");
}
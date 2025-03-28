#include "G_constants.h"

#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

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

    unloadTexture(TEXTURE_DEPTH);
    
    getSurfaceCapabilities(*allInOne.pSurface2D, allInOne.pSurface2DCapabilities);
    getSurfaceFormats(*allInOne.pSurface2D, allInOne.pSurface2DFormat);

    createSwapchain(*allInOne.pSurface2D, *allInOne.pSurface2DCapabilities, *allInOne.pSurface2DFormat, *allInOne.pPresentMode2D, allInOne.pSwapchain2D, oldSwapchain);
    
    getSwapchainNumber(*allInOne.pSwapchain2D, allInOne.pImageCount2D);

    createSwapchainImage(*allInOne.pSwapchain2D, allInOne.pImageCount2D, allInOne.ppSwapchain2DImages);
    createImageViews(*allInOne.ppSwapchain2DImages, *allInOne.pImageCount2D, allInOne.pSurface2DFormat->format, VK_IMAGE_ASPECT_COLOR_BIT, allInOne.ppSwapchain2DImageViews);

    loadDepthResource(TEXTURE_DEPTH);
    VkImageView graphicImageViews[1] = {getTexture(TEXTURE_DEPTH)->imageView};
    createFrameBuffer(*allInOne.pImageCount2D, 2, graphicImageViews, *allInOne.ppSwapchain2DImageViews, allInOne.pRenderPass, allInOne.ppSwapchain2DFramebuffer);

    destroyedFrameBuffer(oldImageCount, oldFrameBuffer);
    destroyImageViews(oldSwapchainImageView, oldImageCount);
    vkDestroySwapchainKHR(*pDevice, oldSwapchain, allInOne.pAllocationCallbacks);
 
#if WINDOW_3D_DEBUG
    VkFramebuffer * oldFrameBuffer3D = *allInOne.ppSwapchain3DFramebuffer;
    VkImageView * oldSwapchainImageView3D = *allInOne.ppSwapchain3DImageViews;
    Uint32 oldImageCount3D = *allInOne.pImageCount3D;
    VkSwapchainKHR oldSwapchain3D = *allInOne.pSwapchain3D;

    getSurfaceCapabilities(*allInOne.pSurface3D, allInOne.pSurface3DCapabilities);
    getSurfaceFormats(*allInOne.pSurface3D, allInOne.pSurface3DFormat);

    createSwapchain(*allInOne.pSurface3D, *allInOne.pSurface3DCapabilities, *allInOne.pSurface3DFormat, *allInOne.pPresentMode3D, allInOne.pSwapchain3D, oldSwapchain3D);
    
    getSwapchainNumber(*allInOne.pSwapchain3D, allInOne.pImageCount3D);

    createSwapchainImage(*allInOne.pSwapchain3D, allInOne.pImageCount3D, allInOne.ppSwapchain3DImages);
    createImageViews(*allInOne.ppSwapchain3DImages, *allInOne.pImageCount3D, allInOne.pSurface3DFormat->format, VK_IMAGE_ASPECT_COLOR_BIT, allInOne.ppSwapchain3DImageViews);
 
    loadDepthResource(TEXTURE_MODEL_DEPTH);
    loadNormalResource(TEXTURE_NORMAL);
    VkImageView modelImageViews[2] = {getTexture(TEXTURE_NORMAL)->imageView, getTexture(TEXTURE_MODEL_DEPTH)->imageView};
    createFrameBuffer(*allInOne.pImageCount3D, 3, modelImageViews, *allInOne.ppSwapchain3DImageViews, allInOne.pRenderPass, allInOne.ppSwapchain3DFramebuffer);

    destroyedFrameBuffer(oldImageCount3D, oldFrameBuffer3D);
    destroyImageViews(oldSwapchainImageView3D, oldImageCount3D);
    vkDestroySwapchainKHR(*pDevice, oldSwapchain3D, allInOne.pAllocationCallbacks);
#endif
 
    print("recreate swapchain");
}
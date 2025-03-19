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

static void newSwapchain(VkDevice * pDevice, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat, VkExtent2D * pExtent2D, VkPresentModeKHR * pPresentMode, QueueFamilyIndices indices, VkSwapchainKHR * pSwapchain)
{
    FuncCode code = createSwapchainF;
    uint32_t imageCount = pSurfaceCapabilities->minImageCount + 1;

    if (pSurfaceCapabilities->maxImageCount > 0 && imageCount > pSurfaceCapabilities->maxImageCount)
        imageCount = pSurfaceCapabilities->maxImageCount;
        
    if (*pSwapchain == NULL)
    {
        //printf("imageCount: %u\n", imageCount);
    }

    VkSwapchainKHR oldSwapchain = *pSwapchain;

    *pSwapchain = NULL;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.surface = *pSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = (*pSurfaceFormat).format;
    createInfo.imageColorSpace = (*pSurfaceFormat).colorSpace;
    createInfo.imageExtent = *pExtent2D;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = 0;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.preTransform = (*pSurfaceCapabilities).currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = *pPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.familyIndice, indices.presentFamily.familyIndice, indices.computeFamily.familyIndice, indices.transferFamily.familyIndice};
    if (indices.graphicsFamily.familyIndice != indices.presentFamily.familyIndice) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } 
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = NULL; // Optional
    }

    resultVulkan(vkCreateSwapchainKHR(*pDevice, &createInfo, allInOne.pAllocationCallbacks, pSwapchain), code, 0);
    
    vkDestroySwapchainKHR(*pDevice, oldSwapchain, allInOne.pAllocationCallbacks);
}
void recreateSwapchain(void)
{
    FuncCode code = recreateSwapchainF; 

    VkDevice * pDevice = allInOne.pDevice;

    VkFramebuffer * oldFrameBuffer = *allInOne.ppSwapchainFramebuffer;
    VkImageView * oldSwapchainImageView = *allInOne.ppSwapchainImageViews;
    Uint32 oldImageCount = *allInOne.pImageCount;

    resultVulkan(vkDeviceWaitIdle(*pDevice), code, 0);

    unloadTexture("depth");
    
    getSurfaceCapabilities(allInOne.pSurfaceCapabilities);
    getSurfaceFormats(allInOne.pSurfaceFormat);

    newSwapchain(pDevice, allInOne.pSurfaceCapabilities, allInOne.pSurface, allInOne.pSurfaceFormat, allInOne.pExtent2D, allInOne.pPresentMode, *allInOne.pQueueFamilyIndices, allInOne.pSwapchain);
    
    getSwapchainNumber();

    createSwapchainImage();
    createImageViews(allInOne.ppSwapchainImages, *allInOne.pImageCount, allInOne.pSurfaceFormat->format, VK_IMAGE_ASPECT_COLOR_BIT, allInOne.ppSwapchainImageViews);

    loadDepthResource("depth");
    createFrameBuffer(*allInOne.pImageCount, *allInOne.ppSwapchainImageViews, &getTexture("depth")->imageView, allInOne.pRenderPass, allInOne.ppSwapchainFramebuffer);

    destroyedFrameBuffer(oldImageCount, oldFrameBuffer);

    destroyImageViews(oldSwapchainImageView, oldImageCount);
    
    logMessage("recreate swapchain");
}
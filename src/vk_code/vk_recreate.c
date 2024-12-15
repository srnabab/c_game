#include "vk_recreate.h"
#include "vk_swapchain.h"
#include "vk_image.h"
#include "vk_framebuffer.h"
#include "vk_depth.h"
#include "custom_math.h"
#include "vk_move.h"

static void newSwapchain(VkDevice * pDevice, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat, VkExtent2D * pExtent2D, VkPresentModeKHR * pPresentMode, QueueFamilyIndices indices, VkSwapchainKHR * pSwapchain)
{
    // FuncCode code = createSwapchainF;
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

    uint32_t queueFamilyIndices[3] = {indices.graphicsFamily.familyIndice, indices.presentFamily.familyIndice, indices.computeFamily.familyIndice};
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

    //resultVulkan(
        vkCreateSwapchainKHR(*pDevice, &createInfo, NULL, pSwapchain);//, code, 0);
    
    vkDestroySwapchainKHR(*pDevice, oldSwapchain, NULL);
}
void recreateSwapchain(Recreate * pRecreate)
{
    FuncCode code = recreateSwapchainF; 
    VkDevice * pDevice = pRecreate->DevicePack.pDevice;
    VkPhysicalDevice * pPhysicalDevice = pRecreate->DevicePack.pPhysicalDevice;
    //printf("wait\n");
    //printf("pDevice: %p\n", pRecreate->pDevice);
    resultVulkan(vkDeviceWaitIdle(*pDevice), code, 0);
    //printf("wait done\n");
    
    SDL_Log("ppSwaapchainFramebuffer: %p\n", pRecreate->ppSwapchainFramebuffer);
    destroyedFrameBuffer(pDevice, *pRecreate->imageCount, *pRecreate->ppSwapchainFramebuffer);
    SDL_free(*pRecreate->ppSwapchainFramebuffer);
    *pRecreate->ppSwapchainFramebuffer = NULL;

    destroyImageViews(pDevice, *pRecreate->ppSwapchainImageViews, *pRecreate->imageCount);
    SDL_free(*pRecreate->ppSwapchainImageViews);
    *pRecreate->ppSwapchainImageViews = NULL;
    SDL_free(*pRecreate->ppSwapchainImages);
    *pRecreate->ppSwapchainImages = NULL;

    vkDestroyImageView(*pDevice, *pRecreate->pDepthImageView, NULL);
    *pRecreate->pDepthImageView = NULL;
    vkDestroyImage(*pDevice, *pRecreate->pDepthImage, NULL);
    *pRecreate->pDepthImage = NULL;
    vkFreeMemory(*pDevice, *pRecreate->pDepthImageMem, NULL);
    *pRecreate->pDepthImageMem = NULL;
    
    //cleanupSwapchain(pRecreate->pDevice, *pRecreate->ImageCount, *pRecreate->pSwapchainFramebuffer, *pRecreate->pSwapchainImageViews, pRecreate->pSwapchain);
    //printf("imageCOunt: %u\n", *pRecreate->ImageCount);

    getSurfaceCapabilities(pPhysicalDevice, pRecreate->pSurface, pRecreate->pSurfaceCapabilities);
    getSurfaceFormats(pPhysicalDevice, pRecreate->pSurface, pRecreate->pSurfaceFormat);
    //chooseSwapExtent(pPhysicalDevice, pRecreate->pSurface, pRecreate->pSurfaceCapabilities, pRecreate->pExtent2D);

    newSwapchain(pDevice, pRecreate->pSurfaceCapabilities, pRecreate->pSurface, pRecreate->pSurfaceFormat, pRecreate->pExtent2D, pRecreate->pPresentMode, *pRecreate->pIndices, pRecreate->pSwapchain);
    
    getSwapchainNumber(pDevice, pRecreate->pSwapchain, pRecreate->imageCount);
    //printf("imageCOunt: %u\n", *pRecreate->ImageCount);

    createSwapchainImage(pDevice, pRecreate->pSwapchain, *pRecreate->imageCount, pRecreate->ppSwapchainImages);
    createImageViews(pDevice, pRecreate->ppSwapchainImages, *pRecreate->imageCount, pRecreate->swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, pRecreate->ppSwapchainImageViews);
    createDepthResoures(pPhysicalDevice, pDevice, pRecreate->pExtent2D, pRecreate->pSwapchainCommandPool, pRecreate->pGraphicQueue, pRecreate->pDepthImage, pRecreate->pDepthImageMem, pRecreate->pDepthImageView);
    SDL_Log("depth resoures created\n");
    createFrameBuffer(pDevice, pRecreate->pExtent2D, *pRecreate->imageCount, *pRecreate->ppSwapchainImageViews, pRecreate->pDepthImageView, pRecreate->pRenderPass, pRecreate->ppSwapchainFramebuffer);
    SDL_Log("swapchain framebuffer created\n");

    //fix_ratio(pRecreate->ppVertices, *pRecreate->pOldExtent2D, *pRecreate->pExtent2D, 0);
    //fix_ratio(pRecreate->ppVertices, *pRecreate->pOldExtent2D, *pRecreate->pExtent2D, 1);
    //fix_ratio(pR)

    //reInitializePosition(64, 64, *pRecreate->pExtent2D, pRecreate->ppVertices, 0);
    
    SDL_Log("recreate swapchain\n");
}
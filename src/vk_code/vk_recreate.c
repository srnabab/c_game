#include "vk_recreate.h"
#include "vk_swapchain.h"
#include "vk_image.h"
#include "vk_framebuffer.h"
#include "vk_depth.h"

void recreateSwapchain(Recreate * pRecreate)
{
    FuncCode code = recreateSwapchainF; 
    VkDevice * pDevice = pRecreate->DevicePack.pDevice;
    VkPhysicalDevice * pPhysicalDevice = pRecreate->DevicePack.pPhysicalDevice;
    //printf("wait\n");
    //printf("pDevice: %p\n", pRecreate->pDevice);
    resultVulkan(vkDeviceWaitIdle(*pDevice), code, 0);
    //printf("wait done\n");
    
    printf("ppSwaapchainFramebuffer: %p\n", pRecreate->ppSwapchainFramebuffer);
    destroyedFrameBuffer(pDevice, *pRecreate->imageCount, *pRecreate->ppSwapchainFramebuffer);
    free(*pRecreate->ppSwapchainFramebuffer);
    *pRecreate->ppSwapchainFramebuffer = VK_NULL_HANDLE;

    destroyImageViews(pDevice, *pRecreate->ppSwapchainImageViews, *pRecreate->imageCount);
    free(*pRecreate->ppSwapchainImageViews);
    *pRecreate->ppSwapchainImageViews = VK_NULL_HANDLE;
    free(*pRecreate->ppSwapchainImages);
    *pRecreate->ppSwapchainImages = VK_NULL_HANDLE;

    vkDestroyImageView(*pDevice, *pRecreate->pDepthImageView, VK_NULL_HANDLE);
    *pRecreate->pDepthImageView = VK_NULL_HANDLE;
    vkDestroyImage(*pDevice, *pRecreate->pDepthImage, VK_NULL_HANDLE);
    *pRecreate->pDepthImage = VK_NULL_HANDLE;
    vkFreeMemory(*pDevice, *pRecreate->pDepthImageMem, VK_NULL_HANDLE);
    *pRecreate->pDepthImageMem = VK_NULL_HANDLE;
    
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
    printf("depth resoures created\n");
    createFrameBuffer(pDevice, pRecreate->pExtent2D, *pRecreate->imageCount, *pRecreate->ppSwapchainImageViews, pRecreate->pDepthImageView, pRecreate->pRenderPass, pRecreate->ppSwapchainFramebuffer);
    printf("swapchain framebuffer created\n");
    
    printf("recreate swapchain\n");
}
static void newSwapchain(VkDevice * pDevice, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat, VkExtent2D * pExtent2D, VkPresentModeKHR * pPresentMode, QueueFamilyIndices indices, VkSwapchainKHR * pSwapchain)
{
    FuncCode code = createSwapchainF;
    uint32_t imageCount = pSurfaceCapabilities->minImageCount + 1;

    if (pSurfaceCapabilities->maxImageCount > 0 && imageCount > pSurfaceCapabilities->maxImageCount)
        imageCount = pSurfaceCapabilities->maxImageCount;
        
    if (*pSwapchain == VK_NULL_HANDLE)
        //printf("imageCount: %u\n", imageCount);
        ;

    VkSwapchainKHR oldSwapchain = *pSwapchain;

    *pSwapchain = VK_NULL_HANDLE;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = VK_NULL_HANDLE;
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
    createInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
    createInfo.preTransform = (*pSurfaceCapabilities).currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = *pPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    uint32_t queueFamilyIndices[2] = {indices.graphicsFamily, indices.presentFamily};
    if (indices.graphicsFamily != indices.presentFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } 
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = VK_NULL_HANDLE; // Optional
    }

    //resultVulkan(
        vkCreateSwapchainKHR(*pDevice, &createInfo, VK_NULL_HANDLE, pSwapchain);//, code, 0);
    
    vkDestroySwapchainKHR(*pDevice, oldSwapchain, VK_NULL_HANDLE);
}
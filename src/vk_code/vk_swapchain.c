#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_judge.h"

#include "SDL3/SDL_vulkan.h"

extern VK_ALL allInOne;

void getSurfaceFormats(void)
{
    FuncCode code = getSurfaceFormatsF;
    uint32_t surfaceFormatCount = 0;
    resultVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(*allInOne.pPhysicalDevice, *allInOne.pSurface, &surfaceFormatCount, NULL), code, 0);
    
    VkSurfaceFormatKHR * surfaceFormat = (VkSurfaceFormatKHR *)SDL_malloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    if (surfaceFormatCount != 0)
    {
        resultVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(*allInOne.pPhysicalDevice, *allInOne.pSurface, &surfaceFormatCount, surfaceFormat), code, 1, surfaceFormat);
    }

    bool selected = false;
    for (uint32_t i = 0;i < surfaceFormatCount;i++)
    {
        //printf("format: %u\n", surfaceFormat[i]);
        if (surfaceFormat[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && surfaceFormat[i].format == VK_FORMAT_R8G8B8A8_SRGB)
        {
            *allInOne.pSurfaceFormat = surfaceFormat[i];
            selected = true;
        }
    }

    if (!selected)
        *allInOne.pSurfaceFormat = surfaceFormat[0];

    SDL_free(surfaceFormat);
}
void getPresentModes(void)
{
    FuncCode code = getPresentModesF;
    uint32_t presentModeCount;
    resultVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(*allInOne.pPhysicalDevice, *allInOne.pSurface, &presentModeCount, NULL), code, 0);

    VkPresentModeKHR * presentModes = (VkPresentModeKHR *)SDL_malloc(presentModeCount * sizeof(VkPresentModeKHR));
    if (presentModeCount != 0) 
    {
        resultVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(*allInOne.pPhysicalDevice, *allInOne.pSurface, &presentModeCount, presentModes), code, 1, presentModes);
    
        for (uint32_t i = 0;i < presentModeCount;i++)
        {
            //printf("present mode%u\n", presentModes[i]);
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                *allInOne.pPresentMode = presentModes[i];
        }
    }
    else
    {
        *allInOne.pPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    SDL_free(presentModes);
}
void getSurfaceCapabilities(void)
{
    FuncCode code = getSurfaceCapabilitiesF;
    resultVulkan(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*allInOne.pPhysicalDevice, *allInOne.pSurface, allInOne.pSurfaceCapabilities), code, 0);
}
void createSwapchain(void)
{
    FuncCode code = createSwapchainF;
    uint32_t imageCount = allInOne.pSurfaceCapabilities->minImageCount + 1;

    if (allInOne.pSurfaceCapabilities->maxImageCount > 0 && imageCount > allInOne.pSurfaceCapabilities->maxImageCount)
        imageCount = allInOne.pSurfaceCapabilities->maxImageCount;
        
    if (*allInOne.pSwapchain == NULL)
    {
        //printf("imageCount: %u\n", imageCount);
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.surface = *allInOne.pSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = allInOne.pSurfaceFormat->format;
    createInfo.imageColorSpace = allInOne.pSurfaceFormat->colorSpace;
    createInfo.imageExtent = *allInOne.pExtent2D;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    createInfo.imageSharingMode = 0;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.preTransform = (*allInOne.pSurfaceCapabilities).currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = *allInOne.pPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = NULL;

    uint32_t queueFamilyIndices[3] = {allInOne.pQueueFamilyIndices->graphicsFamily.familyIndice, allInOne.pQueueFamilyIndices->presentFamily.familyIndice, allInOne.pQueueFamilyIndices->computeFamily.familyIndice};
    if (allInOne.pQueueFamilyIndices->graphicsFamily.familyIndice != allInOne.pQueueFamilyIndices->presentFamily.familyIndice) 
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

    resultVulkan(vkCreateSwapchainKHR(*allInOne.pDevice, &createInfo, allInOne.pAllocationCallbacks, allInOne.pSwapchain), code, 0);
}
void getSwapchainNumber(void)
{
    FuncCode code = getSwapchainNumberF;
    resultVulkan(vkGetSwapchainImagesKHR(*allInOne.pDevice, *allInOne.pSwapchain, allInOne.pImageCount, NULL), code, 0);
}
void createSwapchainImage(void)
{
    FuncCode code = createSwapchainImageF;
    if (*allInOne.ppSwapchainImages != NULL)
    {
        SDL_free(*allInOne.ppSwapchainImages);
        *allInOne.ppSwapchainImages = NULL;
    }
    *allInOne.ppSwapchainImages = (VkImage *)SDL_malloc(*allInOne.pImageCount * sizeof(VkImage));
    resultVulkan(vkGetSwapchainImagesKHR(*allInOne.pDevice, *allInOne.pSwapchain, allInOne.pImageCount, *allInOne.ppSwapchainImages), code, 0);
}
void createSwapchainImageView(VkImageAspectFlags aspectFlags)
{
    FuncCode code = createSwapchainImageViewsF;

    resultVulkan(createImageViews(allInOne.pDevice, allInOne.ppSwapchainImages, *allInOne.pImageCount, allInOne.pSurfaceFormat->format, aspectFlags, allInOne.ppSwapchainImageViews), code, 0);   
}
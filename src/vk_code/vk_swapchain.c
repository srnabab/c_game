#include "vk_swapchain.h"
#include "vk_image.h"
#include "SDL3/SDL_vulkan.h"
#include "vk_code_h/vk_judge.h"

extern SDL_Window * window;

void getSurfaceFormats(VkPhysicalDevice * pDevice, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat)
{
    FuncCode code = getSurfaceFormatsF;
    uint32_t surfaceFormatCount = 0;
    resultVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *pSurface, &surfaceFormatCount, NULL), code, 0);
    if (pSurfaceFormat == NULL)
    {
        //printf("surfaceFormatsCount: %u\n", surfaceFormatCount);
    }

    VkSurfaceFormatKHR * surfaceFormat = (VkSurfaceFormatKHR *)SDL_malloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    if (surfaceFormatCount != 0)
    {
        resultVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *pSurface, &surfaceFormatCount, surfaceFormat), code, 1, surfaceFormat);
    }

    bool selected = false;
    for (uint32_t i = 0;i < surfaceFormatCount;i++)
    {
        //printf("format: %u\n", surfaceFormat[i]);
        if (surfaceFormat[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && surfaceFormat[i].format == VK_FORMAT_R8G8B8A8_SRGB)
        {
            *pSurfaceFormat = surfaceFormat[i];
            selected = true;
        }
    }

    if (!selected)
        *pSurfaceFormat = surfaceFormat[0];

    SDL_free(surfaceFormat);
}
void getPresentModes(VkPhysicalDevice * pDevice, VkSurfaceKHR * pSurface, VkPresentModeKHR * pPresentMode)
{
    FuncCode code = getPresentModesF;
    uint32_t presentModeCount;
    resultVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *pSurface, &presentModeCount, NULL), code, 0);
    if (pPresentMode == NULL)
    {
        //printf("presentModeCount: %u\n ", presentModeCount);
    }

    VkPresentModeKHR * presentModes = (VkPresentModeKHR *)SDL_malloc(presentModeCount * sizeof(VkPresentModeKHR));
    if (presentModeCount != 0) 
    {
        resultVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *pSurface, &presentModeCount, presentModes), code, 1, presentModes);
    
        for (uint32_t i = 0;i < presentModeCount;i++)
        {
            //printf("present mode%u\n", presentModes[i]);
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                *pPresentMode = presentModes[i];
        }
    }
    else
    {
        *pPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    SDL_free(presentModes);
}
void getSurfaceCapabilities(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities)
{
    FuncCode code = getSurfaceCapabilitiesF;
    resultVulkan(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pPhysicalDevice, *pSurface, pSurfaceCapabilities), code, 0);
}
/*void chooseSwapExtent(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkExtent2D * pExtent2D)
{
    //int FramebufferWidth = 0, FramebufferHeight = 0;
	//SDL_VULKAN_(window, &FramebufferWidth, &FramebufferHeight);
    //printf("weight: %u\nheight: %u\n", FramebufferWidth, FramebufferHeight);

	if(pSurfaceCapabilities->currentExtent.width < FramebufferWidth){
		pExtent2D->width = pSurfaceCapabilities->currentExtent.width;
	}else{
		pExtent2D->width = FramebufferWidth;
	}

	if(pSurfaceCapabilities->currentExtent.height < FramebufferHeight){
		pExtent2D->height = pSurfaceCapabilities->currentExtent.height;
	}else{
		pExtent2D->height = FramebufferHeight;
	}
}*/
void createSwapchain(VkDevice * pDevice, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat, VkPresentModeKHR * pPresentMode, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkExtent2D * pExent2D, QueueFamilyIndices indices, VkSwapchainKHR * pSwapchain)
{
    FuncCode code = createSwapchainF;
    uint32_t imageCount = pSurfaceCapabilities->minImageCount + 1;

    if (pSurfaceCapabilities->maxImageCount > 0 && imageCount > pSurfaceCapabilities->maxImageCount)
        imageCount = pSurfaceCapabilities->maxImageCount;
        
    if (*pSwapchain == NULL)
    {
        //printf("imageCount: %u\n", imageCount);
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.surface = *pSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = (*pSurfaceFormat).format;
    createInfo.imageColorSpace = (*pSurfaceFormat).colorSpace;
    createInfo.imageExtent = *pExent2D;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    createInfo.imageSharingMode = 0;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.preTransform = (*pSurfaceCapabilities).currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = *pPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = NULL;

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

    resultVulkan(vkCreateSwapchainKHR(*pDevice, &createInfo, NULL, pSwapchain), code, 0);
}
void getSwapchainNumber(VkDevice * pDevice, VkSwapchainKHR * pSwapchain, uint32_t * pImageCount)
{
    FuncCode code = getSwapchainNumberF;
    resultVulkan(vkGetSwapchainImagesKHR(*pDevice, *pSwapchain, pImageCount, NULL), code, 0);
}
void createSwapchainImage(VkDevice * pDevice, VkSwapchainKHR * pSwapchain, uint32_t imageCount, VkImage ** pSwapchainImages)
{
    FuncCode code = createSwapchainImageF;
    if (*pSwapchainImages != NULL)
    {
        SDL_free(*pSwapchainImages);
        *pSwapchainImages = NULL;
    }
    *pSwapchainImages = (VkImage *)SDL_malloc(imageCount * sizeof(VkImage));
    resultVulkan(vkGetSwapchainImagesKHR(*pDevice, *pSwapchain, &imageCount, *pSwapchainImages), code, 0);
}
void createSwapchainImageView(VkDevice * pDevice, VkImage ** ppImages, uint32_t imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView)
{
    FuncCode code = createSwapchainImageViewsF;

    resultVulkan(createImageViews(pDevice, ppImages, imageCount, format, aspectFlags, ppImageView), code, 0);   
}
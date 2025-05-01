#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "SDL3/SDL_vulkan.h"

extern VK_ALL allInOne;

void getSurfaceFormats(VkSurfaceKHR surface, VkSurfaceFormatKHR * pSurfaceFormat)
{
    uint32_t surfaceFormatCount = 0;
    resultVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(allInOne.physicalDevice, surface, &surfaceFormatCount, NULL), 0);
    
    VkSurfaceFormatKHR * surfaceFormat = (VkSurfaceFormatKHR *)SDL_malloc(surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    if (surfaceFormatCount != 0)
    {
        resultVulkan(vkGetPhysicalDeviceSurfaceFormatsKHR(allInOne.physicalDevice, surface, &surfaceFormatCount, surfaceFormat), 1, surfaceFormat);
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
void getPresentModes(VkPresentModeKHR * pPresentMode)
{
    uint32_t presentModeCount;
    resultVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(allInOne.physicalDevice, allInOne.surface3D, &presentModeCount, NULL), 0);

    VkPresentModeKHR * presentModes = (VkPresentModeKHR *)SDL_malloc(presentModeCount * sizeof(VkPresentModeKHR));
    if (presentModeCount != 0) 
    {
        resultVulkan(vkGetPhysicalDeviceSurfacePresentModesKHR(allInOne.physicalDevice, allInOne.surface3D, &presentModeCount, presentModes), 1, presentModes);
    
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

    *pPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    SDL_free(presentModes);
}
void getSurfaceCapabilities(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities)
{
    resultVulkan(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(allInOne.physicalDevice, surface, pSurfaceCapabilities), 0);
}
void createSwapchain(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR surfaceCapabilities, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkSwapchainKHR * pSwapchain, VkSwapchainKHR oldSwapchain)
{
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        imageCount = surfaceCapabilities.maxImageCount;
        
    if (allInOne.swapchain3D == NULL)
    {
        //printf("imageCount: %u\n", imageCount);
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = allInOne.extent2D;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    createInfo.imageSharingMode = 0;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = NULL; // Optional

    resultVulkan(vkCreateSwapchainKHR(allInOne.device, &createInfo, allInOne.pAllocationCallbacks, pSwapchain), 0);
}
void getSwapchainNumber(VkSwapchainKHR swapchain, Uint32 * pImageCount)
{
    resultVulkan(vkGetSwapchainImagesKHR(allInOne.device, swapchain, pImageCount, NULL), 0);
}
void createSwapchainImage(VkSwapchainKHR swapchain, Uint32 * pImageCount, VkImage ** ppSwapchainImages)
{
    *ppSwapchainImages = (VkImage *)SDL_malloc(*pImageCount * sizeof(VkImage));
    resultVulkan(vkGetSwapchainImagesKHR(allInOne.device, swapchain, pImageCount, *ppSwapchainImages), 0);
}
void createSwapchainImageView(VkImage * pSwapchainImages, Uint32 imageCount, VkFormat swapchainFormat, VkImageAspectFlags aspectFlags, VkImageView ** ppSwapchainImageViews)
{
    resultVulkan(createImageViews(pSwapchainImages, imageCount, swapchainFormat, aspectFlags, ppSwapchainImageViews), 0);   
}
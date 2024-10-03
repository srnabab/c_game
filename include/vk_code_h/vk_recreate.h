#include "vk_all.h"

#ifndef VK_RECREATE_H
#define VK_RECREATE_H

void recreateSwapchain(Recreate * pAllInOne);
static void newSwapchain(VkDevice * pDevice, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat, VkExtent2D * pExtent2D, VkPresentModeKHR * pPresentMode, QueueFamilyIndices indices, VkSwapchainKHR * pSwapchain);


#endif
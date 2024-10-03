#include "vk_all.h"
#include "vk_queue.h"

#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H

void getSurfaceFormats(VkPhysicalDevice * pDevice, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat);
void getPresentModes(VkPhysicalDevice * pDevice, VkSurfaceKHR * pSurface, VkPresentModeKHR * pPresentMode);
void getSurfaceCapabilities(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities);
void chooseSwapExtent(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkExtent2D * pExtent2D);
void createSwapchain(VkDevice * pDevice, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat, VkPresentModeKHR * pPresentMode, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkExtent2D * pExent2D, QueueFamilyIndices indices, VkSwapchainKHR * pSwapchain);
void getSwapchainNumber(VkDevice * pDevice, VkSwapchainKHR * pSwapchain, uint32_t * pImageCount);
void createSwapchainImage(VkDevice * pDevice, VkSwapchainKHR * pSwapchain, uint32_t imageCount, VkImage ** pSwapchainImages);
void createSwapchainImageView(VkDevice * pDevice, VkImage ** ppImages, uint32_t imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView);

#endif // vk_swapchain.h
#include "vk_struct.h"

#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL getSurfaceFormats(VkPhysicalDevice * pDevice, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat);
extern void SDLCALL getPresentModes(VkPhysicalDevice * pDevice, VkSurfaceKHR * pSurface, VkPresentModeKHR * pPresentMode);
extern void SDLCALL getSurfaceCapabilities(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities);
// extern void SDLCALL chooseSwapExtent(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkExtent2D * pExtent2D);
extern void SDLCALL createSwapchain(VkDevice * pDevice, VkSurfaceKHR * pSurface, VkSurfaceFormatKHR * pSurfaceFormat, VkPresentModeKHR * pPresentMode, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkExtent2D * pExent2D, QueueFamilyIndices indices, VkSwapchainKHR * pSwapchain);
extern void SDLCALL getSwapchainNumber(VkDevice * pDevice, VkSwapchainKHR * pSwapchain, uint32_t * pImageCount);
extern void SDLCALL createSwapchainImage(VkDevice * pDevice, VkSwapchainKHR * pSwapchain, uint32_t imageCount, VkImage ** pSwapchainImages);
extern void SDLCALL createSwapchainImageView(VkDevice * pDevice, VkImage ** ppImages, uint32_t imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView);

#include "SDL3/SDL_close_code.h"

#endif // vk_swapchain.h
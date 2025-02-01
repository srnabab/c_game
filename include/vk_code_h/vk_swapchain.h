#include "vk_struct.h"

#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL getSurfaceFormats(void);
extern void SDLCALL getPresentModes(void);
extern void SDLCALL getSurfaceCapabilities(void);
// extern void SDLCALL chooseSwapExtent(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, VkSurfaceCapabilitiesKHR * pSurfaceCapabilities, VkExtent2D * pExtent2D);
extern void SDLCALL createSwapchain(void);
extern void SDLCALL getSwapchainNumber(void);
extern void SDLCALL createSwapchainImage(void);
extern void SDLCALL createSwapchainImageView(VkImageAspectFlags aspectFlags);

#include "SDL3/SDL_close_code.h"

#endif // vk_swapchain.h
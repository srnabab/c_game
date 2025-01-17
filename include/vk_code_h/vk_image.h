#include "vulkan/vulkan.h"
#include "SDL3/SDL_stdinc.h"

#ifndef VK_IMAGE_H
#define VK_IMAGE_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult SDLCALL createImage(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage * pImage, VkDeviceMemory * pImageMem);
extern VkResult SDLCALL createImageView(VkDevice * pDevice, VkImage * pImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pImageView);
extern VkResult SDLCALL createImageViews(VkDevice * pDevice, VkImage ** ppImages, uint32_t imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView);
extern void SDLCALL destroyImageViews(VkDevice * pDevice, VkImageView * pImageView, uint32_t imageCount);
extern VkResult SDLCALL transitionImageLayout(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphcisQueue, VkImage * pImage, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
extern bool SDLCALL hasStencilComponent(VkFormat format);

#include "SDL3/SDL_close_code.h"

#endif //vk_image.h
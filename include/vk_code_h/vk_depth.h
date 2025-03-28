#include "vulkan/vulkan.h"

#ifndef VK_DEPTH_H
#define VK_DEPTH_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL findDepthFormat(VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pDepthFormat);
extern VkFormat SDLCALL createDepthResoures(VkImage * pDepthImage, VkDeviceMemory * pDepthImageMem, VkImageView * pDepthImageView);

#include "SDL3/SDL_close_code.h"

#endif
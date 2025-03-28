#include "vulkan/vulkan.h"

#ifndef VK_NORMAL_H
#define VK_NORMAL_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL findNormalFormat(VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pNormalFormat);
extern VkFormat SDLCALL createNormalResoures(VkImage * pNormalImage, VkDeviceMemory * pNormalImageMem, VkImageView * pNormalImageView);

#include "SDL3/SDL_close_code.h"

#endif //vk_normal.h
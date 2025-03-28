#include "vulkan/vulkan.h"

#ifndef VK_FORMAT_H
#define VK_FORMAT_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL findSupportFormat(VkFormat * candiates, VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat);

#include "SDL3/SDL_close_code.h"

#endif //vk_format.h
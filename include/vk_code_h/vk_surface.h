#include "G_begin_code.h"
#include "vulkan/vulkan.h"
#include "SDL3/SDL_video.h"

#ifndef VK_SURFACE_H
#define VK_SURFACE_H 1

void createSurface(SDL_Window * pWindow, VkInstance * pInstance, VkSurfaceKHR * pSurface);

#endif //vk_surface.h
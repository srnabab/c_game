#include "vulkan/vulkan.h"
#include "SDL3/SDL_video.h"

#ifndef VK_SURFACE_H
#define VK_SURFACE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createSurface(SDL_Window * pWindow, VkSurfaceKHR * pSurface);

#include "SDL3/SDL_close_code.h"

#endif //vk_surface.h
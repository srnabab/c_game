#include "core.h"
#include "SDL3/SDL.h"

#ifndef VK_SURFACE_H
#define VK_SURFACE_H

void createSurface(SDL_Window * pWindow, VkInstance * pInstance, VkSurfaceKHR * pSurface);

#endif //vk_surface.h
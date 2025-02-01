#include "vk_struct.h"

#ifndef VK_DEVICE_H
#define VK_DEVICE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL pickPhysicalDevice(void);
extern void SDLCALL createLogicalDevice(void);

#include "SDL3/SDL_close_code.h"

#endif
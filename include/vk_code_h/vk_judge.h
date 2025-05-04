#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_JUDGE_H
#define VK_JUDGE_H 1

#include "SDL3/SDL_begin_code.h"

//judge VK_RESULT for vulkan
extern void SDLCALL resultVulkan(VkResult result, Uint32 num, ...);

#include "SDL3/SDL_close_code.h"

#endif //clean.h
#include "vulkan/vulkan.h"

#ifndef VK_SAMPLER_H
#define VK_SAMPLER_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createTextureSampler(VkSampler * pSampler);
extern void SDLCALL createNormalSampler(VkSampler * pSampler);
extern void SDLCALL createDepthSampler(VkSampler * pSampler);
extern void SDLCALL createShadowSampler(VkSampler * pSampler);

#include "SDL3/SDL_close_code.h"

#endif // vk_sampler.h
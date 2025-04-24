#include "vk_struct.h"

#include "G_scene.h"

#ifndef VK_PRESENT_H
#define VK_PRESENT_H 1

#include "SDL3/SDL_begin_code.h"

// extern void SDLCALL updateUniformBuffer(uint32_t currentImage, VkExtent2D * pExtent2D, UniformBufferObject * pUbo, void *** pUniformBuffersMapped, float camera_X, float camera_Y, ComputeUniformBufferObject * pComputeUbo, void *** pppComputeUniformBufferMapped, float deltaTime);
extern void SDLCALL drawFrame(Scene scene, Uint32 currentFrame, Uint32 width, Uint32 heigh);

#include "SDL3/SDL_close_code.h"

#endif
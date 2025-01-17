#include "vk_struct.h"

#ifndef VK_PRESENT_H
#define VK_PRESENT_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL recordCommandBuffer(VK_ALL * pAllInOne, uint32_t imageIndex);
extern void SDLCALL recordComputeCommandBuffer(VK_ALL * pAllInOne);
// extern void SDLCALL updateUniformBuffer(uint32_t currentImage, VkExtent2D * pExtent2D, UniformBufferObject * pUbo, void *** pUniformBuffersMapped, float camera_X, float camera_Y, ComputeUniformBufferObject * pComputeUbo, void *** pppComputeUniformBufferMapped, float deltaTime);
extern void SDLCALL drawFrame(VK_ALL * pAllInOne);
extern void SDLCALL updatePosition(float x, float y, VkExtent2D * pExtent2D, Vertex ** ppVertices, uint32_t pictureSequence);

#include "SDL3/SDL_close_code.h"

#endif
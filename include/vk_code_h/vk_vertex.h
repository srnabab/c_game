#include "vk_struct.h"

#ifndef VK_VERTEX_H
#define VK_VERTEX_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createVertexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, void ** ppVertexBufferMemMapped, Uint32 bufferSize);

#include "SDL3/SDL_close_code.h"

#endif
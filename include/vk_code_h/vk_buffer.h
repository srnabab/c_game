#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult SDLCALL createBuffer(VkBuffer * pBuffer, VkDeviceMemory * pBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
extern VkResult SDLCALL beginSingleTimeCommands(VkCommandPool * pCommandPool, VkCommandBuffer * pCommandBuffer);
extern VkResult SDLCALL endSingleTimeCommands(VkCommandPool * pCommandPool, VkQueue * pQueue, VkCommandBuffer * pCommandBuffer);
extern VkResult SDLCALL copyBuffer(VkBuffer * pSrcBuffer, VkBuffer * pDstBuffer, VkDeviceSize size);
extern int SDLCALL findMemoryType(Uint32 typeFilter, VkMemoryPropertyFlags properties);
extern void SDLCALL destroyBufferByBuffering(VkBuffer pBuffers[2], VkDeviceMemory pBuffersMem[2]);
extern void SDLCALL destroyBuffer(VkBuffer pBuffer, VkDeviceMemory pBufferMem);

#include "SDL3/SDL_close_code.h"

#endif
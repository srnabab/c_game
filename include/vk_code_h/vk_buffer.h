#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult SDLCALL createBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pBuffer, VkDeviceMemory * pBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
extern VkResult SDLCALL beginSingleTimeCommands(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer * pCommandBuffer);
extern VkResult SDLCALL endSingleTimeCommands(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicsQueue, VkCommandBuffer * pCommandBuffer);
extern VkResult SDLCALL copyBuffer(VkBuffer * pSrcBuffer, VkBuffer * pDstBuffer, VkDeviceSize size, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue);
extern int SDLCALL findMemoryType(VkPhysicalDevice * pPhysicalDevice, Uint32 typeFilter, VkMemoryPropertyFlags properties);

#include "SDL3/SDL_close_code.h"

#endif
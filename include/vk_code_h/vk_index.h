#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_INDEX_H
#define VK_INDEX_H 1

#include "SDL3/SDL_begin_code.h"

extern void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, void ** pIndexBufferMemMapped, void * indices, Uint32 indicesCount, size_t indexSize, bool staging);

#include "SDL3/SDL_close_code.h"

#endif
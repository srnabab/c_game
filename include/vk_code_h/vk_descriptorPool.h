#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_DESCRIPTORPOOL_H
#define VK_DESCRIPTORPOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createDescriptorPool(VkDevice * pDevice, Uint32 poolSizeCount, VkDescriptorPoolSize * pPoolSizes, Uint32 maxSets, VkDescriptorPool * pDescriptorPool);
extern void SDLCALL createDescriptorSets(VkDescriptorPool * pDescriptorPool, VkDescriptorSetLayout * pDescriptorSetLayout, Uint32 setCount, Uint32 SetsCount, VkDescriptorSet ** ppDescriptorSets);
extern void SDLCALL addDescriptorUpdate_Buffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, VkBuffer * pBuffer, VkDeviceSize offset, VkDeviceSize range);
extern void SDLCALL addDescriptorUpdate_Texture(VkDescriptorType descriptorType, Uint32 binding, const char * innerName, VkSampler sampler, VkImageLayout layout);
extern void SDLCALL addDescriptorUpdate_TexelBuffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, VkBufferView * pBufferView);
extern void SDLCALL executeUpdateDescriptorSets(void);

#include "SDL3/SDL_close_code.h"

#endif
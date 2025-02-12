#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_DESCRIPTORPOOL_H
#define VK_DESCRIPTORPOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createDescriptorPool(VkDevice * pDevice, Uint32 poolSizeCount, VkDescriptorPoolSize * pPoolSizes, Uint32 maxSets, VkDescriptorPool * pDescriptorPool);
extern void SDLCALL modifyPoolSizeDescriptorCount(VkDescriptorType type, Uint32 addDescriptorCount, Uint32 poolCount, VkDescriptorPoolSize ** ppPoolSize);
extern void SDLCALL updateGraphicDescriptorSets(VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkDescriptorSet ** ppDescriptorSets, VkImageView * pTextureImageView, VkSampler * pTextureSampler);
extern void SDLCALL createGraphicDescriptorSets(VkDevice * pDevice, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets);
extern void SDLCALL updateParticleDescriptorSets(VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkDescriptorSet ** ppDescriptorSets);
extern void SDLCALL createParticleDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets);
extern void SDLCALL updateComputeDescriptorSets(VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkBuffer (*ppShaderStorageBuffers)[2], VkDescriptorSet ** ppDescriptorSets);
extern void SDLCALL createComputeDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkBuffer ** ppShaderStorageBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets);
extern void SDLCALL createDescriptorSets(VkDevice * pDevice, VkBuffer ** ppGraphicsUniformBuffers, VkBuffer ** ppComputeUniformBuffers, VkBuffer ** ppShaderStorageBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets, VkImageView * pTextureImageView, VkSampler * pTextureSampler);

#include "SDL3/SDL_close_code.h"

#endif
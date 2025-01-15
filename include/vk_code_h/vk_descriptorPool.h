#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_DESCRIPTORPOOL_H
#define VK_DESCRIPTORPOOL_H 1

void addDescriptorSetLayout(VkDevice * pDevice, uint32_t bindingCount, VkDescriptorSetLayoutBinding * pBindings, uint32_t set, VkDescriptorSetLayout ** ppDescriptorSetLayout);
void setDescriptorSetLayoutBinding(VkDescriptorType descriptorType, VkShaderStageFlags stage, uint32_t descriptorCount, uint32_t binding, uint32_t * pBindingCount, VkDescriptorSetLayoutBinding ** ppDescriptorSetLayoutBinding);
void createDescriptorPool(VkDevice * pDevice, uint32_t poolSizeCount, VkDescriptorPoolSize * pPoolSizes, uint32_t maxSets, VkDescriptorPool * pDescriptorPool);
void setDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount, uint32_t * pPoolCount, VkDescriptorPoolSize ** ppPoolSize);
void modifyPoolSizeDescriptorCount(VkDescriptorType type, uint32_t addDescriptorCount, uint32_t poolCount, VkDescriptorPoolSize ** ppPoolSize);
void createGraphicDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets, VkImageView * pTextureImageView, VkSampler * pTextureSampler);
void createParticleDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets);
void createComputeDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkBuffer ** ppShaderStorageBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets);
void createDescriptorSets(VkDevice * pDevice, VkBuffer ** ppGraphicsUniformBuffers, VkBuffer ** ppComputeUniformBuffers, VkBuffer ** ppShaderStorageBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets, VkImageView * pTextureImageView, VkSampler * pTextureSampler);

#endif
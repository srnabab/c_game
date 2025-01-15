#include "G_begin_code.h"
#include "G_constants.h"
#include "SDL3/SDL_stdinc.h"
#include "vk_code_h/vk_descriptorPool.h"
#include "vk_code_h/vk_judge.h"
#include "vk_struct.h"
#include "G_log.h"

void addDescriptorSetLayout(VkDevice * pDevice, uint32_t bindingCount, VkDescriptorSetLayoutBinding * pBindings, uint32_t set, VkDescriptorSetLayout ** ppDescriptorSetLayout)
{
    // FuncCode code = createDescriptorSetLayoutF;

    *ppDescriptorSetLayout = (VkDescriptorSetLayout *)SDL_realloc(*ppDescriptorSetLayout, (set + 1) * sizeof(VkDescriptorSetLayout));

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = NULL;
    layoutInfo.flags = 0;
    layoutInfo.bindingCount = bindingCount;
    layoutInfo.pBindings = pBindings;

    resultVulkan(vkCreateDescriptorSetLayout(*pDevice, &layoutInfo, NULL, &(*ppDescriptorSetLayout)[set]), createDescriptorSetLayoutF, 0);
}
void setDescriptorSetLayoutBinding(VkDescriptorType descriptorType, VkShaderStageFlags stage, uint32_t descriptorCount, uint32_t binding, uint32_t * pBindingCount, VkDescriptorSetLayoutBinding ** ppDescriptorSetLayoutBinding)
{
    (*pBindingCount)++;

    (*ppDescriptorSetLayoutBinding) = (VkDescriptorSetLayoutBinding *)SDL_realloc(*ppDescriptorSetLayoutBinding, *pBindingCount * sizeof(VkDescriptorSetLayoutBinding));

    uint32_t index = *pBindingCount - 1;
    (*ppDescriptorSetLayoutBinding)[index].binding = binding;
    (*ppDescriptorSetLayoutBinding)[index].descriptorType = descriptorType;
    (*ppDescriptorSetLayoutBinding)[index].descriptorCount = descriptorCount;
    (*ppDescriptorSetLayoutBinding)[index].stageFlags = stage;
    (*ppDescriptorSetLayoutBinding)[index].pImmutableSamplers = NULL;

    logMessage("binding: %u", binding);
}
void createDescriptorPool(VkDevice * pDevice, uint32_t poolSizeCount, VkDescriptorPoolSize * pPoolSizes, uint32_t maxSets, VkDescriptorPool * pDescriptorPool)
{
    FuncCode code = createDescriptorPoolF;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = NULL;
    poolInfo.flags = 0;
    poolInfo.poolSizeCount = poolSizeCount;
    poolInfo.pPoolSizes = pPoolSizes;
    poolInfo.maxSets  = maxSets;

    resultVulkan(vkCreateDescriptorPool(*pDevice, &poolInfo, NULL, pDescriptorPool), code, 0);
}
void setDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount, uint32_t * pPoolCount, VkDescriptorPoolSize ** ppPoolSize)
{
    (*pPoolCount)++;
    (*ppPoolSize) = (VkDescriptorPoolSize *)SDL_realloc(*ppPoolSize, *pPoolCount * sizeof(VkDescriptorPoolSize));

    uint32_t index = *pPoolCount - 1;
    (*ppPoolSize)[index].type = type;
    (*ppPoolSize)[index].descriptorCount = descriptorCount;
}
void modifyPoolSizeDescriptorCount(VkDescriptorType type, uint32_t addDescriptorCount, uint32_t poolCount, VkDescriptorPoolSize ** ppPoolSize)
{
    for (uint32_t i = 0;i < poolCount;i++)
    {
        if ((*ppPoolSize)[i].type == type)
        {
            (*ppPoolSize)[i].descriptorCount += addDescriptorCount;
        }
    }
}
void createGraphicDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets, VkImageView * pTextureImageView, VkSampler * pTextureSampler)
{
    FuncCode code = createDescriptorSetsF;

    VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkDescriptorSetLayout));
    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        layouts[i] = *pDescriptorLayout;
    }
    *ppDescriptorSets = (VkDescriptorSet *)SDL_calloc(MAX_FRAMES_IN_FLIGHT ,sizeof(VkDescriptorSet));

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
    allocInfo.descriptorPool = *pDescriptorPool;
    allocInfo.pSetLayouts = layouts;

    resultVulkan(vkAllocateDescriptorSets(*pDevice, &allocInfo, *ppDescriptorSets), code, 2, layouts, *ppDescriptorSets);

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        VkWriteDescriptorSet descriptorWrite[2];

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = (*ppUniformBuffers)[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].pNext = NULL;
        descriptorWrite[0].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pImageInfo = NULL;
        descriptorWrite[0].pBufferInfo = &bufferInfo;
        descriptorWrite[0].pTexelBufferView = NULL;

        VkDescriptorImageInfo imageInfo[3];
        for (int j = 0;j < 3;j++)
        {
            imageInfo[j].sampler = *pTextureSampler;
            imageInfo[j].imageView = pTextureImageView[j];
            imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].pNext = NULL;
        descriptorWrite[1].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[1].dstBinding = 1;
        descriptorWrite[1].dstArrayElement = 0;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[1].descriptorCount = 3;
        descriptorWrite[1].pImageInfo = imageInfo;
        descriptorWrite[1].pBufferInfo = NULL;
        descriptorWrite[1].pTexelBufferView = NULL;

        vkUpdateDescriptorSets(*pDevice, 2, descriptorWrite, 0, NULL);
    }
}
void createParticleDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets)
{
    VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkDescriptorSetLayout));
    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        layouts[i] = *pDescriptorLayout;
    }
    *ppDescriptorSets = (VkDescriptorSet *)SDL_calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
    allocInfo.descriptorPool = *pDescriptorPool;
    allocInfo.pSetLayouts = layouts;

    logMessage("allocate descriptor sets: %d", vkAllocateDescriptorSets(*pDevice, &allocInfo, *ppDescriptorSets));

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        VkWriteDescriptorSet descriptorWrite[1];

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = (*ppUniformBuffers)[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].pNext = NULL;
        descriptorWrite[0].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[0].dstBinding = 1;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pImageInfo = NULL;
        descriptorWrite[0].pBufferInfo = &bufferInfo;
        descriptorWrite[0].pTexelBufferView = NULL;

        vkUpdateDescriptorSets(*pDevice, 1, descriptorWrite, 0, NULL);
    }
}
void createComputeDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkBuffer ** ppShaderStorageBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets)
{
    VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(3 * sizeof(VkDescriptorSetLayout));
    for (int i = 0;i < 3;i++)
    {
        layouts[i] = *pDescriptorLayout;
    }
    *ppDescriptorSets = (VkDescriptorSet *)SDL_calloc(2, sizeof(VkDescriptorSet));

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.descriptorSetCount = (uint32_t)2;
    allocInfo.descriptorPool = *pDescriptorPool;
    allocInfo.pSetLayouts = layouts;

    logMessage("allocate descriptor sets: %d", vkAllocateDescriptorSets(*pDevice, &allocInfo, *ppDescriptorSets));

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        VkWriteDescriptorSet descriptorWrite[3];

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = (*ppUniformBuffers)[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(ComputeUniformBufferObject);

        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].pNext = NULL;
        descriptorWrite[0].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pImageInfo = NULL;
        descriptorWrite[0].pBufferInfo = &bufferInfo;
        descriptorWrite[0].pTexelBufferView = NULL;
        
        VkDescriptorBufferInfo storageBufferInfoLastFrame = {};
        storageBufferInfoLastFrame.buffer = (*ppShaderStorageBuffers)[(i + MAX_FRAMES_IN_FLIGHT - 1) % MAX_FRAMES_IN_FLIGHT];
        storageBufferInfoLastFrame.offset = 0;
        storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].pNext = NULL;
        descriptorWrite[1].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[1].dstBinding = 1;
        descriptorWrite[1].dstArrayElement = 0;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pImageInfo = NULL;
        descriptorWrite[1].pBufferInfo = &storageBufferInfoLastFrame;
        descriptorWrite[1].pTexelBufferView = NULL;

        VkDescriptorBufferInfo storageBufferInfoCurrentFrame = {};
        storageBufferInfoCurrentFrame.buffer = (*ppShaderStorageBuffers)[i];
        storageBufferInfoCurrentFrame.offset = 0;
        storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

        descriptorWrite[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[2].pNext = NULL;
        descriptorWrite[2].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[2].dstBinding = 2;
        descriptorWrite[2].dstArrayElement = 0;
        descriptorWrite[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrite[2].descriptorCount = 1;
        descriptorWrite[2].pImageInfo = NULL;
        descriptorWrite[2].pBufferInfo = &storageBufferInfoCurrentFrame;
        descriptorWrite[2].pTexelBufferView = NULL;

        vkUpdateDescriptorSets(*pDevice, 3, descriptorWrite, 0, NULL);
    }
}
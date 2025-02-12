#include "G_constants.h"
#include "G_resource.h"

#include "vk_code_h/vk_descriptorPool.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

#include "G_log.h"

extern VK_ALL allInOne;

void createDescriptorPool(VkDevice * pDevice, Uint32 poolSizeCount, VkDescriptorPoolSize * pPoolSizes, Uint32 maxSets, VkDescriptorPool * pDescriptorPool)
{
    FuncCode code = createDescriptorPoolF;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = NULL;
    poolInfo.flags = 0;
    poolInfo.poolSizeCount = poolSizeCount;
    poolInfo.pPoolSizes = pPoolSizes;
    poolInfo.maxSets  = maxSets;

    resultVulkan(vkCreateDescriptorPool(*pDevice, &poolInfo, allInOne.pAllocationCallbacks, pDescriptorPool), code, 0);
}
void modifyPoolSizeDescriptorCount(VkDescriptorType type, Uint32 addDescriptorCount, Uint32 poolCount, VkDescriptorPoolSize ** ppPoolSize)
{
    for (Uint32 i = 0;i < poolCount;i++)
    {
        if ((*ppPoolSize)[i].type == type)
        {
            (*ppPoolSize)[i].descriptorCount += addDescriptorCount;
        }
    }
}
void updateGraphicDescriptorSets(VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkDescriptorSet ** ppDescriptorSets, VkImageView * pTextureImageView, VkSampler * pTextureSampler)
{
    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        VkWriteDescriptorSet descriptorWrite[4];

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = (*ppUniformBuffers)[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

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
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pImageInfo = imageInfo + 0;
        descriptorWrite[1].pBufferInfo = NULL;
        descriptorWrite[1].pTexelBufferView = NULL;

        descriptorWrite[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[2].pNext = NULL;
        descriptorWrite[2].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[2].dstBinding = 2;
        descriptorWrite[2].dstArrayElement = 0;
        descriptorWrite[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[2].descriptorCount = 1;
        descriptorWrite[2].pImageInfo = imageInfo + 1;
        descriptorWrite[2].pBufferInfo = NULL;
        descriptorWrite[2].pTexelBufferView = NULL;

        descriptorWrite[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[3].pNext = NULL;
        descriptorWrite[3].dstSet = (*ppDescriptorSets)[i];
        descriptorWrite[3].dstBinding = 3;
        descriptorWrite[3].dstArrayElement = 0;
        descriptorWrite[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[3].descriptorCount = 1;
        descriptorWrite[3].pImageInfo = imageInfo + 2;
        descriptorWrite[3].pBufferInfo = NULL;
        descriptorWrite[3].pTexelBufferView = NULL;

        vkUpdateDescriptorSets(*pDevice, 4, descriptorWrite, 0, NULL);
    }
}
void updateParticleDescriptorSets(VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkDescriptorSet ** ppDescriptorSets)
{
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
        descriptorWrite[0].dstBinding = 0;
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
void updateComputeDescriptorSets(VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkBuffer (*ppShaderStorageBuffers)[2], VkDescriptorSet ** ppDescriptorSets)
{
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
static bool useImageInfo(VkDescriptorType type)
{
    if (type < 0) return false;
    // VK_DESCRIPTOR_TYPE_SAMPLER = 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1
    // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3
    if (type < 4) return true; 
    // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10
    if (type == 10) return true;

    return false;
}
static bool useBufferInfo(VkDescriptorType type)
{
    if (type < 6) return false;
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9
    if (type < 10) return true;
    // VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000, VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK
    if (type == 1000138000) return true;

    return false;
}
static bool useTexelBuffer(VkDescriptorType type)
{
    // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5
    if (type == 4 || type == 5) return true;

    return false;
}
void updateDescriptorSets(G_DescriptorSet_Update * pUpdate, Uint32 updateCount)
{
    int i, j;
    VkWriteDescriptorSet * pWriteDescriptorSets = (VkWriteDescriptorSet*)SDL_malloc(updateCount * MAX_FRAMES_IN_FLIGHT * sizeof(VkWriteDescriptorSet));
    void ** ppCreateInfo = (void*)SDL_malloc(updateCount * MAX_FRAMES_IN_FLIGHT * sizeof(void*));
    for (i = 0;i < updateCount;i++)
    {
        if (useBufferInfo(pUpdate[i].descriptorType))
        {
            for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
            {
                Uint32 offset = j * updateCount;
                ppCreateInfo[i + offset] = SDL_malloc(sizeof(VkDescriptorBufferInfo));
                VkDescriptorBufferInfo * temp = (VkDescriptorBufferInfo *)(ppCreateInfo[i + offset]);
                temp->buffer = pUpdate[i].bufferImage.pBuffer->pBuffer[j];
                temp->offset = pUpdate[i].bufferImage.pBuffer->offset;
                temp->range = pUpdate[i].bufferImage.pBuffer->range;

                pWriteDescriptorSets[i + offset].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                pWriteDescriptorSets[i + offset].pNext = NULL;
                pWriteDescriptorSets[i + offset].dstSet = pUpdate[i].pSet[j];
                pWriteDescriptorSets[i + offset].dstBinding = pUpdate[i].binding;
                pWriteDescriptorSets[i + offset].dstArrayElement = 0;
                pWriteDescriptorSets[i + offset].descriptorType = pUpdate[i].descriptorType;
                pWriteDescriptorSets[i + offset].descriptorCount = 1;
                pWriteDescriptorSets[i + offset].pImageInfo = NULL;
                pWriteDescriptorSets[i + offset].pBufferInfo = (VkDescriptorBufferInfo*)(ppCreateInfo[i + offset]);
                pWriteDescriptorSets[i + offset].pTexelBufferView = NULL;
            }
        }
        else if (useImageInfo(pUpdate[i].descriptorType))
        {
            for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
            {
                Uint32 offset = j * updateCount;
                ppCreateInfo[i + offset] = SDL_malloc(sizeof(VkDescriptorImageInfo));
                VkDescriptorImageInfo * temp = (VkDescriptorImageInfo *)(ppCreateInfo[i + offset]);
                temp->sampler = pUpdate[i].bufferImage.pTexture->pSampler[j];
                temp->imageView = pUpdate[i].bufferImage.pTexture->pParent->imageView;
                temp->imageLayout = pUpdate[i].bufferImage.pTexture->layout;

                pWriteDescriptorSets[i + offset].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                pWriteDescriptorSets[i + offset].pNext = NULL;
                pWriteDescriptorSets[i + offset].dstSet = pUpdate[i].pSet[j];
                pWriteDescriptorSets[i + offset].dstBinding = pUpdate[i].binding;
                pWriteDescriptorSets[i + offset].dstArrayElement = 0;
                pWriteDescriptorSets[i + offset].descriptorType = pUpdate[i].descriptorType;
                pWriteDescriptorSets[i + offset].descriptorCount = 1;
                pWriteDescriptorSets[i + offset].pImageInfo = (VkDescriptorImageInfo*)(ppCreateInfo[i + offset]);
                pWriteDescriptorSets[i + offset].pBufferInfo = NULL;
                pWriteDescriptorSets[i + offset].pTexelBufferView = NULL;
            }
        }
        else if (useTexelBuffer(pUpdate[i].descriptorType))
        {
            for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
            {
                Uint32 offset = j * updateCount;
                ppCreateInfo[i + offset] = SDL_malloc(sizeof(VkBufferView));
                ppCreateInfo[i + offset] = (void*)&pUpdate[i].bufferImage.pTexelBuffer->pBufferView[j];

                pWriteDescriptorSets[i + offset].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                pWriteDescriptorSets[i + offset].pNext = NULL;
                pWriteDescriptorSets[i + offset].dstSet = pUpdate[i].pSet[j];
                pWriteDescriptorSets[i + offset].dstBinding = pUpdate[i].binding;
                pWriteDescriptorSets[i + offset].dstArrayElement = 0;
                pWriteDescriptorSets[i + offset].descriptorType = pUpdate[i].descriptorType;
                pWriteDescriptorSets[i + offset].descriptorCount = 1;
                pWriteDescriptorSets[i + offset].pImageInfo = NULL;
                pWriteDescriptorSets[i + offset].pBufferInfo = NULL;
                pWriteDescriptorSets[i + offset].pTexelBufferView = (VkBufferView*)(ppCreateInfo[i + offset]);
            }
        }
    }
    vkUpdateDescriptorSets(*allInOne.pDevice, updateCount * MAX_FRAMES_IN_FLIGHT, pWriteDescriptorSets, 0, NULL);

    for (i = 0;i < updateCount * MAX_FRAMES_IN_FLIGHT;i++)
    {
        SDL_free(ppCreateInfo[i]);
    }
    SDL_free(ppCreateInfo);
    SDL_free(pWriteDescriptorSets);
}
// void createGraphicDescriptorSets(VkDevice * pDevice, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets)
// {
//     FuncCode code = createDescriptorSetsF;

//     VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkDescriptorSetLayout));
//     for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
//     {
//         layouts[i] = *pDescriptorLayout;
//     }
//     *ppDescriptorSets = (VkDescriptorSet *)SDL_calloc(MAX_FRAMES_IN_FLIGHT ,sizeof(VkDescriptorSet));

//     VkDescriptorSetAllocateInfo allocInfo = {};
//     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//     allocInfo.pNext = NULL;
//     allocInfo.descriptorSetCount = (Uint32)MAX_FRAMES_IN_FLIGHT;
//     allocInfo.descriptorPool = *pDescriptorPool;
//     allocInfo.pSetLayouts = layouts;

//     resultVulkan(vkAllocateDescriptorSets(*pDevice, &allocInfo, *ppDescriptorSets), code, 2, layouts, *ppDescriptorSets);
// }
// void createComputeDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkBuffer ** ppShaderStorageBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets)
// {
//     VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(3 * sizeof(VkDescriptorSetLayout));
//     for (int i = 0;i < 3;i++)
//     {
//         layouts[i] = *pDescriptorLayout;
//     }
//     *ppDescriptorSets = (VkDescriptorSet *)SDL_calloc(2, sizeof(VkDescriptorSet));

//     VkDescriptorSetAllocateInfo allocInfo = {};
//     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//     allocInfo.pNext = NULL;
//     allocInfo.descriptorSetCount = (Uint32)2;
//     allocInfo.descriptorPool = *pDescriptorPool;
//     allocInfo.pSetLayouts = layouts;

//     logMessage("allocate descriptor sets: %d", vkAllocateDescriptorSets(*pDevice, &allocInfo, *ppDescriptorSets));
// }
// void createParticleDescriptorSets(VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDescriptorSetLayout * pDescriptorLayout, VkDescriptorPool * pDescriptorPool, VkDescriptorSet ** ppDescriptorSets)
// {
//     VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkDescriptorSetLayout));
//     for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
//     {
//         layouts[i] = *pDescriptorLayout;
//     }
//     *ppDescriptorSets = (VkDescriptorSet *)SDL_calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));

//     VkDescriptorSetAllocateInfo allocInfo = {};
//     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//     allocInfo.pNext = NULL;
//     allocInfo.descriptorSetCount = (Uint32)MAX_FRAMES_IN_FLIGHT;
//     allocInfo.descriptorPool = *pDescriptorPool;
//     allocInfo.pSetLayouts = layouts;

//     logMessage("allocate descriptor sets: %d", vkAllocateDescriptorSets(*pDevice, &allocInfo, *ppDescriptorSets));

//     for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
//     {
//         VkWriteDescriptorSet descriptorWrite[1];

//         VkDescriptorBufferInfo bufferInfo = {};
//         bufferInfo.buffer = (*ppUniformBuffers)[i];
//         bufferInfo.offset = 0;
//         bufferInfo.range = sizeof(UniformBufferObject);

//         descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrite[0].pNext = NULL;
//         descriptorWrite[0].dstSet = (*ppDescriptorSets)[i];
//         descriptorWrite[0].dstBinding = 1;
//         descriptorWrite[0].dstArrayElement = 0;
//         descriptorWrite[0].descriptorCount = 1;
//         descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         descriptorWrite[0].descriptorCount = 1;
//         descriptorWrite[0].pImageInfo = NULL;
//         descriptorWrite[0].pBufferInfo = &bufferInfo;
//         descriptorWrite[0].pTexelBufferView = NULL;

//         vkUpdateDescriptorSets(*pDevice, 1, descriptorWrite, 0, NULL);
//     }
// }
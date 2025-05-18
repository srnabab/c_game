#include "G_constants.h"
#include "G_texture.h"
#include "G_allocator.h"

#include "vk_code_h/vk_descriptorPool.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_log.h"
#include "G_struct.h"

#define MAX_UPDATE_COUNT 40

extern VK_ALL allInOne;
extern G_SYNC allSync;

#warning "descriptor set update is fixed to 40"
static G_DescriptorSet_Update updates[MAX_UPDATE_COUNT];
static Uint32 updatesCount = 0;

void createDescriptorPool(VkDevice * pDevice, Uint32 poolSizeCount, VkDescriptorPoolSize * pPoolSizes, Uint32 maxSets, VkDescriptorPool * pDescriptorPool)
{
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = NULL;
    poolInfo.flags = 0;
    poolInfo.poolSizeCount = poolSizeCount;
    poolInfo.pPoolSizes = pPoolSizes;
    poolInfo.maxSets  = maxSets;

    resultVulkan(vkCreateDescriptorPool(*pDevice, &poolInfo, allInOne.pAllocationCallbacks, pDescriptorPool), 0);
}
void createDescriptorSets(VkDescriptorPool * pDescriptorPool, VkDescriptorSetLayout * pDescriptorSetLayout, Uint32 setCount, Uint32 SetsCount, VkDescriptorSet ** ppDescriptorSets)
{
    int i, j, k;
    VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)G_malloc(MAX_FRAMES_IN_FLIGHT * setCount * SetsCount * sizeof(VkDescriptorSetLayout));
    for (k = 0;k < SetsCount;k++)
    for (i = 0;i < setCount;i++)
    for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
    {
        layouts[i * MAX_FRAMES_IN_FLIGHT + j + k * MAX_FRAMES_IN_FLIGHT * setCount] = pDescriptorSetLayout[i];
    }

    *ppDescriptorSets = (VkDescriptorSet *)G_malloc(MAX_FRAMES_IN_FLIGHT * SetsCount * setCount * sizeof(VkDescriptorSet));

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.descriptorSetCount = (Uint32)(MAX_FRAMES_IN_FLIGHT * setCount * SetsCount);
    allocInfo.descriptorPool = *pDescriptorPool;
    allocInfo.pSetLayouts = layouts;

    print("%d", vkAllocateDescriptorSets(allInOne.device, &allocInfo, *ppDescriptorSets));

    G_free(layouts);
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
static int getShaderStorageBufferIndex(void * pBufferAddress)
{
    static void ** ptrs = NULL;
    static int8_t count = 0;
    static int8_t * enterCount = NULL;
    int i;

    if (pBufferAddress == NULL)
    {
        if (count)
        {
            count = 0;
            G_free(ptrs);
            G_free(enterCount);
            return 2;
        }
        else return 2;
    }

    for (i = 0;i < count;i++)
    {
        if (ptrs[i] == pBufferAddress)
        {
            enterCount[i]++;
            if (enterCount[i] == 2) return 0;
            if (enterCount[i] == 3) return 0;
            if (enterCount[i] == 4) return 1;
        }
    }

    if (i == count)
    {
        count++;
        ptrs = (void**)G_realloc(ptrs, count * sizeof(void*));
        enterCount = (int8_t*)G_realloc(enterCount, count * sizeof(int8_t*));

        ptrs[i] = pBufferAddress;
        enterCount[i] = 0;
        enterCount[i]++;

        return 1;
    }

    return 2;
}
static void outOfCount(void)
{
    print("out of limit");
}
void addDescriptorUpdate_Buffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, G_Buffer ** pBuffers)
{
    G_Descriptor_Update_Buffer tempBuffer;
    tempBuffer.pBuffer = pBuffers;

    SDL_LockMutex(allSync.descriptorUpdateMutex);

    if (updatesCount == MAX_UPDATE_COUNT) 
    {
        outOfCount();
        return;
    }

    updates[updatesCount].descriptorType = descriptorType;
    updates[updatesCount].binding = binding;
    updates[updatesCount].pSet = pSet;
    updates[updatesCount].bufferImage.Buffer = tempBuffer;
    updatesCount++;

    SDL_UnlockMutex(allSync.descriptorUpdateMutex);
}
void addDescriptorUpdate_Texture(VkDescriptorType descriptorType, Uint32 binding, const char *innerName, VkSampler sampler, VkImageLayout layout)
{
    G_Descriptor_Update_Texture tempTexture;
    tempTexture.pParent = getTexture(innerName);
    tempTexture.sampler = sampler;
    tempTexture.layout = layout;

    SDL_LockMutex(allSync.descriptorUpdateMutex);

    if (updatesCount == MAX_UPDATE_COUNT) 
    {
        outOfCount();
        return;
    }

    updates[updatesCount].descriptorType = descriptorType;
    updates[updatesCount].binding = binding;
    updates[updatesCount].pSet = tempTexture.pParent->pDescriptorSet;
    updates[updatesCount].bufferImage.Texture = tempTexture;
    updatesCount++;

    SDL_UnlockMutex(allSync.descriptorUpdateMutex);
}
void addDescriptorUpdate_TexelBuffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, VkBufferView * pBufferView)
{
    G_Descriptor_Update_Buffer_View tempBufferView;
    tempBufferView.pBufferView = pBufferView;

    SDL_LockMutex(allSync.descriptorUpdateMutex);

    if (updatesCount == MAX_UPDATE_COUNT) 
    {
        outOfCount();
        return;
    }

    updates[updatesCount].descriptorType = descriptorType;
    updates[updatesCount].binding = binding;
    updates[updatesCount].pSet = pSet;
    updates[updatesCount].bufferImage.TexelBuffer = tempBufferView;
    updatesCount++;

    SDL_UnlockMutex(allSync.descriptorUpdateMutex);
}
static void updateDescriptorSets(G_DescriptorSet_Update * pUpdate, Uint32 updateCount)
{
    int i, j;
    VkWriteDescriptorSet * pWriteDescriptorSets = (VkWriteDescriptorSet*)G_malloc(updateCount * MAX_FRAMES_IN_FLIGHT * sizeof(VkWriteDescriptorSet));
    void ** ppCreateInfo = (void*)G_malloc(updateCount * MAX_FRAMES_IN_FLIGHT * sizeof(void*));
    for (i = 0;i < updateCount;i++)
    {
        if (useBufferInfo(pUpdate[i].descriptorType))
        {
            for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
            {
                Uint32 offset = j * updateCount;
                ppCreateInfo[i + offset] = G_malloc(sizeof(VkDescriptorBufferInfo));
                VkDescriptorBufferInfo * temp = (VkDescriptorBufferInfo *)(ppCreateInfo[i + offset]);
                if (pUpdate[i].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                {
                    Uint32 tempIndex = getShaderStorageBufferIndex(pUpdate[i].bufferImage.Buffer.pBuffer);
                    temp->buffer = pUpdate[i].bufferImage.Buffer.pBuffer[tempIndex]->pBufferPool->buffer;
                    temp->offset = pUpdate[i].bufferImage.Buffer.pBuffer[tempIndex]->startOffset;
                    temp->range = pUpdate[i].bufferImage.Buffer.pBuffer[tempIndex]->bufferSize;
                }
                else
                {
                    temp->buffer = pUpdate[i].bufferImage.Buffer.pBuffer[j]->pBufferPool->buffer;
                    temp->offset = pUpdate[i].bufferImage.Buffer.pBuffer[j]->startOffset;
                    temp->range = pUpdate[i].bufferImage.Buffer.pBuffer[j]->bufferSize;
                }

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
                ppCreateInfo[i + offset] = G_malloc(sizeof(VkDescriptorImageInfo));
                VkDescriptorImageInfo * temp = (VkDescriptorImageInfo *)(ppCreateInfo[i + offset]);
                temp->sampler = pUpdate[i].bufferImage.Texture.sampler;
                temp->imageView = pUpdate[i].bufferImage.Texture.pParent->imageView;
                temp->imageLayout = pUpdate[i].bufferImage.Texture.layout;

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
                ppCreateInfo[i + offset] = G_malloc(sizeof(VkBufferView));
                ppCreateInfo[i + offset] = (void*)&pUpdate[i].bufferImage.TexelBuffer.pBufferView[j];

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
    vkUpdateDescriptorSets(allInOne.device, updateCount * MAX_FRAMES_IN_FLIGHT, pWriteDescriptorSets, 0, NULL);

    for (i = 0;i < updateCount * MAX_FRAMES_IN_FLIGHT;i++)
    {
        G_free(ppCreateInfo[i]);
    }
    G_free(ppCreateInfo);
    G_free(pWriteDescriptorSets);
    getShaderStorageBufferIndex(NULL);
}
void executeUpdateDescriptorSets(void)
{
    SDL_LockMutex(allSync.descriptorUpdateMutex);

    updateDescriptorSets(updates, updatesCount);
    updatesCount = 0;

    SDL_UnlockMutex(allSync.descriptorUpdateMutex);
}
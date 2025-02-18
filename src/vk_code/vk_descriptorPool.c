#include "G_constants.h"
#include "G_resource.h"

#include "vk_code_h/vk_descriptorPool.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

#include "G_log.h"

#include "SDL3/SDL_mutex.h"

extern VK_ALL allInOne;

static G_DescriptorSet_Update updates[20];
static Uint32 updatesCount = 0;
static SDL_Mutex * descriptorSetMutex = NULL;

void initDescriptorUpdate(void)
{
    descriptorSetMutex = SDL_CreateMutex();
}
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
void createDescriptorSets(VkDescriptorPool * pDescriptorPool, VkDescriptorSetLayout * pDescriptorSetLayout, Uint32 setCount, Uint32 SetsCount, VkDescriptorSet ** ppDescriptorSets)
{
    int i, j, k;
    VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * setCount * SetsCount * sizeof(VkDescriptorSetLayout));
    for (k = 0;k < SetsCount;k++)
    for (i = 0;i < setCount;i++)
    for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
    {
        layouts[i * MAX_FRAMES_IN_FLIGHT + j + k * MAX_FRAMES_IN_FLIGHT * setCount] = pDescriptorSetLayout[i];
    }

    *ppDescriptorSets = (VkDescriptorSet *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * SetsCount * setCount * sizeof(VkDescriptorSet));

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.descriptorSetCount = (Uint32)(MAX_FRAMES_IN_FLIGHT * setCount * SetsCount);
    allocInfo.descriptorPool = *pDescriptorPool;
    allocInfo.pSetLayouts = layouts;

    logMessage("%d", vkAllocateDescriptorSets(*allInOne.pDevice, &allocInfo, *ppDescriptorSets));

    SDL_free(layouts);
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
static int getShaderStorageBufferIndex(VkBuffer * pBufferAddress)
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
            SDL_free(ptrs);
            SDL_free(enterCount);
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
        ptrs = (void**)SDL_realloc(ptrs, count * sizeof(void*));
        enterCount = (int8_t*)SDL_realloc(enterCount, count * sizeof(int8_t*));

        ptrs[i] = pBufferAddress;
        enterCount[i] = 0;
        enterCount[i]++;

        return 1;
    }

    return 2;
}
void addDescriptorUpdate_Buffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, VkBuffer * pBuffer, VkDeviceSize offset, VkDeviceSize range)
{
    G_Buffer tempBuffer;
    tempBuffer.pBuffer = pBuffer;
    tempBuffer.offset = offset;
    tempBuffer.range = range;

    SDL_LockMutex(descriptorSetMutex);

    if (updatesCount == 20) return;

    updates[updatesCount].descriptorType = descriptorType;
    updates[updatesCount].binding = binding;
    updates[updatesCount].pSet = pSet;
    updates[updatesCount].bufferImage.Buffer = tempBuffer;
    updatesCount++;

    SDL_UnlockMutex(descriptorSetMutex);
}
void addDescriptorUpdate_Texture(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, PathType type, VkSampler sampler, VkImageLayout layout)
{
    G_Texture tempTexture;
    tempTexture.pParent = getTexture(NULL, type);
    tempTexture.sampler = sampler;
    tempTexture.layout = layout;

    SDL_LockMutex(descriptorSetMutex);

    if (updatesCount == 20) return;

    updates[updatesCount].descriptorType = descriptorType;
    updates[updatesCount].binding = binding;
    updates[updatesCount].pSet = pSet;
    updates[updatesCount].bufferImage.Texture = tempTexture;
    updatesCount++;

    deRefTexture(tempTexture.pParent, None);

    SDL_UnlockMutex(descriptorSetMutex);
}
void addDescriptorUpdate_TexelBuffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, VkBufferView * pBufferView)
{
    G_Buffer_View tempBufferView;
    tempBufferView.pBufferView = pBufferView;

    SDL_LockMutex(descriptorSetMutex);

    if (updatesCount == 20) return;

    updates[updatesCount].descriptorType = descriptorType;
    updates[updatesCount].binding = binding;
    updates[updatesCount].pSet = pSet;
    updates[updatesCount].bufferImage.TexelBuffer = tempBufferView;
    updatesCount++;

    SDL_UnlockMutex(descriptorSetMutex);
}
static void updateDescriptorSets(G_DescriptorSet_Update * pUpdate, Uint32 updateCount)
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
                if (pUpdate[i].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                {
                    temp->buffer = pUpdate[i].bufferImage.Buffer.pBuffer[getShaderStorageBufferIndex(pUpdate[i].bufferImage.Buffer.pBuffer)];
                }
                else
                {
                    temp->buffer = pUpdate[i].bufferImage.Buffer.pBuffer[j];
                }

                temp->offset = pUpdate[i].bufferImage.Buffer.offset;
                temp->range = pUpdate[i].bufferImage.Buffer.range;

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
                ppCreateInfo[i + offset] = SDL_malloc(sizeof(VkBufferView));
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
    vkUpdateDescriptorSets(*allInOne.pDevice, updateCount * MAX_FRAMES_IN_FLIGHT, pWriteDescriptorSets, 0, NULL);

    for (i = 0;i < updateCount * MAX_FRAMES_IN_FLIGHT;i++)
    {
        SDL_free(ppCreateInfo[i]);
    }
    SDL_free(ppCreateInfo);
    SDL_free(pWriteDescriptorSets);
    getShaderStorageBufferIndex(NULL);
}
void executeUpdateDescriptorSets(void)
{
    SDL_LockMutex(descriptorSetMutex);

    updateDescriptorSets(updates, updatesCount);
    updatesCount = 0;

    SDL_UnlockMutex(descriptorSetMutex);
}
void deInitDescriptorUpdate(void)
{
    SDL_DestroyMutex(descriptorSetMutex);
}
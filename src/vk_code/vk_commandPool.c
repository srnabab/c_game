#include "G_constants.h"
#include "G_allocator.h"

#include "vk_code_h/vk_commandPool.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_log.h"

extern VK_ALL allInOne;

void createCommandPool(VkCommandPoolCreateFlags flag, Uint32 graphicsFamilyIndice, VkCommandPool * pCommandPool)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = NULL;
    commandPoolCreateInfo.flags = flag;
    commandPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndice;

    resultVulkan(vkCreateCommandPool(allInOne.device, &commandPoolCreateInfo, allInOne.pAllocationCallbacks, pCommandPool), 0);
    print("command pool created\n");
} 
void createCommandBuffers(VkCommandBufferLevel level, VkCommandPool commandPool, VkCommandBuffer * pCommandBuffer, Uint32 count)
{ 
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = count;

    resultVulkan(vkAllocateCommandBuffers(allInOne.device, &allocInfo, pCommandBuffer), 0);
    print("command buffer allocated\n");
}
void createCommandBuffer(VkCommandBufferLevel level, VkCommandPool commandPool, VkCommandBuffer ** ppCommandBuffer, Uint32 bufferCount)
{
    *ppCommandBuffer = (VkCommandBuffer*)G_malloc(bufferCount * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = bufferCount;

    vkAllocateCommandBuffers(allInOne.device, &allocInfo, *ppCommandBuffer);
    print("command buffer allocated\n");
}
VkCommandPool getCommandPoolByIndice(Uint32 indice)
{
    if (indice == allInOne.queueFamilyIndices.graphicsFamily.familyIndice)
    {
        return allInOne.graphicCommandPool;
    }
    else if (indice == allInOne.queueFamilyIndices.presentFamily.familyIndice)
    {
        return allInOne.presentCommandPool;
    }
    else if (indice == allInOne.queueFamilyIndices.computeFamily.familyIndice)
    {
        return allInOne.computeCommandPool;
    }
    else if (indice == allInOne.queueFamilyIndices.transferFamily.familyIndice)
    {
        return allInOne.transferCommandPool;
    }

    return NULL;
}
VkCommandBuffer * getCommandBufferByIndice(Uint32 indice)
{
    if (indice == allInOne.queueFamilyIndices.graphicsFamily.familyIndice)
    {
        return allInOne.pGraphicCommandBuffer;
    }
    else if (indice == allInOne.queueFamilyIndices.presentFamily.familyIndice)
    {
        return allInOne.pPresentCommandBuffer;
    }
    else if (indice == allInOne.queueFamilyIndices.computeFamily.familyIndice)
    {
        return allInOne.pComputeCommandBuffer;
    }
    else if (indice == allInOne.queueFamilyIndices.transferFamily.familyIndice)
    {
        return allInOne.pTransferCommandBuffer;
    }

    return NULL;
}
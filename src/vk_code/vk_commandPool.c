#include "G_constants.h"

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

    resultVulkan(vkCreateCommandPool(*allInOne.pDevice, &commandPoolCreateInfo, allInOne.pAllocationCallbacks, pCommandPool), 0);
    logMessage("command pool created\n");
} 
void createCommandbufferByBuffering(VkCommandBufferLevel level, VkCommandPool * pCommandPool, VkCommandBuffer (*ppCommandBuffer)[MAX_FRAMES_IN_FLIGHT])
{ 
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = *pCommandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    resultVulkan(vkAllocateCommandBuffers(*allInOne.pDevice, &allocInfo, *ppCommandBuffer), 0);
    logMessage("command buffer allocated\n");
}
void createCommandBuffer(VkCommandBufferLevel level, VkCommandPool * pCommandPool, VkCommandBuffer ** ppCommandBuffer, Uint32 bufferCount)
{
    *ppCommandBuffer = (VkCommandBuffer*)SDL_malloc(bufferCount * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = *pCommandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = bufferCount;

    vkAllocateCommandBuffers(*allInOne.pDevice, &allocInfo, *ppCommandBuffer);
    logMessage("command buffer allocated\n");
}
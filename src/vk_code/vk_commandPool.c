#include "vk_commandPool.h"

void createCommandPool(VkDevice * pDevice, uint32_t graphicsFamilyIndice, VkCommandPool * pCommandPool)
{
    FuncCode code = createCommandPoolF;

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = VK_NULL_HANDLE;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndice;

    resultVulkan(vkCreateCommandPool(*pDevice, &commandPoolCreateInfo, VK_NULL_HANDLE, pCommandPool), code, 0);
    //printf("command pool created\n");
} 
void createCommandbuffer(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer ** pCommandBuffer)
{
    FuncCode code = createCommandbufferF;

    *pCommandBuffer = (VkCommandBuffer *)malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = VK_NULL_HANDLE;
    allocInfo.commandPool = *pCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    resultVulkan(vkAllocateCommandBuffers(*pDevice, &allocInfo, *pCommandBuffer), code, 0);
    //printf("command buffer allocated\n");
}
#include "G_constants.h"

#include "vk_code_h/vk_commandPool.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

extern VK_ALL allInOne;

void createCommandPool(VkDevice * pDevice, Uint32 graphicsFamilyIndice, VkCommandPool * pCommandPool)
{
    FuncCode code = createCommandPoolF;

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = NULL;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndice;

    resultVulkan(vkCreateCommandPool(*pDevice, &commandPoolCreateInfo, allInOne.pAllocationCallbacks, pCommandPool), code, 0);
    //printf("command pool created\n");
} 
void createCommandbuffer(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer ** pCommandBuffer)
{
    FuncCode code = createCommandbufferF;

    *pCommandBuffer = (VkCommandBuffer *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = *pCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    resultVulkan(vkAllocateCommandBuffers(*pDevice, &allocInfo, *pCommandBuffer), code, 0);
    //printf("command buffer allocated\n");
}
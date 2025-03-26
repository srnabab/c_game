#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

VkResult createBuffer(VkBuffer * pBuffer, VkDeviceMemory * pBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkResult result = VK_SUCCESS;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = NULL;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = NULL;

    result |= vkCreateBuffer(*allInOne.pDevice, &bufferCreateInfo, allInOne.pAllocationCallbacks, pBuffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*allInOne.pDevice, *pBuffer, &memRequirements);

    VkMemoryAllocateInfo bufferMemAllocateInfo = {};
    bufferMemAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    bufferMemAllocateInfo.pNext = NULL;
    bufferMemAllocateInfo.allocationSize = memRequirements.size;
    bufferMemAllocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result |= vkAllocateMemory(*allInOne.pDevice, &bufferMemAllocateInfo, allInOne.pAllocationCallbacks, pBufferMemory);
    result |= vkBindBufferMemory(*allInOne.pDevice, *pBuffer, *pBufferMemory, 0);

    if (result)
        return 0x7FFFFFFF;
    else 
        return result;
}
VkResult beginSingleTimeCommands(VkCommandPool * pCommandPool, VkCommandBuffer * pCommandBuffer)
{
    VkResult result = VK_SUCCESS;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = *pCommandPool;
    allocInfo.commandBufferCount = 1;

    result |= vkAllocateCommandBuffers(*allInOne.pDevice, &allocInfo, pCommandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = NULL;

    result |= vkBeginCommandBuffer(*pCommandBuffer, &beginInfo);

    return result;
}
VkResult endSingleTimeCommands(VkCommandPool * pCommandPool, VkQueue * pQueue, VkCommandBuffer * pCommandBuffer)
{
    VkResult result = VK_SUCCESS;

    result |= vkEndCommandBuffer(*pCommandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.pWaitDstStageMask = 0;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = pCommandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;

    result |= vkQueueSubmit(*pQueue, 1, &submitInfo, NULL);
    result |= vkQueueWaitIdle(*pQueue);

    vkFreeCommandBuffers(*allInOne.pDevice, *pCommandPool, 1, pCommandBuffer);

    return result;
}
VkResult copyBuffer(VkBuffer * pSrcBuffer, VkBuffer * pDstBuffer, VkDeviceSize size)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer commandBuffer = NULL;
    result |= beginSingleTimeCommands(allInOne.pTransferCommandPool, &commandBuffer);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, *pSrcBuffer, *pDstBuffer, 1, &copyRegion);

    result |= endSingleTimeCommands(allInOne.pTransferCommandPool, allInOne.pTransferQueue, &commandBuffer);

    if (result)
        return 0x7FFFFFFF;
    else 
        return result;
}
int findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(*allInOne.pPhysicalDevice, &memProperties);
    for (uint32_t i = 0;i < memProperties.memoryTypeCount;i++)
    {
        if ((typeFilter & (i << i)) && (memProperties.memoryTypes[i].propertyFlags & properties))
            return i;
    }

    return -1;
}
void destroyBufferByBuffering(VkBuffer pBuffers[2], VkDeviceMemory pBuffersMem[2])
{
    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        vkUnmapMemory(*allInOne.pDevice, pBuffersMem[i]);
        vkDestroyBuffer(*allInOne.pDevice, pBuffers[i], allInOne.pAllocationCallbacks);
        vkFreeMemory(*allInOne.pDevice, pBuffersMem[i], allInOne.pAllocationCallbacks);
    }
}
void destroyBuffer(VkBuffer Buffer, VkDeviceMemory BufferMem)
{
    vkUnmapMemory(*allInOne.pDevice, BufferMem);
    vkDestroyBuffer(*allInOne.pDevice, Buffer, allInOne.pAllocationCallbacks);
    vkFreeMemory(*allInOne.pDevice, BufferMem, allInOne.pAllocationCallbacks);
}
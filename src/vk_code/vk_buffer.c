#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

VkResult createBuffer(VkBuffer * pBuffer, VkDeviceMemory * pBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
#warning error process needed
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

    result |= vkCreateBuffer(allInOne.device, &bufferCreateInfo, allInOne.pAllocationCallbacks, pBuffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(allInOne.device, *pBuffer, &memRequirements);

    VkMemoryAllocateInfo bufferMemAllocateInfo = {};
    bufferMemAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    bufferMemAllocateInfo.pNext = NULL;
    bufferMemAllocateInfo.allocationSize = memRequirements.size;
    bufferMemAllocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result |= vkAllocateMemory(allInOne.device, &bufferMemAllocateInfo, allInOne.pAllocationCallbacks, pBufferMemory);
    result |= vkBindBufferMemory(allInOne.device, *pBuffer, *pBufferMemory, 0);

    // if (firstQueueCommandPool != NULL)
    // {
    //     result |= initBufferQueueFamily(firstQueueCommandPool, srcQueueFamiltIndice, dstQueueFamilyIndice, *pBuffer, size);
    // }

    if (result)
        return 0x7FFFFFFF;
    else 
        return result;
}
VkResult beginSingleTimeCommands(VkCommandPool commandPool, VkCommandBuffer * pCommandBuffer)
{
    VkResult result = VK_SUCCESS;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    result |= vkAllocateCommandBuffers(allInOne.device, &allocInfo, pCommandBuffer);

    result |= beginCommandBuffer(*pCommandBuffer);

    return result;
}
VkResult endSingleTimeCommands(VkCommandPool commandPool, VkQueue queue, VkCommandBuffer * pCommandBuffer)
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

    result |= vkQueueSubmit(queue, 1, &submitInfo, NULL);
    result |= vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(allInOne.device, commandPool, 1, pCommandBuffer);

    return result;
}
VkResult initBufferQueueFamily(VkCommandPool commandPool, Uint32 srcQueueFamilyIndice, Uint32 dstQueueFamilyIndice, VkBuffer buffer, VkDeviceSize bufferSize)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer singleTimeCommandBuffer = NULL;
    result |= beginSingleTimeCommands(commandPool, &singleTimeCommandBuffer);

    Uint32 clearValue = 0;

    vkCmdFillBuffer(singleTimeCommandBuffer, buffer, 0, bufferSize, clearValue);

    VkBufferMemoryBarrier bufferMemoryBarrierRelease = {};
    _setBufferMemoryBarrier(NULL, VK_ACCESS_SHADER_WRITE_BIT, 0, srcQueueFamilyIndice, dstQueueFamilyIndice , buffer, 0, bufferSize, &bufferMemoryBarrierRelease);
    vkCmdPipelineBarrier(singleTimeCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 1, &bufferMemoryBarrierRelease, 0, NULL);

    result |= endSingleTimeCommands(commandPool, getFirstQueueByCommandPool(commandPool), &singleTimeCommandBuffer);

    if (result)
        return 0x7FFFFFFF;
    else 
        return result;
}
static VkResult _setBufferCopyRegion(VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size, VkBufferCopy * pRegion)
{
    pRegion->srcOffset = srcOffset;
    pRegion->dstOffset = dstOffset;
    pRegion->size = size;
}
VkResult copyBuffer(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkBuffer srcBuffer, VkDeviceSize srcOffset, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer singleTimeCommandBuffer = NULL;
    if (commandBuffer == NULL)
    {
        result |= beginSingleTimeCommands(commandPool, &singleTimeCommandBuffer);
    }
    else
    {
        singleTimeCommandBuffer = commandBuffer;
    }

    VkBufferCopy copyRegion = {};
    _setBufferCopyRegion(srcOffset, dstOffset, size, &copyRegion);

    vkCmdCopyBuffer(singleTimeCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    if (commandBuffer == NULL)
    {
        result |= endSingleTimeCommands(commandPool, getFirstQueueByCommandPool(commandPool), &singleTimeCommandBuffer);
    }


    if (result)
        return 0x7FFFFFFF;
    else 
        return result;
}
int findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(allInOne.physicalDevice, &memProperties);
    for (uint32_t i = 0;i < memProperties.memoryTypeCount;i++)
    {
        if ((typeFilter & (i << i)) && (memProperties.memoryTypes[i].propertyFlags & properties))
            return i;
    }

    return -1;
}
void _setMemoryBarrier(void * memPNext, VkAccessFlags memSrcAccessMask, VkAccessFlags memDstAccessMask, VkMemoryBarrier * pMemoryBarrier)
{
    pMemoryBarrier->sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    pMemoryBarrier->pNext = memPNext;
    pMemoryBarrier->srcAccessMask = memSrcAccessMask;
    pMemoryBarrier->dstAccessMask = memDstAccessMask;
}
void _setBufferMemoryBarrier(void * bufferPNext, VkAccessFlags bufferSrcAccessMask, VkAccessFlags bufferDstAccessMask, Uint32 bufferSrcQueueFamilyIndex, Uint32 bufferDstQueueFamilyIndex, VkBuffer buffer, VkDeviceSize offset\
    , VkDeviceSize size, VkBufferMemoryBarrier * pBufferMemoryBarrier)
{
    pBufferMemoryBarrier->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    pBufferMemoryBarrier->pNext = bufferPNext;
    pBufferMemoryBarrier->srcAccessMask = bufferSrcAccessMask;
    pBufferMemoryBarrier->dstAccessMask = bufferDstAccessMask;
    pBufferMemoryBarrier->srcQueueFamilyIndex = bufferSrcQueueFamilyIndex;
    pBufferMemoryBarrier->dstQueueFamilyIndex = bufferDstQueueFamilyIndex;
    pBufferMemoryBarrier->buffer = buffer;
    pBufferMemoryBarrier->offset = offset;
    pBufferMemoryBarrier->size = size;
}
void _setImageMemoryBarrier(void * imgPNext, VkAccessFlags imgSrcAccessMask, VkAccessFlags imgDstAccessMask, VkImageLayout oldLayout\
    , VkImageLayout newLayout, Uint32 imgSrcQueueFamilyIndex, Uint32 imgDstQueueFamilyIndex, VkImage image, VkImageAspectFlags aspectMask, Uint32 baseMipLevel, Uint32 levelCount, Uint32 baseArrayLayer\
    , Uint32 layerCount, VkImageMemoryBarrier * pImageMemoryBarrier)
{
    pImageMemoryBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    pImageMemoryBarrier->pNext = imgPNext;
    pImageMemoryBarrier->srcAccessMask = imgSrcAccessMask;
    pImageMemoryBarrier->dstAccessMask = imgDstAccessMask;
    pImageMemoryBarrier->oldLayout = oldLayout;
    pImageMemoryBarrier->newLayout = newLayout;
    pImageMemoryBarrier->srcQueueFamilyIndex = imgSrcQueueFamilyIndex;
    pImageMemoryBarrier->dstQueueFamilyIndex = imgDstQueueFamilyIndex;
    pImageMemoryBarrier->image = image;
    pImageMemoryBarrier->subresourceRange.aspectMask = aspectMask;
    pImageMemoryBarrier->subresourceRange.baseMipLevel = baseMipLevel;
    pImageMemoryBarrier->subresourceRange.levelCount = levelCount;
    pImageMemoryBarrier->subresourceRange.baseArrayLayer = baseArrayLayer;
    pImageMemoryBarrier->subresourceRange.layerCount = layerCount;
}
// void getBufferFromQueue(VkCommandPool commandPool, VkAccessFlags dstAccessMask, VkPipelineStageFlags dstStageFlags, Uint32 srcQueueFamilyIndice, Uint32 dstQueueFamilyIndice, VkBuffer buffer, VkDeviceSize bufferSize)
// {
//     VkCommandBuffer singleTimeCommandBuffer = NULL;
//     beginSingleTimeCommands(commandPool, &singleTimeCommandBuffer);

//     VkBufferMemoryBarrier bufferMemoryBarrierRelease = {};
//     _setBufferMemoryBarrier(NULL, srcAccessMask, 0, srcQueueFamilyIndice, dstQueueFamilyIndice , buffer, 0, bufferSize, &bufferMemoryBarrierRelease);
//     vkCmdPipelineBarrier(singleTimeCommandBuffer, srcStageFlags, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 1, &bufferMemoryBarrierRelease, 0, NULL);

//     endSingleTimeCommands(commandPool, getFirstQueueByCommandPool(commandPool), &singleTimeCommandBuffer);
// }
void releaseBufferFromQueue(VkCommandPool commandPool, VkAccessFlags srcAccessMask, VkPipelineStageFlags srcStageFlags, Uint32 srcQueueFamilyIndice, Uint32 dstQueueFamilyIndice, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize bufferSize)
{
    VkCommandBuffer singleTimeCommandBuffer = NULL;
    beginSingleTimeCommands(commandPool, &singleTimeCommandBuffer);

    VkBufferMemoryBarrier bufferMemoryBarrierRelease = {};
    _setBufferMemoryBarrier(NULL, srcAccessMask, 0, srcQueueFamilyIndice, dstQueueFamilyIndice , buffer, offset, bufferSize, &bufferMemoryBarrierRelease);
    vkCmdPipelineBarrier(singleTimeCommandBuffer, srcStageFlags, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 1, &bufferMemoryBarrierRelease, 0, NULL);

    endSingleTimeCommands(commandPool, getFirstQueueByCommandPool(commandPool), &singleTimeCommandBuffer);
}
void destroyBufferByBuffering(VkBuffer pBuffers[2], VkDeviceMemory pBuffersMem[2])
{
    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        vkUnmapMemory(allInOne.device, pBuffersMem[i]);
        vkDestroyBuffer(allInOne.device, pBuffers[i], allInOne.pAllocationCallbacks);
        vkFreeMemory(allInOne.device, pBuffersMem[i], allInOne.pAllocationCallbacks);
    }
}
void destroyBuffer(VkBuffer Buffer, VkDeviceMemory BufferMem)
{
    vkUnmapMemory(allInOne.device, BufferMem);
    vkDestroyBuffer(allInOne.device, Buffer, allInOne.pAllocationCallbacks);
    vkFreeMemory(allInOne.device, BufferMem, allInOne.pAllocationCallbacks);
}
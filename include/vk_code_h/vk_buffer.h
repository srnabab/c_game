#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"
#include "G_buffer.h"

#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult SDLCALL createBuffer(VkBuffer * pBuffer, VkDeviceMemory * pBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
extern VkResult SDLCALL beginSingleTimeCommands(VkCommandPool commandPool, VkCommandBuffer * pCommandBuffer);
extern VkResult SDLCALL endSingleTimeCommands(VkCommandPool commandPool, VkQueue queue, VkCommandBuffer * pCommandBuffer);
extern VkResult SDLCALL initBufferQueueFamily(VkCommandPool commandPool, Uint32 srcQueueFamilyIndice, Uint32 dstQueueFamilyIndice, VkBuffer buffer, VkDeviceSize bufferSize);
extern void SDLCALL initBufferData(G_Buffer * pBuffer, void * data, Uint32 bufferSize);
extern VkResult SDLCALL copyBuffer(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkBuffer srcBuffer, VkDeviceSize srcOffset, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size);
extern void SDLCALL _setBufferCopyRegion(VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size, VkBufferCopy * pRegion);
extern void SDLCALL releaseBufferFromQueue(VkCommandPool commandPool, VkAccessFlags srcAccessMask, VkPipelineStageFlags srcStageFlags, Uint32 srcQueueFamilyIndice, Uint32 dstQueueFamilyIndice, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize bufferSize);
extern void SDLCALL _setMemoryBarrier(void * memPNext, VkAccessFlags memSrcAccessMask, VkAccessFlags memDstAccessMask, VkMemoryBarrier * pMemoryBarrier);
extern void SDLCALL _setBufferMemoryBarrier(void * bufferPNext, VkAccessFlags bufferSrcAccessMask, VkAccessFlags bufferDstAccessMask, Uint32 bufferSrcQueueFamilyIndex, Uint32 bufferDstQueueFamilyIndex, VkBuffer buffer, VkDeviceSize offset\
    , VkDeviceSize size, VkBufferMemoryBarrier * pBufferMemoryBarrier);
extern void SDLCALL _setImageMemoryBarrier(void * imgPNext, VkAccessFlags imgSrcAccessMask, VkAccessFlags imgDstAccessMask, VkImageLayout oldLayout\
    ,VkImageLayout newLayout, Uint32 imgSrcQueueFamilyIndex, Uint32 imgDstQueueFamilyIndex, VkImage image, VkImageAspectFlags aspectMask, Uint32 baseMipLevel, Uint32 levelCount, Uint32 baseArrayLayer\
    ,Uint32 layerCount, VkImageMemoryBarrier * pImageMemoryBarrier);
extern int SDLCALL findMemoryType(Uint32 typeFilter, VkMemoryPropertyFlags properties);
extern void SDLCALL destroyBufferByBuffering(VkBuffer pBuffers[2], VkDeviceMemory pBuffersMem[2]);
extern void SDLCALL destroyBuffer(VkBuffer pBuffer, VkDeviceMemory pBufferMem);

#include "SDL3/SDL_close_code.h"

#endif
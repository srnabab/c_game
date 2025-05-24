#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_mutex.h"
#include "vulkan/vulkan.h"

#ifndef G_BUFFER_H
#define G_BUFFER_H 1

#include "SDL3/SDL_begin_code.h"

struct _G_Buffer;

struct _G_BufferPool
{
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void* bufferMemoryMapped;
    VkDeviceSize totalBufferSize;
    VkDeviceSize usedBufferSize;
    SDL_Mutex * mutex;
    struct _G_Buffer * buffers;
};
typedef struct _G_BufferPool G_BufferPool;

struct _G_Buffer
{
    G_BufferPool * pBufferPool;
    VkDeviceSize bufferSize;
    VkDeviceSize startOffset;
    int32_t currentQueueIndex;
    int32_t used;
    struct _G_Buffer * next;
};
typedef struct _G_Buffer G_Buffer;

extern G_BufferPool SDLCALL createBufferPool(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool staging);
extern void SDLCALL destroyBufferPool(G_BufferPool * pBufferPool);
extern G_Buffer SDLCALL * allocateStagingBuffer(VkDeviceSize bufferSize, G_BufferPool * pBufferPool);
extern void SDLCALL freeStagingBuffer(G_Buffer * pBuffer);
extern G_Buffer * SDLCALL allocateBuffer(VkDeviceSize bufferSize, G_BufferPool * pBufferPool);
extern void SDLCALL freeBuffer(G_Buffer * pBuffer);
extern void SDLCALL bufferMemcpy(G_Buffer * pBuffer, VkDeviceSize offset, void * src, size_t len);
extern void SDLCALL bufferMemmove(G_Buffer * pBuffer, VkDeviceSize srcOffset, VkDeviceSize dstOffset, size_t len);

#include "SDL3/SDL_close_code.h"

#endif // G_buffer.h

#if 0
{
    G_BufferPool bufferPool = createBufferPool(80000, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
    G_Buffer * buffer1 = allocateBuffer(40000, &bufferPool);
    G_Buffer * buffer2 = allocateBuffer(30000, &bufferPool);
    freeBuffer(buffer1);
    buffer1 = allocateBuffer(20000, &bufferPool);
    destroyBufferPool(&bufferPool);
}
#endif
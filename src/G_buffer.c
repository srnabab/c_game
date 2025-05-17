#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"
#include "G_buffer.h"
#include "G_allocator.h" 

extern VK_ALL allInOne;

G_BufferPool createBufferPool(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool staging)
{
    G_BufferPool bufferPool;

#warning error process needed
    createBuffer(&bufferPool.buffer, &bufferPool.bufferMemory, size, usage, properties);

    bufferPool.bufferMemoryMapped = NULL;
    bufferPool.totalBufferSize = size;
    bufferPool.usedBufferSize = 0;
    bufferPool.buffers = NULL;
    if (staging)
    {
        vkMapMemory(allInOne.device, bufferPool.bufferMemory, 0, size, 0, &bufferPool.bufferMemoryMapped);
    }

    bufferPool.mutex = SDL_CreateMutex();

    return bufferPool;
}
void destroyBufferPool(G_BufferPool * pBufferPool)
{
    if (pBufferPool->bufferMemoryMapped) vkUnmapMemory(allInOne.device, pBufferPool->bufferMemory);
    vkFreeMemory(allInOne.device, pBufferPool->bufferMemory, allInOne.pAllocationCallbacks);
    vkDestroyBuffer(allInOne.device, pBufferPool->buffer, allInOne.pAllocationCallbacks);

    G_Buffer * tempBuffer1 = pBufferPool->buffers;
    G_Buffer * tempBuffer2 = NULL;

    while (tempBuffer1 != NULL)
    {
        tempBuffer2 = tempBuffer1;
        tempBuffer1 = tempBuffer1->next;
        G_free(tempBuffer2);
    }

    pBufferPool->buffer = NULL;
    pBufferPool->bufferMemory = NULL;
    pBufferPool->bufferMemoryMapped = NULL;
    pBufferPool->usedBufferSize = 0;
    pBufferPool->totalBufferSize = 0;
}
static G_Buffer * toEnd(G_Buffer * pBuffer)
{
    G_Buffer * tempBuffer = pBuffer;

    while (tempBuffer != NULL)
    {
        if (tempBuffer->next == NULL) return tempBuffer;
        tempBuffer = tempBuffer->next;
    }

    return pBuffer;
}
static G_Buffer * findFreeBuffer(G_Buffer * pBuffer, VkDeviceSize needBufferSize)
{
    G_Buffer * tempBuffer = pBuffer;

    while (tempBuffer != NULL)
    {
        if (pBuffer->used == false)
        {
            if (pBuffer->bufferSize == needBufferSize)
            {
                return pBuffer;
            }
            else if (pBuffer->bufferSize > needBufferSize)
            {
                G_Buffer * newBuffer = G_malloc(sizeof(G_Buffer));
                if (newBuffer == NULL) return NULL;

                newBuffer->bufferSize = pBuffer->bufferSize - needBufferSize;
                newBuffer->currentQueueIndex = pBuffer->currentQueueIndex;
                newBuffer->pBufferPool = pBuffer->pBufferPool;
                newBuffer->startOffset = pBuffer->startOffset + needBufferSize;
                newBuffer->next = pBuffer->next;
                newBuffer->used = false;

                pBuffer->bufferSize = needBufferSize;
                pBuffer->next = newBuffer;
                pBuffer->used = true;

                return pBuffer;
            }
        }
    }

    return NULL;
}
G_Buffer * allocateBuffer(Uint32 bufferSize, G_BufferPool * pBufferPool)
{
    SDL_LockMutex(pBufferPool->mutex);

    G_Buffer * tempBuffer = NULL;
    if (pBufferPool->buffers == NULL)
    {
        pBufferPool->buffers = G_malloc(sizeof(G_Buffer));

        if (pBufferPool->buffers == NULL) 
        {
            SDL_UnlockMutex(pBufferPool->mutex);
            return NULL;
        }

        tempBuffer = pBufferPool->buffers;
    }

    if (bufferSize + pBufferPool->usedBufferSize > pBufferPool->totalBufferSize) 
    {
        tempBuffer = findFreeBuffer(pBufferPool->buffers, bufferSize);

        SDL_UnlockMutex(pBufferPool->mutex);
        return tempBuffer;
    }

    if (tempBuffer == NULL)
    {
        tempBuffer = toEnd(pBufferPool->buffers);

        tempBuffer->next = G_malloc(sizeof(G_Buffer));

        if (tempBuffer->next == NULL) 
        {
            SDL_UnlockMutex(pBufferPool->mutex);
            return NULL;
        }

        tempBuffer = tempBuffer->next;
    }

    tempBuffer->bufferSize = bufferSize;
    tempBuffer->currentQueueIndex = -1;
    tempBuffer->pBufferPool = pBufferPool;
    tempBuffer->startOffset = pBufferPool->usedBufferSize;
    tempBuffer->used = true;
    tempBuffer->next = NULL;

    pBufferPool->usedBufferSize += bufferSize;

    SDL_UnlockMutex(pBufferPool->mutex);

    return tempBuffer;
}
void freeBuffer(G_Buffer * pBuffer)
{
    pBuffer->used = false;
}
void bufferMemcpy(G_Buffer * pBuffer, void * src, size_t len)
{
    memcpy((char*)pBuffer->pBufferPool->bufferMemoryMapped + pBuffer->startOffset, src, len);
}
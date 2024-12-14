#include "vk_code_h/vk_alloc_func.h"

void * VKAPI_PTR SDL_VK_allocationFunc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)allocationScope;
    if (size == 0) return NULL;

    SDL_VK_AllocatedBlock * ptr = SDL_aligned_alloc(alignment, size + sizeof(SDL_VK_AllocatedBlock));
    ptr->size = size;
    ptr->memory = (void*)BYTE_OFFSET(ptr, sizeof(SDL_VK_AllocatedBlock));
    
    return ptr->memory;
}

void * VKAPI_PTR SDL_VK_reallocationFunc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)allocationScope;
    if (size == 0)
    {
        SDL_aligned_free(BYTE_OFFSET(pOriginal, -sizeof(SDL_VK_AllocatedBlock)));
        return NULL;
    }

    SDL_VK_AllocatedBlock * oldBlock = (SDL_VK_AllocatedBlock*)BYTE_OFFSET(pOriginal, -sizeof(SDL_VK_AllocatedBlock));
    size_t copySize = oldBlock->size < size ? oldBlock->size : size;

    SDL_VK_AllocatedBlock * newBlock = SDL_aligned_alloc(alignment, size + sizeof(SDL_VK_AllocatedBlock));
    if (!newBlock) 
    {
        SDL_aligned_free(oldBlock);
        return NULL;
    }
    newBlock->size = size;
    newBlock->memory = (void*)BYTE_OFFSET(newBlock, sizeof(SDL_VK_AllocatedBlock));

    memcpy(newBlock->memory, pOriginal, copySize);
    SDL_aligned_free(oldBlock);
    return newBlock->memory;
}

void VKAPI_PTR SDL_VK_freeFunc(void * pUserData, void * pMemory)
{
    if (pMemory) SDL_aligned_free(BYTE_OFFSET(pMemory, -sizeof(SDL_VK_AllocatedBlock)));
}
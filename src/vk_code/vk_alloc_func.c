#include "vk_code_h/vk_alloc_func.h"
#include "SDL_allocator.h"

void * SDL_VK_alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)allocationScope;
    if (size == 0) return NULL;

    SDL_AllocatedBlock * ptr = SDL_aligned_alloc(alignment, size + sizeof(SDL_AllocatedBlock));
    ptr->size = size;
    ptr->memory = (void*)BYTE_OFFSET(ptr, sizeof(SDL_AllocatedBlock));
    
    return ptr->memory;
}

void * SDL_VK_realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)allocationScope;
    if (size == 0)
    {
        SDL_aligned_free(BYTE_OFFSET(pOriginal, -sizeof(SDL_AllocatedBlock)));
        return NULL;
    }

    SDL_AllocatedBlock * oldBlock = (SDL_AllocatedBlock*)BYTE_OFFSET(pOriginal, -sizeof(SDL_AllocatedBlock));
    size_t copySize = oldBlock->size < size ? oldBlock->size : size;

    SDL_AllocatedBlock * newBlock = SDL_aligned_alloc(alignment, size + sizeof(SDL_AllocatedBlock));
    if (!newBlock) 
    {
        SDL_aligned_free(oldBlock);
        return NULL;
    }
    newBlock->size = size;
    newBlock->memory = (void*)BYTE_OFFSET(newBlock, sizeof(SDL_AllocatedBlock));

    memcpy(newBlock->memory, pOriginal, copySize);
    SDL_aligned_free(oldBlock);
    return newBlock->memory;
}

void SDL_VK_free(void * pUserData, void * pMemory)
{
    if (pMemory) SDL_aligned_free(BYTE_OFFSET(pMemory, -sizeof(SDL_AllocatedBlock)));
}
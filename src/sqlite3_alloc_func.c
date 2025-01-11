#include "SDL_allocator.h"
#include "sqlite3/sqlite3_alloc_func.h"

void * SDL_SQLite_malloc(int size)
{
    if (size == 0) return NULL;

    SDL_AllocatedBlock * ptr = SDL_aligned_alloc(ALIGNMENT, size + sizeof(SDL_AllocatedBlock));
    ptr->size = (int64_t)size;
    ptr->memory = (void*)BYTE_OFFSET(ptr, sizeof(SDL_AllocatedBlock));

    return ptr->memory;
}
void SDL_SQLite_free(void * mem)
{
    SDL_aligned_free(BYTE_OFFSET(mem, -sizeof(SDL_AllocatedBlock)));
}
void * SDL_SQLite_realloc(void * pOriginal, int size)
{
    if (size == 0)
    {
        SDL_aligned_free(BYTE_OFFSET(pOriginal, -sizeof(SDL_AllocatedBlock)));
        return NULL;
    }

    SDL_AllocatedBlock * oldBlock = (SDL_AllocatedBlock*)BYTE_OFFSET(pOriginal, -sizeof(SDL_AllocatedBlock));
    size_t copySize = oldBlock->size < size ? oldBlock->size : size;

    SDL_AllocatedBlock * newBlock = SDL_aligned_alloc(ALIGNMENT, size + sizeof(SDL_AllocatedBlock));
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
int SDL_SQLite_memSize(void * mem)
{
    return (int)((SDL_AllocatedBlock*)BYTE_OFFSET(mem, -sizeof(SDL_AllocatedBlock)))->size;
}
int SDL_SQLite_RoundUp(int size)
{
    return ROUND8(size);
}
void SDL_SQLite_shutDown(void *)
{
    return;
}
int SDL_SQLite_Init(void *)
{
    return SQLITE_OK;
}
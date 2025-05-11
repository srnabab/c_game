// #define TRACE_PTR
#include "G_allocator.h"

#include "SDL3/SDL_atomic.h"

#include "G_log.h"

static Uint64 totalAllocSize = 0;
static Uint32 allocations = 0;
static SDL_SpinLock sizeLock[1];

// alloc size is multiple of 8
void * G_malloc(size_t size)
{
    if (size == 0) return NULL;

    uint64_t allocateSize = ROUND8(size);
    uint64_t * ptr = SDL_malloc(allocateSize + sizeof(uint64_t));

    if (ptr == NULL) return NULL;

    ptr[0] = allocateSize;
    ptr++;

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize += allocateSize + sizeof(uint64_t);
    allocations++;
    SDL_UnlockSpinlock(&sizeLock[0]);

    // print("size: %llu", totalAllocSize);

    return ptr;
}

void * G_calloc(size_t n_elements, size_t elem_size)
{
    if (!n_elements | !elem_size) return NULL;

    uint64_t req = ROUND8(n_elements * elem_size);
    uint64_t * ptr = SDL_calloc(1, req + sizeof(uint64_t));

    if (ptr == NULL) return NULL;

    ptr[0] = req;
    ptr++;

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize += req + sizeof(uint64_t);
    allocations++;
    SDL_UnlockSpinlock(&sizeLock[0]);

    // print("size: %llu", totalAllocSize);

    return ptr;
}

void * G_realloc(void * pOriginal, size_t size)
{
    if (pOriginal == NULL)
    {
        return G_malloc(size);
    }

    if (!size)
    {
        G_free(pOriginal);
        return NULL;
    }

    uint64_t * oldMem = (uint64_t*)BYTE_OFFSET(pOriginal, -sizeof(uint64_t));
    uint64_t originalSize = oldMem[0];
    uint64_t allocateSize = ROUND8(size);
    uint64_t * ptr = SDL_realloc(oldMem, allocateSize + sizeof(uint64_t));

    if (ptr == NULL) return NULL;

    ptr[0] = allocateSize;
    ptr++;

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize -= originalSize;
    totalAllocSize += allocateSize;
    SDL_UnlockSpinlock(&sizeLock[0]);

    // print("size: %llu", totalAllocSize);

    return ptr;
}

void G_free(void * pMemory)
{
    if (pMemory == NULL) return; 

    uint64_t * ptr = (uint64_t*)BYTE_OFFSET(pMemory, -sizeof(uint64_t));

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize -= ptr[0] + sizeof(uint64_t);
    allocations--;
    SDL_UnlockSpinlock(&sizeLock[0]);

    SDL_free(ptr);
    // print("size: %llu", totalAllocSize);
}

#ifdef _WIN32
 #include <windows.h>
#else
 #define SDL_ALIGNED_ALLOC
 #define SLOW_ALIGNED_REALLOC
 #define SDL_ALIGNED_FREE
#endif

void * G_aligned_alloc(size_t alignment, size_t size)
{
    if (size == 0) return NULL;
    if (alignment < 8) alignment = 8;

#ifdef SDL_ALIGNED_FREE
    uint64_t * ptr = SDL_aligned_alloc(alignment, size + alignment + alignment + alignment);
#endif

#ifdef _WIN32
    uint64_t * ptr = _aligned_malloc(size + alignment + alignment + alignment, alignment);
#endif

    if (ptr == NULL) return NULL;

    uint64_t * mem = (uint64_t*)BYTE_OFFSET(ptr, 3 * alignment);
    uint64_t * sizeptr = (uint64_t*)BYTE_OFFSET(mem, -sizeof(uint64_t));
    sizeptr[0] = size;
    uint64_t * alignptr = (uint64_t*)BYTE_OFFSET(sizeptr, -sizeof(uint64_t));
    alignptr[0] = alignment;
    uintptr_t ** rawptrptr = (uintptr_t**)BYTE_OFFSET(alignptr, -sizeof(void*));
    rawptrptr[0] = ptr;

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize += size+ alignment + alignment + alignment;
    allocations++;
    SDL_UnlockSpinlock(&sizeLock[0]);

    // print("size: %llu", totalAllocSize);

    return mem;
}

void * G_aligned_realloc(void * pOriginal, size_t size, size_t alignment)
{
    uint64_t * sizeptr = (uint64_t*)BYTE_OFFSET(pOriginal, -sizeof(uint64_t));
    uint64_t * alignptr = (uint64_t*)BYTE_OFFSET(sizeptr, -sizeof(uint64_t));
    uintptr_t ** rawptrptr = (uintptr_t**)BYTE_OFFSET(alignptr, -sizeof(void*));
    uint64_t oldSize = sizeptr[0];
    void * rawptr = rawptrptr[0];

    if (size == 0) 
    {
        G_aligned_free(rawptr);
        return NULL;
    }
    if (alignment < 8) alignment = 8;

#ifdef SLOW_ALIGNED_REALLOC
    size_t copySize = oldSize < size ? oldSize : size;

    uint64_t * newMem = SDL_aligned_alloc(alignment, size + alignment + alignment + alignment);
#endif

#ifdef _WIN32
    uint64_t * newMem = _aligned_realloc(rawptr, size + alignment + alignment + alignment, alignment);
#endif

    if (newMem == NULL) return NULL;

    uint64_t * mem = (uint64_t*)BYTE_OFFSET(newMem, alignment * 3);
    sizeptr = (uint64_t*)BYTE_OFFSET(mem, -sizeof(uint64_t));
    sizeptr[0] = size;
    sizeptr--;
    sizeptr[0] = alignment;

    rawptrptr = (uintptr_t**)BYTE_OFFSET(sizeptr, -sizeof(void*));
    rawptrptr[0] = newMem;

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize -= oldSize;
    totalAllocSize += size;
    SDL_UnlockSpinlock(&sizeLock[0]);

#ifdef SLOW_ALIGNED_REALLOC
    memcpy(mem, pOriginal, copySize);
    SDL_aligned_free(rawptr);
#endif

    // print("size: %llu", totalAllocSize);

    return mem;
}

void G_aligned_free(void * pMemory)
{
    if (pMemory == NULL) return;

    uint64_t * sizeptr = (uint64_t*)BYTE_OFFSET(pMemory, -sizeof(uint64_t));
    uint64_t * alignptr = (uint64_t*)BYTE_OFFSET(sizeptr, -sizeof(uint64_t));
    uintptr_t ** rawptr = (uintptr_t**)BYTE_OFFSET(alignptr, -sizeof(void*));

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize -= sizeptr[0] + 3 * alignptr[0];
    allocations--;
    SDL_UnlockSpinlock(&sizeLock[0]);

#ifdef SDL_ALIGNED_FREE
    SDL_aligned_free(rawptr[0]);
#endif

#ifdef _WIN32
    _aligned_free(rawptr[0]);
#endif

    // print("size: %llu", totalAllocSize);
}

uint64_t G_getMemSize(void * mem)
{
    return ((uint64_t*)BYTE_OFFSET(mem, -sizeof(uint64_t)))[0];
}

uint64_t G_totalAllocSize(void)
{
    return totalAllocSize;
}

uint32_t G_allocations(void)
{
    return allocations;
}
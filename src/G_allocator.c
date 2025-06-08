#include "G_allocator.h"

#include "SDL3/SDL_log.h"
#include "SDL3/SDL_atomic.h"

static Uint64 totalAllocSize = 0;
static Uint32 allocations = 0;
static SDL_SpinLock sizeLock[1];

#ifdef TRACE_PTR

#include "SDL3/SDL_mutex.h"
static SDL_Mutex * recordMutex;
static G_memptr_record * record = NULL;
void initMemoryRecord(void)
{
    recordMutex = SDL_CreateMutex();
}
static void addMemory(void * ptr, void * returnAddress0, void * returnAddress1, int line, char * file)
{
    SDL_LockMutex(recordMutex);
    G_memptr_record * s;

    HASH_FIND_PTR(record, &ptr, s);
    if (s == NULL)
    {
        s = malloc(sizeof(*s));
        s->rawptr = ptr;
        s->returnAddress0 = returnAddress0;
        s->returnAddress1 = returnAddress1;
        s->address1.line = line;
        SDL_strlcpy(s->address1.file, file, 255);
        HASH_ADD_PTR(record, rawptr, s);
    }
    SDL_UnlockMutex(recordMutex);
}
// static G_memptr_record * findMemory(void * ptr)
// {
//     G_memptr_record * s;

//     HASH_FIND_PTR(record, ptr, s);
//     return s;
// }
static void deleteMemory(void * ptr)
{
    SDL_LockMutex(recordMutex);
    G_memptr_record * s;
    HASH_FIND_PTR(record, &ptr, s);
    if (s != NULL)
    {
        HASH_DEL(record, s);
        free(s);
    }
    SDL_UnlockMutex(recordMutex);
}
void deleteRecord(void)
{
    SDL_LockMutex(recordMutex);
    G_memptr_record * current = NULL;
    G_memptr_record * tmp = NULL;

    HASH_ITER(hh, record, current, tmp)
    {
        HASH_DEL(record, current);
        free(current);
    }
    SDL_UnlockMutex(recordMutex);
    SDL_DestroyMutex(recordMutex);
}
void printResidueMemory(void)
{
    SDL_LockMutex(recordMutex);
    G_memptr_record *s, *tmp;

    HASH_ITER(hh, record, s, tmp)
    {
        SDL_Log("residue memory: 0x%p, return address0: 0x%p, return address1: 0x%p, allocations: %u\n%d, %s\n", s->rawptr, s->returnAddress0, s->returnAddress1, allocations, s->address1.line, s->address1.file);
        BREAK_POINT
        allocations--;
    }
    SDL_UnlockMutex(recordMutex);
}
#endif

// alloc size is multiple of 8
#ifdef TRACE_PTR
void * _G_malloc(size_t size, void * returnAddress, int line, char * file)
#else
void * _G_malloc(size_t size)
#endif
{
    if (size == 0) return NULL;

    uint64_t allocateSize = ROUND8(size);
    uint64_t * ptr = SDL_malloc(allocateSize + sizeof(uint64_t));

    if (ptr == NULL) return NULL;

#ifdef TRACE_PTR
    addMemory(ptr, returnAddress, __builtin_return_address(0), line, file);
#endif
 
    ptr[0] = allocateSize;
    ptr++;

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize += allocateSize + sizeof(uint64_t);
    allocations++;
    SDL_UnlockSpinlock(&sizeLock[0]);

   // print("size: %llu", totalAllocSize);

    return ptr;
}

#ifdef TRACE_PTR
void * _G_calloc(size_t n_elements, size_t elem_size, void * returnAddress, int line, char * file)
#else
void * _G_calloc(size_t n_elements, size_t elem_size)
#endif
{
    if (!n_elements | !elem_size) return NULL;

    uint64_t req = ROUND8(n_elements * elem_size);
    uint64_t * ptr = SDL_calloc(1, req + sizeof(uint64_t));

    if (ptr == NULL) return NULL;

#ifdef TRACE_PTR
    addMemory(ptr, returnAddress, __builtin_return_address(0), line, file);
#endif
 
    ptr[0] = req;
    ptr++;

    SDL_LockSpinlock(&sizeLock[0]);
    totalAllocSize += req + sizeof(uint64_t);
    allocations++;
    SDL_UnlockSpinlock(&sizeLock[0]);

    // print("size: %llu", totalAllocSize);

    return ptr;
}

#ifdef TRACE_PTR
void * _G_realloc(void * pOriginal, size_t size, void * returnAddress, int line, char * file)
#else
void * _G_realloc(void * pOriginal, size_t size)
#endif
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

#ifdef TRACE_PTR
    deleteMemory(oldMem);
#endif

    uint64_t originalSize = oldMem[0];
    uint64_t allocateSize = ROUND8(size);
    uint64_t * ptr = SDL_realloc(oldMem, allocateSize + sizeof(uint64_t));

    if (ptr == NULL) return NULL;

#ifdef TRACE_PTR
    addMemory(ptr, returnAddress, __builtin_return_address(0), line, file);
#endif
 
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

#ifdef TRACE_PTR
    deleteMemory(ptr);
#endif
 
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

#ifdef TRACE_PTR
void * _G_aligned_alloc(size_t alignment, size_t size, void * returnAddress, int line, char * file)
#else
void * _G_aligned_alloc(size_t alignment, size_t size)
#endif
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

#ifdef TRACE_PTR
    addMemory(ptr, returnAddress, __builtin_return_address(0), line, file);
#endif

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

#ifdef TRACE_PTR
void * _G_aligned_realloc(void * pOriginal, size_t size, size_t alignment, void * returnAddress, int line, char * file)
#else
void * _G_aligned_realloc(void * pOriginal, size_t size, size_t alignment)
#endif
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

#ifdef TRACE_PTR
    deleteMemory(rawptr);
#endif

#ifdef SLOW_ALIGNED_REALLOC
    size_t copySize = oldSize < size ? oldSize : size;

    uint64_t * newMem = SDL_aligned_alloc(alignment, size + alignment + alignment + alignment);
#endif

#ifdef _WIN32
    uint64_t * newMem = _aligned_realloc(rawptr, size + alignment + alignment + alignment, alignment);
#endif

    if (newMem == NULL) return NULL;

#ifdef TRACE_PTR
    addMemory(newMem, returnAddress, __builtin_return_address(0), line, file);
#endif

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
    G_aligned_free(rawptr);
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

#ifdef TRACE_PTR
    deleteMemory(rawptr[0]);
#endif

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

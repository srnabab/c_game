#include "SDL3/SDL_stdinc.h"

#ifndef G_ALLOCATOR_H
#define G_ALLOCATOR_H 1

#define BYTE_OFFSET(ptr, offset) (((char*)(ptr) + (offset)))
#define ROUND8(X) (((X)+7)&~7)

struct _G_aligned
{
    uint64_t * alignment;
    void * mem;
};
typedef struct _G_aligned G_aligned;

#ifdef TRACE_PTR
struct _G_memptr_record
{
    void * rawptr;
    bool freed;
    char align[7];
};
typedef struct _G_memptr_record G_memptr_record;
#endif

#include "SDL3/SDL_begin_code.h"

extern void * SDLCALL G_malloc(size_t size);
extern void * SDLCALL G_calloc(size_t n_elements, size_t elem_size);
extern void * SDLCALL G_realloc(void * pOriginal, size_t size);
extern void SDLCALL G_free(void * pMemory);
extern void * SDLCALL G_aligned_alloc(size_t alignment, size_t size);
extern void * SDLCALL G_aligned_realloc(void * pOriginal, size_t size, size_t alignment);
extern void SDLCALL G_aligned_free(void * pMemory);
extern uint64_t SDLCALL G_getMemSize(void * mem);
extern uint64_t SDLCALL G_totalAllocSize(void);
extern uint32_t SDLCALL G_allocations(void);

#include "SDL3/SDL_close_code.h"

#endif // G_allocator.h
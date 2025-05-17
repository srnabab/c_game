#include "G_constants.h"
#include "SDL3/SDL_stdinc.h"

#ifndef G_ALLOCATOR_H
#define G_ALLOCATOR_H 1

#define BYTE_OFFSET(ptr, offset) (((char*)(ptr) + (offset)))
#define ROUND8(X) (((X)+7)&~7)

typedef struct _G_aligned G_aligned;

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL G_free(void * pMemory);
extern void SDLCALL G_aligned_free(void * pMemory);
extern uint64_t SDLCALL G_getMemSize(void * mem);
extern uint64_t SDLCALL G_totalAllocSize(void);
extern uint32_t SDLCALL G_allocations(void);

#ifdef TRACE_PTR

extern void * SDLCALL _G_malloc(size_t size, void * returnAddress, int line, char * file);
extern void * SDLCALL _G_calloc(size_t n_elements, size_t elem_size, void * returnAddress, int line, char * file);
extern void * SDLCALL _G_realloc(void * pOriginal, size_t size, void * returnAddress, int line, char * file);
extern void * SDLCALL _G_aligned_alloc(size_t alignment, size_t size, void * returnAddress, int line, char * file);
extern void * SDLCALL _G_aligned_realloc(void * pOriginal, size_t size, size_t alignment, void * returnAddress, int line, char * file);


#define G_malloc(size) _G_malloc(size, __builtin_return_address(0), __LINE__, __FILE__)
#define G_calloc(n_elements, elem_size) _G_calloc(n_elements, elem_size, __builtin_return_address(0), __LINE__, __FILE__)
#define G_realloc(pOrigninal, size) _G_realloc(pOrigninal, size, __builtin_return_address(0), __LINE__, __FILE__)
#define G_aligned_alloc(alignment, size) _G_aligned_alloc(alignment, size, __builtin_return_address(0), __LINE__, __FILE__)
#define G_aligned_realloc(pOriginal, alignment, size) _G_aligned_realloc(pOriginal, alignment, size, __builtin_return_address(0), __LINE__, __FILE__)

#include "uthash/uthash.h"
struct _G_line_file
{
    int line;
    char file[256];
};
typedef struct _G_line_file G_line_file;

struct _G_memptr_record
{
    void * rawptr;
    void * returnAddress0;
    void * returnAddress1;
    G_line_file address1;
    UT_hash_handle hh;
};
typedef struct _G_memptr_record G_memptr_record;
extern void SDLCALL initMemoryRecord(void);
extern void SDLCALL printResidueMemory(void);
extern void SDLCALL deleteRecord(void);
#else
extern void * SDLCALL _G_malloc(size_t size);
extern void * SDLCALL _G_calloc(size_t n_elements, size_t elem_size);
extern void * SDLCALL _G_realloc(void * pOriginal, size_t size);
extern void SDLCALL G_free(void * pMemory);
extern void * SDLCALL _G_aligned_alloc(size_t alignment, size_t size);
extern void * SDLCALL _G_aligned_realloc(void * pOriginal, size_t size, size_t alignment);

#define G_malloc(size) _G_malloc(size)
#define G_calloc(n_elements, elem_size) _G_calloc(n_elements, elem_size)
#define G_realloc(pOrigninal, size) _G_realloc(pOrigninal, size)
#define G_aligned_alloc(alignment, size) _G_aligned_alloc(alignment, size)
#define G_aligned_realloc(pOriginal, alignment, size) _G_aligned_realloc(pOriginal, alignment, size)
#endif

#include "SDL3/SDL_close_code.h"

#endif // G_allocator.h
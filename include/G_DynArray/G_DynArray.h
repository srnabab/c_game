#include "SDL3/SDL_stdinc.h"

#ifndef G_DYNARRAY_H
#define G_DYNARRAY_H

#include "SDL3/SDL_begin_code.h"

typedef bool (*G_DynArrayAddFunc)(void * pData, G_DynArray * pArray);
typedef bool (*G_DynArrayChangeFunc)(void * pData, Uint32 index, G_DynArray * pArray);
typedef void * (*G_DynArrayGetFunc)(Uint32 index, G_DynArray * pArray);

struct _G_DynArray
{
    void * pData;
    Uint32 size;
    Uint32 capacity;
    Uint32 elementSize;
    G_DynArrayAddFunc add;
    G_DynArrayChangeFunc change;
    G_DynArrayGetFunc get;
};
typedef struct _G_DynArray G_DynArray;

bool G_InitDynArray(Uint32 elementSize, Uint32 capacity, G_DynArray * pArray);
void G_DeInitDynArray(G_DynArray * pArray);

#include "SDL3/SDL_close_code.h"

#endif // G_DYNARRAY_H
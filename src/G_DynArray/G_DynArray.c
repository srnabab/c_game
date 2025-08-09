#include "G_DynArray/G_DynArray.h"

bool G_InitDynArray(Uint32 elementSize, Uint32 capacity, G_DynArray * pArray)
{
    pArray->pData = SDL_malloc(elementSize * capacity);
    if (pArray->pData == NULL) return false;

    pArray->size = 0;
    pArray->capacity = capacity;
    pArray->elementSize = elementSize;
    pArray->add = G_AddDynArray;
    pArray->change = G_ChangeDynArray;
    pArray->get = G_GetDynArray;

    return true;
}
static bool G_ResizeDynArray(G_DynArray * pArray)
{
    pArray->pData = SDL_realloc(pArray->pData, pArray->elementSize * pArray->capacity * 2);
    if (pArray->pData == NULL) return false;

    pArray->capacity *= 2;

    return true;
}
static bool G_AddDynArray(void * pData, G_DynArray * pArray)
{
    if (pArray->size == pArray->capacity)
    if (!G_ResizeDynArray(pArray)) return false;

    memcpy((Uint8*)pArray->pData + pArray->size * pArray->elementSize, pData, pArray->elementSize);
    pArray->size++;

    return true;
}
static bool G_ChangeDynArray(void * pData, Uint32 index, G_DynArray * pArray)
{
    if (index >= pArray->size) return false;

    memcpy((Uint8*)pArray->pData + index * pArray->elementSize, pData, pArray->elementSize);

    return true;
}
static void * G_GetDynArray(Uint32 index, G_DynArray * pArray)
{
    if (index >= pArray->size) return NULL;

    return (void*)((Uint8*)pArray->pData + index * pArray->elementSize);
}
void G_DeInitDynArray(G_DynArray * pArray)
{
    SDL_free(pArray->pData);
    pArray->pData = NULL;
    pArray->size = 0;
    pArray->capacity = 0;
    pArray->elementSize = 0;
}
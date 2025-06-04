#include "G_allocator.h"
#include "sqlite3/sqlite3_alloc_func.h"

void * SDL_SQLite_malloc(int size)
{
    return G_malloc(size);
}
void SDL_SQLite_free(void * mem)
{
    G_free(mem);
}
void * SDL_SQLite_realloc(void * pOriginal, int size)
{
    return G_realloc(pOriginal, size);
}
int SDL_SQLite_memSize(void * mem)
{
    return (int)G_getMemSize(mem);
}
int SDL_SQLite_RoundUp(int size)
{
    return ROUND8(size);
}
void SDL_SQLite_shutDown(void * arg)
{
    return;
}
int SDL_SQLite_Init(void * arg)
{
    return SQLITE_OK;
}
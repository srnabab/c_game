#include "sqlite3/sqlite3.h"

#ifndef SQLITE3_ALLOC_FUNC_H
#define SQLITE3_ALLOC_FUNC_H

#define ROUND8(X) (((X)+7)&~7)
#define ALIGNMENT 8

void * SDL_SQLite_malloc(int size);
void SDL_SQLite_free(void *mem);
void * SDL_SQLite_realloc(void *mem, int size);
int SDL_SQLite_memSize(void * mem);
int SDL_SQLite_RoundUp(int size);
void SDL_SQLite_shutDown(void *);
int SDL_SQLite_Init(void *);

#endif
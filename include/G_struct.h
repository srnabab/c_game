#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_mutex.h"

#ifndef STRUCT_H
#define STRUCT_H 1

#include "SDL3/SDL_begin_code.h"

typedef struct _ThreadSem
{
    SDL_Thread * thread;
    SDL_Semaphore * semaphore;
} ThreadSem;

#include "SDL3/SDL_close_code.h"

#endif
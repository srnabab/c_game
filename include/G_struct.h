#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_mutex.h"

#ifndef STRUCT_H
#define STRUCT_H 1

#include "SDL3/SDL_begin_code.h"

struct _G_SYNC
{
    // for input
    SDL_Mutex * inputMutex;
    // for update
    SDL_Mutex * updateMutex;
    // for render
    SDL_Mutex * renderMutex;
    // for log putMessage
    SDL_Mutex * logMutex;
    // for log printMessage
    SDL_Mutex * printMutex;
    // for pop window
    SDL_Mutex * popWindowMutex;
    // for texture
    SDL_Mutex * textureMutex;
    // for timer
    SDL_Mutex * timerMutex;
    // for descriptor update
    SDL_Mutex * descriptorUpdateMutex;
    // for vertex data set
    SDL_Mutex * vertexMutex;
    // for tile set load
    SDL_Mutex * tileSetMutex;

    // for update sync
    SDL_Semaphore * updateSemaphore;
    // for render sync
    SDL_Semaphore * renderSemaphore;
    // for vertex cpoy
    SDL_Semaphore * vertexSemaphore;
    // for main cycle sync
    SDL_Semaphore * signalSemaphore;
    // for log putMessage sync
    SDL_Semaphore * logSemaphore;
    // for physical process sync
    SDL_Semaphore * worldSemaphore;
};
typedef struct _G_SYNC G_SYNC;

#include "SDL3/SDL_close_code.h"

#endif
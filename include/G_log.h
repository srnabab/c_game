#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_log.h"

#ifndef LOG_H
#define LOG_H 1

#include "SDL3/SDL_begin_code.h"

#define DEBUG_PRINT 1

#define LOG_ENABLED 1
#define LOG_TXT 2

extern bool SDLCALL initLog(Uint8 log);
extern void SDLCALL logMessage(char * format, ...);
extern void SDLCALL destroyLog(void);

#if defined(DEBUG_PRINT)
# define print SDL_Log
#else 
# define print logMessage
#endif

#include "SDL3/SDL_close_code.h"

#endif
#include "SDL3/SDL_stdinc.h"

#ifndef LOG_H
#define LOG_H 1

#include "SDL3/SDL_begin_code.h"


#define LOG_ENABLED 1
#define LOG_TXT 2

extern void SDLCALL initLog(Uint8 log);
extern void SDLCALL logMessage(char * format, ...);
extern void SDLCALL destroyLog(void);

#ifdef LOG_ENABLE
# define LogMessage(fmt, ...) logMessage(fmt, __VA_ARGS__)
#else 
# define LogMessage(fmt, ...)
#endif

#include "SDL3/SDL_close_code.h"

#endif
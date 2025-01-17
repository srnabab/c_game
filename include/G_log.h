#ifndef LOG_H
#define LOG_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL initLog(void);
extern void SDLCALL logMessage(char * format, ...);
extern void SDLCALL destroyLog(void);

#include "SDL3/SDL_close_code.h"

#endif
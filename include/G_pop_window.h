#include "SDL3/SDL_messagebox.h"

#ifndef POP_WINDOW_H
#define POP_WINDOW_H 1

#include "SDL3/SDL_begin_code.h"

extern bool SDLCALL initPopWindow(void);
extern void SDLCALL pushMessage(SDL_MessageBoxFlags flags, const char * title, char *fmt, ...);
extern bool SDLCALL willPopWindow(void);
extern void SDLCALL popWindow(void);
extern bool SDLCALL deInitPopWindow(void);

#include "SDL3/SDL_close_code.h"

#endif
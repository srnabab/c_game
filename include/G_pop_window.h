#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_messagebox.h"

#ifndef POP_WINDOW_H
#define POP_WINDOW_H 1

bool initPopWindow(void);
void pushMessage(SDL_MessageBoxFlags flags, const char * title, char *fmt, ...);
bool willPopWindow(void);
void popWindow(void);
bool deInitPopWindow(void);

#endif
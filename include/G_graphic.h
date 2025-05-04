#include "SDL3/SDL_stdinc.h"

#ifndef G_GRAPHIC_H
#define G_GRAPHIC_H 1

#include "SDL3/SDL_begin_code.h"

//base function code to clean up resources created
extern void SDLCALL cleanVulkan(void);
extern bool SDLCALL initWindow_3D(void);
extern void SDLCALL initVulkan(void);

#include "SDL3/SDL_close_code.h"

#endif //G_graphic.h
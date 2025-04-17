#include "SDL3/SDL_stdinc.h"

#ifndef G_MAP_H
#define G_MAP_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL setMapBottom(Uint32 width, Uint32 height, int centerX, int centerY, Uint32 * pRowCount, Uint32 * pColumnCount, int * pFirstBottom_X, int * pFirstBottom_Y, int * pBaseX, int * pBaseY, int32_t * pFirstBottomID);

#include "SDL3/SDL_close_code.h"

#endif // G_map.h
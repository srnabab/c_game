#include "SDL3/SDL_stdinc.h"

#ifndef TEXT_H
#define TEXT_H 1

#include "SDL3/SDL_begin_code.h"

#define MAX_CHARACTERS 100
#define FOUR_POINT 4

bool initTextSystem(void);
void getTextUV(char * text, uint32_t * textLen);

#include "SDL3/SDL_close_code.h"

#endif
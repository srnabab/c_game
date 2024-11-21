#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_iostream.h"

#ifndef SDL_COMPLEMENT_H
#define SDL_COMPLEMENT_H

size_t SDL_strcspn(const char *str1, const char *str2);
char *SDL_IOgets(char *__restrict_arr _Buf, int _MaxCount, SDL_IOStream *__restrict_arr _IoStream);

#endif
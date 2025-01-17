#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_iostream.h"

#ifndef SDL_COMPLEMENT_H
#define SDL_COMPLEMENT_H 1

#include "SDL3/SDL_begin_code.h"

size_t SDL_strcspn(const char *str1, const char *str2);
char *SDL_IOgets(char *__restrict_arr _Buf, int _MaxCount, SDL_IOStream *__restrict_arr _IoStream);

#include "SDL3/SDL_close_code.h"

#endif
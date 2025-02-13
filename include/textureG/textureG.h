#include "SDL3/SDL_stdinc.h"
#include "cglm/vec2.h"

#ifndef TEXTUREG_H
#define TEXTUREG_H 1

#include "SDL3/SDL_begin_code.h"

#define MAX_2D_SIZE 16384

#define HASH_SIZE 65537
#define MAX_KICKS 128

typedef struct _HASH
{
    vec2 uv[4];
    Uint32 utf32;
} Hash;

extern SDL_DECLSPEC SDL_NODISCARD Uint32 SDLCALL hash_func1(Uint32 utf32);
extern SDL_DECLSPEC SDL_NODISCARD Uint32 SDLCALL hash_func2(Uint32 utf32);

extern SDL_DECLSPEC SDL_NODISCARD int SDLCALL find_value(Hash * hash, Uint32 utf32);
extern SDL_DECLSPEC int SDLCALL textureGenerate(const char* fontPath, const char* hashTablePath, const char* pngSavePath, Sint8 channels, int fontSize, int* failed);

#include "SDL3/SDL_close_code.h"

#endif
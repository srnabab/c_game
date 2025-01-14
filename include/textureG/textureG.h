#include "begin_code.h"
#include <cglm/vec2.h>

#ifndef TEXTUREG_H
#define TEXTUREG_H

#define MAX_2D_SIZE 16383

#define HASH_SIZE 65537
#define MAX_KICKS 128

typedef struct _HASH
{
    vec2 uv[4];
    uint32_t utf32;
} Hash;

extern G_DECLSPEC G_NODISCARD uint32_t G_CALL hash_func1(uint32_t utf32);
extern G_DECLSPEC G_NODISCARD uint32_t G_CALL hash_func2(uint32_t utf32);

extern G_DECLSPEC G_NODISCARD int G_CALL find_value(Hash * hash, uint32_t utf32);
extern G_DECLSPEC int G_CALL textureGenerate(const char* fontPath, const char* hashTablePath, const char* pngSavePath, int8_t channels, int fontSize, int* failed);
/*unused*/
// static long getFileSize(FILE * fp)
// {
//     fseek(fp, 0, SEEK_END);
//     long size = ftell(fp);
//     rewind(fp);

//     return size;
// }

#endif
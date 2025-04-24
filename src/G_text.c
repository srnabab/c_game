#include "textureG.h"

#include "G_text.h"
#include "G_log.h"
#include "G_file/G_file.h"

#include "SDL3/SDL_iostream.h"
#include <unistr.h>

static TextHash textHash[HASH_SIZE * 2];

bool initTextSystem(void)
{
    SDL_IOStream * fp;
    int failed = 0;

    if ((fp = SDL_IOFromFile(getPath(MainFontHashTable), "rb")) == NULL)
    {
        textureGenerate(getPath(MainFont), getPath(MainFontHashTable), getPath(MainFontPng), 1, 72, &failed);
        if (failed)
        {
            print("font error(%d)", failed);
        }
        fp = SDL_IOFromFile(getPath(MainFontHashTable), "rb");
        if (fp == NULL)
        {
            return false;
        }
    }

    SDL_ReadIO(fp, textHash, sizeof(TextHash) * HASH_SIZE * 2);
    SDL_CloseIO(fp);
    
    if ((fp = SDL_IOFromFile(getPath(EmojiHashTable), "rb")) == NULL)
    {
        int code = textureGenerate(getPath(EmojiFont), getPath(EmojiHashTable), getPath(EmojiPng), 4, 132, &failed);
        print("%d\n", code);
        if (failed)
        {
            print("font error(%d)", failed);
        }
        fp = SDL_IOFromFile(getPath(EmojiHashTable), "rb");
        if (fp == NULL)
        {
            return false;
        }
    }
    SDL_CloseIO(fp);
    // for (int x = 0;x < 2;x++)
    // for (int i = 0;i < 5000;i++)
    // {
    //     print("uv: %f", hash[x][i].uv[0][0]);
    // }

   return true;
}

vec2 UVs[MAX_CHARACTERS][FOUR_POINT];

void getTextUV(char * text, uint32_t * textLen)
{
    size_t u8Len = SDL_strlen(text);
    unistring_uint32_t * u32Result;
    size_t u32Len;
    u32Result = u8_to_u32((unistring_uint8_t*)text, u8Len, NULL, &u32Len);
    memset(UVs, 0, sizeof(UVs));
    for (size_t i = 0;i < u32Len;i++)
    {
        int index = find_value(textHash, u32Result[i]);
        print("U+%X, index: %d", u32Result[i], index);
        if (index != -1)
        {
            for (int x = 0;x < 4;x++)
            {
                UVs[i][x][0] = textHash[index].uv[x][0];
                UVs[i][x][1] = textHash[index].uv[x][1];
                print("u: %f, v: %f", UVs[i][x][0], UVs[i][x][1]);
            }
        }
    }
    *textLen = u32Len;
}
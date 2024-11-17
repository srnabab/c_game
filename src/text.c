#include "text.h"
#include "log.h"
#include "file.h"
#include "textureG.h"
#include <unistr.h>

static Hash textHash[HASH_SIZE * 2];

bool initTextSystem(void)
{
    FILE * fp;
    int failed = 0;

    if ((fp = fopen(getPath(TextHashTable), "rb")) == NULL)
    {
        textureGenerate(getPath(TextFont), getPath(TextHashTable), getPath(TextPng), 1, 60, &failed);
        if (failed)
        {
            logMessage("font error(%d)", failed);
        }
        fp = fopen(getPath(TextHashTable), "rb");
        if (fp == NULL)
        {
            return false;
        }
    }

    fread(textHash, sizeof(Hash), HASH_SIZE * 2, fp);
    fclose(fp);
    
    if ((fp = fopen(getPath(EmojiHashTable), "rb")) == NULL)
    {
        int code = textureGenerate(getPath(EmojiFont), getPath(EmojiHashTable), getPath(EmojiPng), 4, 132, &failed);
        printf("%d\n", code);
        if (failed)
        {
            logMessage("font error(%d)", failed);
        }
        fp = fopen(getPath(EmojiHashTable), "rb");
        if (fp == NULL)
        {
            return false;
        }
    }
    fclose(fp);
    // for (int x = 0;x < 2;x++)
    // for (int i = 0;i < 5000;i++)
    // {
    //     logMessage("uv: %f", hash[x][i].uv[0][0]);
    // }

   return true;
}

vec2 UVs[MAX_CHARACTERS][FOUR_POINT];

void getTextUV(char * text, uint32_t * textLen)
{
    size_t u8Len = strlen(text);
    unistring_uint32_t * u32Result;
    size_t u32Len;
    u32Result = u8_to_u32((unistring_uint8_t*)text, u8Len, NULL, &u32Len);
    memset(UVs, 0, sizeof(UVs));
    for (size_t i = 0;i < u32Len;i++)
    {
        int index = find_value(textHash, u32Result[i]);
        logMessage("U+%X, index: %d", u32Result[i], index);
        if (index != -1)
        {
            for (int x = 0;x < 4;x++)
            {
                UVs[i][x][0] = textHash[index].uv[x][0];
                UVs[i][x][1] = textHash[index].uv[x][1];
                logMessage("u: %f, v: %f", UVs[i][x][0], UVs[i][x][1]);
            }
        }
    }
    *textLen = u32Len;
}
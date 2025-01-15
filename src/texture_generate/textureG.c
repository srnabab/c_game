#include <ft2build.h>
#include <freetype/freetype.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_STATIC
#include "textureG/stb_image_write.h"

#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_iostream.h"

#include "textureG/textureG.h"

uint32_t hash_func1(uint32_t utf32)
{
    return (utf32 * 2654435761U) % HASH_SIZE;
}

uint32_t hash_func2(uint32_t utf32)
{
    return (utf32 * 3344921057U) % HASH_SIZE;
}

static bool insert(Hash * hash, uint32_t utf32)
{
    int pos1 = hash_func1(utf32);
    if (hash[pos1].utf32 == 0) 
    {  // 空位直接插入
        hash[pos1].utf32 = utf32;
        return true;
    }

    int pos2 = hash_func2(utf32) + HASH_SIZE;
    if (hash[pos2].utf32 == 0) 
    {  // 第二个表也检查
        hash[pos2].utf32 = utf32;
        return true;
    }

    // 否则，开始踢出过程
    for (int i = 0; i < MAX_KICKS; i++) 
    {
        int temp = hash[pos1].utf32;
        hash[pos1].utf32 = utf32;
        utf32 = temp;  // 被踢出的元素
        
        pos2 = hash_func2(utf32) + HASH_SIZE;
        if (hash[pos2].utf32 == 0) 
        {
            hash[pos2].utf32 = utf32;
            return true;
        }
        
        // 再次踢出
        temp = hash[pos2].utf32;
        hash[pos2].utf32 = utf32;
        utf32 = temp;
        
        pos1 = hash_func1(utf32);
        if (hash[pos1].utf32 == 0) 
        {
            hash[pos1].utf32 = utf32;
            return true;
        }
    }
    return false;
}

static bool look_up(Hash * hash, uint32_t utf32)
{
    return (hash[hash_func1(utf32)].utf32 == utf32) | (hash[hash_func2(utf32) + HASH_SIZE].utf32 == utf32);
}

int find_value(Hash * hash, uint32_t utf32)
{
    uint32_t pos1 = hash_func1(utf32);

    if (hash[pos1].utf32 == utf32)
    {
        return pos1;
    }
    
    uint32_t pos2 = hash_func2(utf32) + HASH_SIZE;
    if (hash[pos2].utf32 == utf32)
    {
        return pos2;
    }
    else
    {
        return -1;
    }
}
static void SDL_stb_image_write_func(void * context, void * data, int size)
{
    SDL_WriteIO((SDL_IOStream*)context, data, (size_t)size);
}
int textureGenerate(const char* fontPath, const char* hashTablePath, const char* pngSavePath, int8_t channels, int fontSize, int* failed)
{
    if ((fontPath == NULL) || (hashTablePath == NULL) || (pngSavePath == NULL) || (fontSize <= 0))
    {
        return -1;
    }

    Hash * hash = (Hash*)SDL_calloc(2 * HASH_SIZE, sizeof(Hash));

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) 
    {
        // logMessage("Could not init FreeType Library");
        return -2;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath, 0, &face)) 
    {
        // logMessage("Failed to load font");
        return -3;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    FT_UInt indexCount = 0;
    FT_ULong charCode = FT_Get_First_Char(face, &indexCount);
    int maxCode = 1;
    FT_ULong * codeSets = (FT_ULong*)SDL_malloc(maxCode * sizeof(FT_ULong));
    int count = 0;
    while (indexCount)
    {
        if (count == maxCode)
        {
            maxCode <<= 1;
            codeSets = (FT_ULong*)SDL_realloc(codeSets, maxCode * sizeof(FT_ULong));
        }
        if (!look_up(hash, charCode))
        {
            // assert((insert(hash, charCode) == 1));
            insert(hash, charCode);
            codeSets[count] = charCode;
            count++;
        }
        charCode = FT_Get_Next_Char(face, charCode, &indexCount);
    }

    const int characterPerLine = MAX_2D_SIZE / fontSize;

    const int characterCount = count;
    const int line = characterCount / characterPerLine + 1;

    const int imageWidth = fontSize;
    const int pixelsPerLine = imageWidth * channels;
    const int imageHeight = fontSize;

    unsigned char * buffer = (unsigned char*)SDL_calloc(imageWidth * imageHeight * line * characterPerLine * channels, sizeof(unsigned char));

    float yOffset = 1 / (float)line;
    float xOffset = 1 / (float)characterPerLine;

    for (int i = 0;i < characterCount;i++)
    {
        FT_ULong utf32 = codeSets[i];
        int n = find_value(hash, utf32);
        
        if (n < 0)
        {
            (*failed)++;
            continue;
            // printf("less than 0(%lu)\n", utf32);
            // exit(0);
        }

        static int yStep = 0;
        static int xStep = 0;
        float xOffsets = xStep * xOffset;
        float yOffsets = yStep * yOffset;
        hash[n].uv[0][0] = xOffsets;
        hash[n].uv[0][1] = yOffsets + yOffset;
        hash[n].uv[1][0] = xOffsets + xOffset;
        hash[n].uv[1][1] = yOffsets + yOffset;
        hash[n].uv[2][0] = xOffsets + xOffset;
        hash[n].uv[2][1] = yOffsets;
        hash[n].uv[3][0] = xOffsets;
        hash[n].uv[3][1] = yOffsets;
        
        xStep = (xStep + 1) % characterPerLine;
        yStep = i / characterPerLine;

        if (FT_Load_Char(face, utf32, FT_LOAD_RENDER | FT_LOAD_COLOR)) 
        {
            // logMessage("Failed to load Glyph");
            return -7;
        }

        FT_Bitmap* bitmap = &face->glyph->bitmap;

        int xOffset = (imageWidth - bitmap->width) / 2;
        int yOffset = (imageHeight - bitmap->rows) / 2;

        FT_Set_Pixel_Sizes(face, 0, fontSize);
        for (int i = 1;(bitmap->width > fontSize) | (bitmap->rows > fontSize);i++)
        {
            // assert((printf("width: %u, %u\n", bitmap->width, fontSize)) && ((bitmap->width > fontSize) | (bitmap->rows > fontSize)));
            FT_Set_Pixel_Sizes(face, 0, fontSize - 1 * i);
            FT_Load_Char(face, charCode, FT_LOAD_RENDER | FT_LOAD_COLOR);
            xOffset = (fontSize - bitmap->width) / 2;
            yOffset = (fontSize - bitmap->rows) / 2;
        }
        
        for (int y = 0; y < bitmap->rows; y++) 
        {
            for (int x = 0; x < bitmap->width; x++) 
            {
                uint32_t bufferIndex = ((y + yOffset) * imageWidth + (x + xOffset)) * channels + i * imageWidth * imageHeight * channels;
                uint32_t bitmapBufferIndex = (y * bitmap->width + x) * channels;

                // unsigned char grayValue = bitmap->buffer[y * bitmap->width + x];
            
                if (channels == 1)
                {
                    buffer[bufferIndex] = bitmap->buffer[bitmapBufferIndex];
                }
                else if (channels == 4)
                {
                    for (int s = 0;s < channels;s++)
                    {
                        buffer[bufferIndex + (2 - s)] = bitmap->buffer[bitmapBufferIndex + s];
                    }
                    buffer[bufferIndex + 3] = bitmap->buffer[bitmapBufferIndex + 3];
                }
            }
        }
    }

    SDL_free(codeSets);
    
    unsigned char * buffer2 = (unsigned char*)SDL_calloc(imageWidth * imageHeight * line * characterPerLine * channels, sizeof(unsigned char));
    for (int y = 0;y < line;y++)
    {
        for (int i = 0;i < characterPerLine;i++)
        {
            for (int x = 0;x < imageHeight;x++)
            {
                memcpy(buffer2 + (x * imageWidth * characterPerLine * channels + i * pixelsPerLine + y * imageHeight * imageWidth * characterPerLine * channels), buffer + (imageWidth * imageHeight * i * channels + x * pixelsPerLine + y * imageHeight * imageWidth * characterPerLine * channels), sizeof(unsigned char) * pixelsPerLine);
            }
        }
    }
    SDL_free(buffer);

    SDL_IOStream * fp = NULL;
    if ((fp = SDL_IOFromFile(hashTablePath, "wb")) == NULL)
    {
        // printf("hash map not saved\n");
        return -8;
    }
    else
    {
        SDL_WriteIO(fp, hash, sizeof(Hash) * HASH_SIZE * 2);
        SDL_CloseIO(fp);
        SDL_free(hash);
    }

    // int pngRes = stbi_write_png(pngSavePath, imageWidth * characterPerLine, imageHeight * line, channels, buffer2, imageWidth * characterPerLine * channels);
    SDL_IOStream * png = SDL_IOFromFile(pngSavePath, "wb");
    int pngRes = stbi_write_png_to_func(SDL_stb_image_write_func, (void*)png, imageWidth * characterPerLine, imageHeight * line, channels, buffer2, imageWidth * characterPerLine * channels);
    
    if (!pngRes)
    {
        return -9;
    }

    SDL_free(buffer2);
    SDL_CloseIO(png);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return 1;
}
#include "G_TileMap/G_TileSet.h"
#include "G_log.h"
#include "G_struct.h"

#include "SDL3/SDL_iostream.h"

static TILE_SET * tileSets = NULL;
static Uint32 tileSetCount = 0;
static Uint32 tileCap = 0;

extern G_SYNC allSync;

void initTileMapSystem(void)
{
    tileCap++;
    tileSets = SDL_calloc(tileCap, sizeof(TILE_SET));
}
static unsigned char* readTileSetData(PathType path, Uint32 * pTileWidth, Uint32 * pTileHeight, Uint32 * pTilePropertyCount, Uint32 * pTileCount, Uint32 * pImageWidth, Uint32 * pImageHeight)
{
    SDL_IOStream * tsd = SDL_IOFromFile(getPath(path), "rb");
    if (tsd == NULL)
    {
        logMessage("%s open failed", getPath(path));
        *pTileWidth = *pTileHeight = *pTilePropertyCount = *pTileCount = 0;

        return NULL;
    }

    Uint32 tileWidth, tileHeight, tilePropertyCount, tileCount, imageWidth, imageHeight;
    SDL_ReadIO(tsd, &imageWidth, sizeof(Uint32));
    SDL_ReadIO(tsd, &imageHeight, sizeof(Uint32));
    SDL_ReadIO(tsd, &tileWidth, sizeof(Uint32));
    SDL_ReadIO(tsd, &tileHeight, sizeof(Uint32));
    SDL_ReadIO(tsd, &tilePropertyCount, sizeof(Uint32));
    SDL_ReadIO(tsd, &tileCount, sizeof(Uint32));
    Uint32 dataLen = tileCount * (sizeof(Uint32) + tilePropertyCount * sizeof(bool));

    unsigned char * data = SDL_malloc(dataLen);
    SDL_ReadIO(tsd, data, dataLen);

    Uint32 crc32;
    SDL_ReadIO(tsd, &crc32, sizeof(Uint32));
    if (crc32 != SDL_crc32(0, data, dataLen))
    {
        *pTileWidth = *pTileHeight = *pTilePropertyCount = *pTileCount = *pImageWidth = *pImageHeight;
        SDL_free(data);
        SDL_CloseIO(tsd);

        return NULL;
    }
    
    SDL_CloseIO(tsd);

    *pImageWidth = imageWidth;
    *pImageHeight = imageHeight;
    *pTileWidth = tileWidth;
    *pTileHeight = tileHeight;
    *pTilePropertyCount = tilePropertyCount;
    *pTileCount = tileCount;

    return data;
}
static TILE_SET * loadTileSetData(PathType setDataPath, const char * innerName)
{
    Uint32 i, k, tilePropertyCount, imageWidth, imageHeight;

    SDL_LockMutex(allSync.tileSetMutex);
    if (tileCap == tileSetCount)
    {
        tileCap++;
        tileSets = (TILE_SET*)SDL_realloc(tileSets, tileCap * sizeof(TILE_SET));
        if (tileSets == NULL) goto unlockMutex;
    }
    unsigned char * data = readTileSetData(setDataPath, &tileSets[tileSetCount].tileWidth, &tileSets[tileSetCount].tileHeight, &tilePropertyCount, &tileSets[tileSetCount].tileCount, &imageWidth, &imageHeight);
    if (data == NULL) goto freeData;

    float xOffset = tileSets[tileSetCount].tileWidth / (float)imageWidth;
    float yOffset = tileSets[tileSetCount].tileHeight / (float)imageHeight;
    Uint32 row = imageWidth / tileSets[tileSetCount].tileWidth;
    Uint32 col = imageHeight / tileSets[tileSetCount].tileHeight;

    tileSets[tileSetCount].tileUV = (vec2**)SDL_calloc(tileSets[tileSetCount].tileCount, sizeof(vec2*));
    if (tileSets[tileSetCount].tileUV == NULL) goto freeData;

    tileSets[tileSetCount].properties = (TILE_PROPERTY*)SDL_calloc(tileSets[tileSetCount].tileCount, sizeof(TILE_PROPERTY));
    if (tileSets[tileSetCount].properties == NULL) goto freeUV;

    for (i = 0;i < row;i++)
    for (k = 0;k < col;k++)
    {
        float x = xOffset * k;
        float y = yOffset * i;
        Uint32 index = i * col + k;

        tileSets[tileSetCount].tileUV[index] = (vec2*)SDL_calloc(4, sizeof(vec2));
        if (tileSets[tileSetCount].tileUV[index] == NULL) goto freeUVP;

        tileSets[tileSetCount].tileUV[index][0][0] = x;
        tileSets[tileSetCount].tileUV[index][0][1] = y + yOffset;
        tileSets[tileSetCount].tileUV[index][1][0] = x + xOffset;
        tileSets[tileSetCount].tileUV[index][1][1] = y + yOffset;
        tileSets[tileSetCount].tileUV[index][2][0] = x + xOffset;
        tileSets[tileSetCount].tileUV[index][2][1] = y;
        tileSets[tileSetCount].tileUV[index][3][0] = x;
        tileSets[tileSetCount].tileUV[index][3][1] = y;

        tileSets[tileSetCount].properties[index].show = data[index * (sizeof(Uint32) + sizeof(bool) * tilePropertyCount) + sizeof(Uint32)];
        logMessage("index: %u, show: %u", index, tileSets[tileSetCount].properties[index].show);
    }
    goto normal;

    freeUVP:
    for (i = 0;i < tileSets[tileSetCount].tileCount;i++)
    {
        SDL_free(tileSets[tileSetCount].tileUV[i]);
    }

    SDL_free(tileSets[tileSetCount].properties);

    freeUV:
    SDL_free(tileSets[tileSetCount].tileUV);

    freeData:
    SDL_free(data);

    unlockMutex:
    SDL_UnlockMutex(allSync.tileSetMutex);
    return NULL;

    normal:
    SDL_free(data);
    SDL_strlcpy(tileSets[tileSetCount].innerName, innerName, SDL_strlen(innerName));

    tileSetCount++;
    SDL_UnlockMutex(allSync.tileSetMutex);
    return tileSets + tileSetCount - 1;
}
bool loadTileSet(PathType setImagePath, PathType setDataPath, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet)
{
    TILE_SET * pSet = loadTileSetData(setDataPath, innerName);
    if (pSet == NULL) return false;

    bool res = loadTexture(setImagePath, format, flags, innerName, pDescriptorSet);
    if (res == false) return false;

    SDL_LockMutex(allSync.tileSetMutex);
    pSet->pTexture = getTexture(innerName);
    SDL_UnlockMutex(allSync.tileSetMutex);

    return true;
}
TILE_SET * getTileSet(const char * innerName)
{
    for (Uint32 i = 0;i < tileSetCount;i++)
    {
        if (SDL_strcmp(innerName, tileSets[i].innerName) == 0) return tileSets + i;
    }

    return NULL;
}
void deInitTileMapSystem(void)
{
    for (Uint32 i = 0;i < tileSetCount;i++)
    {
        for (Uint32 j = 0;j < tileSets[i].tileCount;j++)
        {
            SDL_free(tileSets[i].tileUV[j]);
        }
        SDL_free(tileSets[i].tileUV);
        SDL_free(tileSets[i].properties);
        SDL_free(tileSets);
    }
}
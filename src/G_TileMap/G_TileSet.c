#include "G_TileMap/G_TileSet.h"
#include "G_constants.h"
#include "G_log.h"
#include "G_allocator.h"
#include "G_struct.h"

#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"

static TILE_SET * tileSets = NULL;
static Uint32 tileSetCount = 0;
static Uint32 tileCap = 0;

extern G_SYNC allSync;

bool initTileMapSystem(void)
{
    tileCap++;
    tileSets = G_calloc(tileCap, sizeof(TILE_SET));
    if (tileSets == NULL) return false;

    memset(tileSets, 0, sizeof(TILE_SET));

    return true;
}
void getTileSetCount(Uint32 * pTileSetCount)
{
    SDL_LockMutex(allSync.tileSetMutex);

    *pTileSetCount = tileSetCount;

    SDL_UnlockMutex(allSync.tileSetMutex);
}
void getTileSetPtr(TILE_SET ** ppSet)
{
    SDL_LockMutex(allSync.tileSetMutex);

    *ppSet = tileSets;

    SDL_UnlockMutex(allSync.tileSetMutex);
}
static unsigned char* readTileSetData(PathType path, Uint32 * pTileWidth, Uint32 * pTileHeight, Uint32 * pTilePropertyCount, Uint32 * pTileCount, Uint32 * pImageWidth, Uint32 * pImageHeight)
{
    SDL_IOStream * tsd = SDL_IOFromFile(getPath(path), "rb");
    if (tsd == NULL)
    {
        print("%s open failed", getPath(path));
        *pTileWidth = *pTileHeight = *pTilePropertyCount = *pTileCount = 0;

        return NULL;
    }

    TSD_Head head = {0};

    SDL_ReadIO(tsd, &head, sizeof(TSD_Head));

    if (head.format[0] != 't' && head.format[1] != 's' && head.format[2] != 'd' && head.format[3] != 99)
    {
        SDL_CloseIO(tsd);

        return NULL;
    }

    unsigned char * data = G_malloc(head.dataLen);
    if (data == NULL)
    {
        SDL_CloseIO(tsd);

        return NULL;
    }

    SDL_ReadIO(tsd, data, head.dataLen);

    // Uint32 crc32;
    // SDL_ReadIO(tsd, &crc32, sizeof(Uint32));
    // if (crc32 != SDL_crc32(0, data, dataLen))
    // {
    //     *pTileWidth = *pTileHeight = *pTilePropertyCount = *pTileCount = *pImageWidth = *pImageHeight;
    //     G_free(data);
    //     SDL_CloseIO(tsd);

    //     return NULL;
    // }
    
    SDL_CloseIO(tsd);

    *pImageWidth = head.imageWidth;
    *pImageHeight = head.imageHeight;
    *pTileWidth = head.tileWidth;
    *pTileHeight = head.tileHeight;
    *pTilePropertyCount = head.propertyCount;
    *pTileCount= (head.imageWidth / head.tileWidth) * (head.imageHeight / head.tileHeight);

    return data;
}
static TILE_SET * loadTileSetData(PathType setDataPath, const char * innerName)
{
    Uint32 i, k, tilePropertyCount, imageWidth, imageHeight;

    SDL_LockMutex(allSync.tileSetMutex);
    if (tileCap == tileSetCount)
    {
        tileCap++;
        tileSets = (TILE_SET*)G_realloc(tileSets, tileCap * sizeof(TILE_SET));
        if (tileSets == NULL)
        {
            SDL_UnlockMutex(allSync.tileSetMutex);
            return NULL;
        }
        memset(tileSets + tileSetCount, 0, sizeof(TILE_SET));
    }

    TILE_SET * tileSet = tileSets + tileSetCount;

    unsigned char * data = readTileSetData(setDataPath, &tileSet->tileWidth, &tileSet->tileHeight, &tilePropertyCount, &tileSet->tileCount, &imageWidth, &imageHeight);
    if (data == NULL)
    {
        SDL_UnlockMutex(allSync.tileSetMutex);
        return NULL;
    }

    float xOffset = tileSet->tileWidth / (float)imageWidth;
    float yOffset = tileSet->tileHeight / (float)imageHeight;
    Uint32 row = imageWidth / tileSet->tileWidth;
    Uint32 col = imageHeight / tileSet->tileHeight;

    tileSet->tiles = (TILE*)G_malloc(tileSet->tileCount * sizeof(TILE));
    if (tileSet->tiles == NULL)
    {
        G_free(data);

        SDL_UnlockMutex(allSync.tileSetMutex);

        return NULL;
    }

    for (i = 0;i < row;i++)
    for (k = 0;k < col;k++)
    {
        float x = xOffset * i;
        float y = yOffset * k;
        Uint32 index = i * col + k;

        // set ID
        memcpy(&tileSet->tiles[index].ID, data + (index * (sizeof(Uint32) + sizeof(bool) * tilePropertyCount)), sizeof(Uint32));
        memcpy(&tileSet->tiles[index].property, data + (index * (sizeof(Uint32) + sizeof(bool) * tilePropertyCount) + sizeof(Uint32)), sizeof(bool));

        tileSet->tiles[index].tileUV[0][0] = x;
        tileSet->tiles[index].tileUV[0][1] = y + yOffset;
        tileSet->tiles[index].tileUV[1][0] = x + xOffset;
        tileSet->tiles[index].tileUV[1][1] = y + yOffset;
        tileSet->tiles[index].tileUV[2][0] = x + xOffset;
        tileSet->tiles[index].tileUV[2][1] = y;
        tileSet->tiles[index].tileUV[3][0] = x;
        tileSet->tiles[index].tileUV[3][1] = y;

        print("index: %u, show: %u", index, tileSet->tiles[index].property.show);
        print("tileUV: %f, %f, %f, %f", tileSet->tiles[index].tileUV[0][0], tileSet->tiles[index].tileUV[0][1], tileSet->tiles[index].tileUV[1][0], tileSet->tiles[index].tileUV[1][1]);
        print("tileUV: %f, %f, %f, %f", tileSet->tiles[index].tileUV[2][0], tileSet->tiles[index].tileUV[2][1], tileSet->tiles[index].tileUV[3][0], tileSet->tiles[index].tileUV[3][1]);
    }

    SDL_strlcpy(tileSets[tileSetCount].innerName, innerName, SDL_strlen(innerName) + 1);

    tileSetCount++;

    G_free(data);

    SDL_UnlockMutex(allSync.tileSetMutex);

    return tileSets + tileSetCount - 1;
}
bool loadTileSet(PathType setImagePath, PathType setDataPath, VkFormat format, VkImageAspectFlags flags, const char * innerName, G_DescriptorSets * pDescriptorSet, void * pUniformBuffer)
{
    bool res = G_loadImage(setImagePath, format, flags, innerName, pDescriptorSet, 0, pUniformBuffer);
    if (res == false) return false;

    TILE_SET * pSet = loadTileSetData(setDataPath, innerName);
    if (pSet == NULL) 
    {
        unloadTexture(innerName);
        return false;
    }

    return true;
}
static TILE_SET * getTileSet(const char * innerName)
{
    for (Uint32 i = 0;i < tileSetCount;i++)
    {
        if (SDL_strcmp(innerName, tileSets[i].innerName) == 0) 
        {
            return tileSets + i;
        }
    }

    return NULL;
}
static Uint32 * loadTileMapData(PathType tileMapData, Uint32 * pRow, Uint32 * pCol, Uint32 * pGroupCount)
{
    SDL_IOStream * tsdI = SDL_IOFromFile(getPath(tileMapData), "rb");
    if (tsdI == NULL);

    TSDI_Head head = {0};

    SDL_ReadIO(tsdI, &head, sizeof(TSDI_Head));

    if (head.format[0] != 't' && head.format[1] != 's' && head.format[2] != 'd' && head.format[3] != 'I')
    {
        SDL_CloseIO(tsdI);

        return NULL;
    }

    Uint32 * data = (Uint32*)G_malloc(head.dataLen);
    if (data == NULL)
    {
        SDL_CloseIO(tsdI);

        return NULL;
    }
    SDL_ReadIO(tsdI, data, head.dataLen);

    // Uint32 crc32 = SDL_crc32(0, data, tileCount * sizeof(Uint32));
    // Uint32 crc32_check;
    // SDL_ReadIO(tsdI, &crc32_check, sizeof(Uint32));
    // if (crc32 != crc32_check) 
    // {
    //     G_free(data);
    //     SDL_CloseIO(tsdI);

    //     return NULL;
    // }

    SDL_CloseIO(tsdI);

    *pRow = head.rowCount;
    *pCol = head.colCount;
    *pGroupCount = head.groupCount;

    return data;
}
static Map_Group * getGroupPointer(Uint32 groupCount, Map_Group * mapGroups, int32_t ID)
{
    if (ID == -1) return NULL;

    for(Uint32 i = 0;i < groupCount;i++)
    {
        if (mapGroups[i].groupID == ID)
        {
            return mapGroups + i;
        }
    }

    return NULL;
}
bool loadTileMap(PathType tileMapData, const char * setInnerName, const char * mapInnerName)
{
    SDL_LockMutex(allSync.tileSetMutex);

    Uint32 i, j;
    TILE_SET * pSet = getTileSet(setInnerName);
    if (pSet == NULL)
    {
        SDL_UnlockMutex(allSync.tileSetMutex);

        return false;
    }

    Uint32 row, col, groupCount;
    Uint32 * temp = loadTileMapData(tileMapData, &row, &col, &groupCount);
    if (temp == NULL) 
    {
        SDL_UnlockMutex(allSync.tileSetMutex);

        return false;
    }

    Uint32 groupSize = row * col + 5;
    void * ptr = G_realloc(pSet->maps, (pSet->mapCount + 1) * sizeof(TILE_MAP));
    if (ptr == NULL)
    {
        G_free(temp);

        SDL_UnlockMutex(allSync.tileSetMutex);

        return false;
    }

    pSet->maps = ptr;
    pSet->maps[pSet->mapCount].rowCount = row;
    pSet->maps[pSet->mapCount].colCount = col;
    pSet->maps[pSet->mapCount].groupCount = groupCount;
    pSet->maps[pSet->mapCount].mapGroups = (Map_Group*)G_malloc(groupCount * sizeof(Map_Group));
    if (pSet->maps[pSet->mapCount].mapGroups == NULL)
    {
        G_free(temp);

        SDL_UnlockMutex(allSync.tileSetMutex);

        return false;
    }

    int32_t ** tempIDs = G_malloc(groupCount * sizeof(int32_t**));
    if (tempIDs == NULL)
    {
        G_free(temp);
        G_free(pSet->maps[pSet->mapCount].mapGroups);

        SDL_UnlockMutex(allSync.tileSetMutex);

        return false;
    }

    for (i = 0;i < groupCount;i++)
    {
        tempIDs[i] = G_malloc(4 * sizeof(int32_t));
        if (tempIDs[i] == NULL)
        {
            for (;i > -1;i--)
            {
                G_free(tempIDs[i]);
            }
            G_free(temp);
            G_free(tempIDs);
            G_free(pSet->maps[pSet->mapCount].mapGroups);

            SDL_UnlockMutex(allSync.tileSetMutex);

            return false;
        }
    }

    // int32_t minID = INT32_MAX;

    for (i = 0;i < groupCount;i++)
    {
        pSet->maps[pSet->mapCount].mapGroups[i].groupID = (int32_t)temp[i * groupSize + 0];
        tempIDs[i][0] = (int32_t)temp[i * groupSize + 1];
        tempIDs[i][1] = (int32_t)temp[i * groupSize + 2];
        tempIDs[i][2] = (int32_t)temp[i * groupSize + 3];
        tempIDs[i][3] = (int32_t)temp[i * groupSize + 4];

        // minID = pSet->maps[pSet->mapCount].mapGroups[i].groupID > minID ? minID : pSet->maps[pSet->mapCount].mapGroups[i].groupID;

        pSet->maps[pSet->mapCount].mapGroups[i].indices = (Uint32**)G_malloc(row * sizeof(Uint32*));
        if (pSet->maps[pSet->mapCount].mapGroups[i].indices == NULL)
        {
            i--;
            for (;i > -1;i--)
            {
                for (j = 0;j < row;j++)
                {
                    G_free(pSet->maps[pSet->mapCount].mapGroups[i].indices[j]);
                }
                G_free(pSet->maps[pSet->mapCount].mapGroups[i].indices);
            }

            for (i = 0;i < groupCount;i++)
            {
                G_free(tempIDs[i]);
            }
            G_free(tempIDs);
            G_free(temp);
            G_free(pSet->maps[pSet->mapCount].mapGroups);

            SDL_UnlockMutex(allSync.tileSetMutex);

            return false;
        }
        for (j = 0;j < row;j++)
        {
            pSet->maps[pSet->mapCount].mapGroups[i].indices[j] = (Uint32*)G_malloc(col * sizeof(Uint32));
            if (pSet->maps[pSet->mapCount].mapGroups[i].indices[j] == NULL)
            {
                for (;j > -1;j--)
                {
                    G_free(pSet->maps[pSet->mapCount].mapGroups[i].indices[j]);
                }
                G_free(pSet->maps[pSet->mapCount].mapGroups[i].indices);
                i--;

                for (;i > -1;i--)
                {
                    for (j = 0;j < row;j++)
                    {
                        G_free(pSet->maps[pSet->mapCount].mapGroups[i].indices[j]);
                    }
                    G_free(pSet->maps[pSet->mapCount].mapGroups[i].indices);
                }

                for (i = 0;i < groupCount;i++)
                {
                    G_free(tempIDs[i]);
                }
                G_free(tempIDs);
                G_free(temp);
                G_free(pSet->maps[pSet->mapCount].mapGroups);

                SDL_UnlockMutex(allSync.tileSetMutex);

                return false;
            }
            memcpy(pSet->maps[pSet->mapCount].mapGroups[i].indices[j], temp + (i * groupSize + 5 + col * j), col * sizeof(Uint32));
        }
    }

    G_free(temp);

    for (i = 0;i < groupCount;i++)
    {
        pSet->maps[pSet->mapCount].mapGroups[i].up = getGroupPointer(groupCount, pSet->maps[pSet->mapCount].mapGroups, tempIDs[i][0]);
        pSet->maps[pSet->mapCount].mapGroups[i].down = getGroupPointer(groupCount, pSet->maps[pSet->mapCount].mapGroups, tempIDs[i][1]);
        pSet->maps[pSet->mapCount].mapGroups[i].left = getGroupPointer(groupCount, pSet->maps[pSet->mapCount].mapGroups, tempIDs[i][2]);
        pSet->maps[pSet->mapCount].mapGroups[i].right = getGroupPointer(groupCount, pSet->maps[pSet->mapCount].mapGroups, tempIDs[i][3]);

        // print("ID: %d, upID: %d, downID: %d, leftID: %d, rightID: %d", pSet->maps[pSet->mapCount].mapGroups[i].groupID, (pSet->maps[pSet->mapCount].mapGroups[i].up == NULL) ? -1 : pSet->maps[pSet->mapCount].mapGroups[i].up->groupID,
        // (pSet->maps[pSet->mapCount].mapGroups[i].down == NULL) ? -1 : pSet->maps[pSet->mapCount].mapGroups[i].down->groupID, (pSet->maps[pSet->mapCount].mapGroups[i].left == NULL) ? -1 : pSet->maps[pSet->mapCount].mapGroups[i].left->groupID
        // , (pSet->maps[pSet->mapCount].mapGroups[i].right == NULL) ? -1 : pSet->maps[pSet->mapCount].mapGroups[i].right->groupID);

        G_free(tempIDs[i]);
    }

    G_free(tempIDs);

    SDL_strlcpy(pSet->maps[pSet->mapCount].innerName, mapInnerName, SDL_strlen(mapInnerName) + 1);

    pSet->mapCount++;
    // SDL_Log("row: %u, col: %u, x: %d, y: %d", pSet->maps[pSet->mapCount].rowCount, pSet->maps[pSet->mapCount].colCount, pSet->maps[pSet->mapCount].x, pSet->maps[pSet->mapCount].y);

    SDL_UnlockMutex(allSync.tileSetMutex);

    return true;
}
Map_Group * getFirstMapGroup(const char * setInnerName, const char * mapInnerName)
{
    Uint32 i, j;

    SDL_LockMutex(allSync.tileSetMutex);

    for (i = 0;i < tileSetCount;i++)
    {
        if (SDL_strcmp(tileSets[i].innerName, setInnerName) == 0)
        {
            for (j = 0;j < tileSets[i].mapCount;j++)
            {
                if (SDL_strcmp(tileSets[i].maps[j].innerName, mapInnerName) == 0)
                {
                    SDL_UnlockMutex(allSync.tileSetMutex);

                    return tileSets[i].maps[j].mapGroups + 0;
                }
            }

            if (j == tileSets[i].mapCount)
            {
                SDL_UnlockMutex(allSync.tileSetMutex);

                return NULL;
            }
        }
    }

    SDL_UnlockMutex(allSync.tileSetMutex);

    return NULL;
}
Map_Group * getMapGroup(const char * setInnerName, const char * mapInnerName, int32_t groupID)
{
    Uint32 i, j, k;

    SDL_LockMutex(allSync.tileSetMutex);

    for (i = 0;i < tileSetCount;i++)
    {
        if (SDL_strcmp(tileSets[i].innerName, setInnerName) == 0)
        {
            for (j = 0;j < tileSets[i].mapCount;j++)
            {
                if (SDL_strcmp(tileSets[i].maps[j].innerName, mapInnerName) == 0)
                {
                    for (k = 0;k < tileSets[i].maps[j].groupCount;k++)
                    {
                        if (tileSets[i].maps[j].mapGroups[k].groupID == groupID)
                        {
                            SDL_UnlockMutex(allSync.tileSetMutex);

                            return tileSets[i].maps[j].mapGroups + k;
                        }
                    }

                    if (k == tileSets[i].maps[j].groupCount)
                    {
                        SDL_UnlockMutex(allSync.tileSetMutex);

                        return NULL;
                    }
                }
            }

            if (j == tileSets[i].mapCount)
            {
                SDL_UnlockMutex(allSync.tileSetMutex);

                return NULL;
            }
        }
    }

    SDL_UnlockMutex(allSync.tileSetMutex);

    return NULL;
}
Map_Group * mapGroupToRight(Map_Group * mapGroup, int32_t moveCount)
{
    if (mapGroup == NULL) return NULL;

    if (moveCount) return mapGroupToRight(mapGroup->right, moveCount - 1);
    else return mapGroup;
}
Map_Group * mapGroupToDown(Map_Group * mapGroup, int32_t moveCount)
{
    if (mapGroup == NULL) return NULL;

    if (moveCount) return mapGroupToDown(mapGroup->down, moveCount - 1);
    else return mapGroup;
}
void setTilemapUVs(Map_Group * group, vec2 * pUVs, Uint32 index)
{
    Uint32 startIndex = MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D * index;
    Uint32 i, j;

    for (i = 0;i < tileSets[0].maps[0].rowCount;i++)
    {
        for (j = 0;j < tileSets[0].maps[0].colCount;j++)
        {
            Uint32 UVindex = (i * tileSets[0].maps[0].colCount + j) * 4;
            Uint32 j2 = j;
            Uint32 i2 = i;

            pUVs[startIndex + UVindex][0] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[0][0];
            pUVs[startIndex + UVindex][1] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[0][1];

            // print("UVindex: %u, index: %u, UV: %f, %f", UVindex, group->indices[i][j], pUVs[startIndex + UVindex][0], pUVs[startIndex + UVindex][1]);

            UVindex++;
            pUVs[startIndex + UVindex][0] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[1][0];
            pUVs[startIndex + UVindex][1] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[1][1];

            // print("UVindex: %u, index: %u, UV: %f, %f", UVindex, group->indices[i][j], pUVs[startIndex + UVindex][0], pUVs[startIndex + UVindex][1]);

            UVindex++;
            pUVs[startIndex + UVindex][0] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[2][0];
            pUVs[startIndex + UVindex][1] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[2][1];

            // print("UVindex: %u, index: %u, UV: %f, %f", UVindex, group->indices[i][j], pUVs[startIndex + UVindex][0], pUVs[startIndex + UVindex][1]);

            UVindex++;
            pUVs[startIndex + UVindex][0] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[3][0];
            pUVs[startIndex + UVindex][1] = tileSets[0].tiles[group->indices[i2][j2]].tileUV[3][1];

            // print("tile index: %u, index: %u, UV: %f, %f", i * tileSets[0].maps[0].colCount + j, group->indices[i][j], pUVs[startIndex + UVindex][0], pUVs[startIndex + UVindex][1]);
        }

    }

    // print("end");
}
void deInitTileMapSystem(void)
{
    for (Uint32 i = 0;i < tileSetCount;i++)
    {
        G_free(tileSets[i].tiles);
        for (Uint32 j = 0;j < tileSets[i].mapCount;j++)
        {
            for (Uint32 k = 0;k < tileSets[i].maps[j].groupCount;k++)
            {
                for (Uint32 l = 0;l < tileSets[i].maps[j].rowCount;l++)
                {
                    G_free(tileSets[i].maps[j].mapGroups[k].indices[l]);
                }
                G_free(tileSets[i].maps[j].mapGroups[k].indices);
            }
            G_free(tileSets[i].maps[j].mapGroups);
        }
        G_free(tileSets[i].maps);
    }
    G_free(tileSets);
}

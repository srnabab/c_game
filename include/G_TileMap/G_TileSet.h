#include "cglm/cglm.h"
#include "SDL3/SDL_stdinc.h"
#include "G_file/G_file.h"
#include "G_resource.h"

#ifndef G_TILE_SET_H
#define G_TILE_SET_H 1

#include "SDL3/SDL_begin_code.h"

struct _TILE_PROPERTY
{
    bool show;
};
typedef struct _TILE_PROPERTY TILE_PROPERTY;

struct _Map_Group;

struct _Map_Group
{
    int32_t groupID;
    struct _Map_Group * up;
    struct _Map_Group * down;
    struct _Map_Group * left;
    struct _Map_Group * right;
    Uint32 ** indices;
};
typedef struct _Map_Group Map_Group;

/* x , y is point of top-left*/
struct _TILE_MAP
{
    char innerName[16];
    Uint32 rowCount;
    Uint32 colCount;
    Uint32 groupCount;
    Map_Group * mapGroups;
};
typedef struct _TILE_MAP TILE_MAP;

struct _TILE
{
    Uint32 ID;
    TILE_PROPERTY property;
    vec2 tileUV[4];
};
typedef struct _TILE TILE;

struct _TILE_SET
{
    char innerName[16];
    Uint32 tileCount;
    Uint32 tileWidth;
    Uint32 tileHeight;
    TILE * tiles;
    TILE_MAP * maps;
    Uint32 mapCount;
};
typedef struct _TILE_SET TILE_SET;

struct _TSD_Head
{
    char format[4];
    Uint32 imageWidth;
    Uint32 imageHeight;
    Uint32 tileWidth;
    Uint32 tileHeight;
    Uint32 propertyCount;
    Uint32 dataLen;
};
typedef struct _TSD_Head TSD_Head;

struct _TSDI_Head
{
    char format[4];
    Uint32 rowCount;
    Uint32 colCount; 
    Uint32 groupCount;
    Uint32 dataLen;
};
typedef struct _TSDI_Head TSDI_Head;

extern bool SDLCALL initTileMapSystem(void);
extern void SDLCALL getTileSetCount(Uint32 * pTileSetCount);
extern void SDLCALL getTileSetPtr(TILE_SET ** ppSet);
extern bool SDLCALL loadTileSet(PathType setImagePath, PathType setDataPath, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL loadTileMap(PathType tileMapData, const char * setInnerName, const char * mapInnerName);
extern Map_Group* SDLCALL getFirstMapGroup(const char * setInnerName, const char * mapInnerName);
extern Map_Group* SDLCALL getMapGroup(const char * setInnerName, const char * mapInnerName, int32_t groupID);
extern Map_Group* SDLCALL mapGroupToRight(Map_Group * mapGroup, int32_t moveCount);
extern Map_Group* SDLCALL mapGroupToDown(Map_Group * mapGroup, int32_t moveCount);
extern void SDLCALL deInitTileMapSystem(void);

#include "SDL3/SDL_close_code.h"

#endif // G_TileSet.h
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

/* x , y is point of top-left*/
struct _TILE_MAP
{
    Uint32 rowCount;
    Uint32 colCount;
    int32_t x;
    int32_t y;
    Uint32 * indeices;
};
typedef struct _TILE_MAP TILE_MAP;

struct _TILE_SET
{
    char innerName[16];
    Uint32 tileCount;
    Uint32 tileWidth;
    Uint32 tileHeight;
    TILE_PROPERTY * properties;
    vec2 ** tileUV;
    TILE_MAP * maps;
    Uint32 mapCount;
};
typedef struct _TILE_SET TILE_SET;

extern void SDLCALL initTileMapSystem(void);
extern void SDLCALL getTileSetCount(Uint32 * pTileSetCount);
extern void SDLCALL getTileSetPtr(TILE_SET ** ppSet);
extern bool SDLCALL loadTileSet(PathType setImagePath, PathType setDataPath, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL loadTileMap(PathType tileMapData, int32_t x, int32_t y, const char * innerName);
extern void SDLCALL deInitTileMapSystem(void);

#include "SDL3/SDL_close_code.h"

#endif // G_TileSet.h
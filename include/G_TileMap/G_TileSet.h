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

struct _TILE_SET
{
    char innerName[16];
    Uint32 tileCount;
    Uint32 tileWidth;
    Uint32 tileHeight;
    TILE_PROPERTY * properties;
    vec2 ** tileUV;
    G_Texture_P * pTexture;
};
typedef struct _TILE_SET TILE_SET;

extern void initTileMapSystem(void);
extern bool SDLCALL loadTileSet(PathType setImagePath, PathType setDataPath, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet);
extern TILE_SET * SDLCALL getTileSet(const char * innerName);
extern void SDLCALL deInitTileMapSystem(void);

#include "SDL3/SDL_close_code.h"

#endif // G_TileSet.h
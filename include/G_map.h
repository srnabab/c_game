#include "SDL3/SDL_stdinc.h"
#include "G_TileMap/G_TileSet.h"
#include "G_entity.h"
#include "vulkan/vulkan.h"

#ifndef G_MAP_H
#define G_MAP_H 1

struct _FromTo
{
    Uint32 from;
    Uint32 to;
};
typedef struct _FromTo FromTo;

struct _DrawHere
{
    Uint32 BottomID;
    char align1[4];
    Map_Group * group;
};
typedef struct _DrawHere DrawHere;

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL setMapBottom(Uint32 width, Uint32 height, int centerX, int centerY, Uint32 * pRowCount, Uint32 * pColumnCount, int * pFirstBottom_X, int * pFirstBottom_Y, int * pBaseX, int * pBaseY, int32_t * pFirstBottomID);
extern bool SDLCALL moveBottomImage(Uint32 currentFrame);
extern G_Point_Int SDLCALL locatePoint(G_Entity * entity, Uint32 groupRowCount, Uint32 groupColCount, int32_t firstBottom_X, int32_t firstBottom_Y, Uint32 groupID);

#include "SDL3/SDL_close_code.h"

#endif // G_map.h
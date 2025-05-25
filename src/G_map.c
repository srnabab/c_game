#include "G_constants.h"
#include "G_staticModel.h"
#include "G_map.h"
#include "G_threadPool.h"
#include "G_log.h"
#include "G_struct.h"

#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_move.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;

struct _mapGroupVertex
{
    vec2 pos;
    Uint32 offset;
};
typedef struct _mapGroupVertex mapGroupVertex;

static mapGroupVertex mapGroupVertices[MAX_MAP_GROUP];

#define LEFT_COL_ADD 1
#define RIGHT_COL_ADD 2
#define DOWN_ROW_ADD 4 
#define UP_ROW_ADD 8
#define LEFT_COL_DEL 16
#define RIGHT_COL_DEL 32
#define UP_ROW_DEL 64
#define DOWN_ROW_DEL 128

void mapVertexInitialize(float x, float y, float width, float height, float depth, Uint32 * pVertexCount, Vertex332_ * pVertices, TILE_MAP * pMap, int32_t firstGroupID)
{
    Uint32 i, l, j, k;
    float xpos = x;
    float ypos = y;
    vec2 UVs[2500 * 4];
    Map_Group * pGroup = NULL;
    Map_Group * firstGroup = NULL;
    if (firstGroupID != -1)
    {
        pGroup = getMapGroup(TEXTURE_TILE_SET, MAIN_TILE_MAP, firstGroupID);
    }
    else
    {
        pGroup = getFirstMapGroup(TEXTURE_TILE_SET, MAIN_TILE_MAP);
    }
    firstGroup = pGroup;

    for (i = 0;i < 6;i++)
    // for (i = 0;i < 1;i++)
    {
        xpos = x;
        ypos = y - i * height * 50;
        for (l = 0;l < 8;l++)
        // for (l = 0;l < 1;l++)
        {
            xpos = x + l * width * 50;
            ypos = y - i * height * 50;
            mapGroupVertices[i * 8 + l].pos[0] = xpos;
            mapGroupVertices[i * 8 + l].pos[1] = ypos;
            mapGroupVertices[i * 8 + l].offset = *pVertexCount;
            setTilemapUVs(pGroup, UVs, 0);
            for (j = 0;j < 50;j++)
            {
                xpos = x + l * width * 50;
                ypos -= height;
                for (k = 0;k < 50;k++)
                {
                    textureVertexInit_SetUV(xpos, ypos, width, height, depth, pVertexCount, pVertices, UVs + (j * 50 + k) * 4, getTexture(TEXTURE_TILE_SET));
                    xpos += width;
                }
            }
            pGroup = pGroup->right;
        }
        pGroup = firstGroup->down;
        firstGroup = pGroup;
    }
}
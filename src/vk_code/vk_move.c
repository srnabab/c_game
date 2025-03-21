#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_buffer.h"

#include "G_struct.h"
#include "G_TileMap/G_TileSet.h"
#include "G_log.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;

void initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, Uint32 verticesCount)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * verticesCount;

    createBuffer(pMoveBuffer, pMoveBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    vkMapMemory(*pDevice, *pMoveBufferMemory, 0, bufferSize, 0, ppMovingBufferMapped);
    memcpy(*ppMovingBufferMapped, vertices, (size_t)bufferSize);
}
void vertexInitialize(float x, float y, float width, float height, float depth, Uint32 vertexCount, Vertex * pVertices)
{
    float WindowHeight = (float)allInOne.pExtent2D->height;
    float xOffset = width / (WindowHeight / 2);
    float yOffset = height / (WindowHeight / 2);

    vec2 leftUp;
    leftUp[0] = x / (WindowHeight / 2);
    leftUp[1] = y / (WindowHeight / 2);
    // vec2 rightUp; 
    // rightUp[0] = leftUp[0] + xOffset;
    // rightUp[1] = leftUp[1];
    // vec2 rightDown;
    // rightDown[0] = rightUp[0];
    // rightDown[1] = rightUp[1] + yOffset;
    // vec2 leftDown;
    // leftDown[0] = leftUp[0];
    // leftDown[1] = leftUp[1] + yOffset;

    //left-up
    pVertices[vertexCount].pos[0] = leftUp[0];
    pVertices[vertexCount].pos[1] = leftUp[1];
    pVertices[vertexCount].pos[2] = depth;
    pVertices[vertexCount].color[0] = 1.0f;
    pVertices[vertexCount].color[1] = 0.0f;
    pVertices[vertexCount].color[2] = 0.0f;
    pVertices[vertexCount].texCoord[0] = 0.0f;
    pVertices[vertexCount].texCoord[1] = 1.0f;

    //right-up
    vertexCount++;
    pVertices[vertexCount].pos[0] = leftUp[0] + xOffset;
    pVertices[vertexCount].pos[1] = leftUp[1];
    pVertices[vertexCount].pos[2] = depth;
    pVertices[vertexCount].color[0] = 0.0f;
    pVertices[vertexCount].color[1] = 1.0f;
    pVertices[vertexCount].color[2] = 0.0f;
    pVertices[vertexCount].texCoord[0] = 1.0f;
    pVertices[vertexCount].texCoord[1] = 1.0f;

    //right-dowm
    vertexCount++;
    pVertices[vertexCount].pos[0] = leftUp[0] + xOffset;
    pVertices[vertexCount].pos[1] = leftUp[1] + yOffset;
    pVertices[vertexCount].pos[2] = depth;
    pVertices[vertexCount].color[0] = 0.0f;
    pVertices[vertexCount].color[1] = 0.0f;
    pVertices[vertexCount].color[2] = 1.0f;
    pVertices[vertexCount].texCoord[0] = 1.0f;
    pVertices[vertexCount].texCoord[1] = 0.0f;

    //left-down
    vertexCount++;
    pVertices[vertexCount].pos[0] = leftUp[0];
    pVertices[vertexCount].pos[1] = leftUp[1] + yOffset;
    pVertices[vertexCount].pos[2] = depth;
    pVertices[vertexCount].color[0] = 1.0f;
    pVertices[vertexCount].color[1] = 1.0f;
    pVertices[vertexCount].color[2] = 1.0f;
    pVertices[vertexCount].texCoord[0] = 0.0f;
    pVertices[vertexCount].texCoord[1] = 0.0f;
}
void textureVertexInit(float x, float y, float width, float height, float depth, Uint32 * pVertexCount, Vertex * pVertices, G_Texture_P * tempTexture)
{
    SDL_LockMutex(allSync.vertexMutex);

    Uint32 vertexCount = *pVertexCount;
    textureOffsetsAdd(tempTexture, vertexCount);
    if (*pVertexCount < allInOne.maxVertices2DCount) *pVertexCount += 4;
    else return;

    SDL_UnlockMutex(allSync.vertexMutex);

    SDL_LockMutex(allSync.vertexMutex);
    vertexInitialize(x, y, width, height, depth, vertexCount, pVertices);
    SDL_UnlockMutex(allSync.vertexMutex);
}
void textureVertexInit_SetUV(float x, float y, float width, float height, float depth, Uint32 * pVertexCount, Vertex * pVertices, vec2 * UV, G_Texture_P * tempTexture)
{
    SDL_LockMutex(allSync.vertexMutex);

    Uint32 vertexCount = *pVertexCount;
    textureOffsetsAdd(tempTexture, vertexCount);
    if (*pVertexCount < allInOne.maxVertices2DCount) *pVertexCount += 4;
    else return;

    SDL_UnlockMutex(allSync.vertexMutex);

    SDL_LockMutex(allSync.vertexMutex);
    vertexInitialize(x, y, width, height, depth, vertexCount, pVertices);
    
    pVertices[vertexCount].texCoord[0] = UV[0][0];
    pVertices[vertexCount].texCoord[1] = UV[0][1];

    vertexCount++;
    pVertices[vertexCount].texCoord[0] = UV[1][0];
    pVertices[vertexCount].texCoord[1] = UV[1][1];
    
    vertexCount++;
    pVertices[vertexCount].texCoord[0] = UV[2][0];
    pVertices[vertexCount].texCoord[1] = UV[2][1];
    
    vertexCount++;
    pVertices[vertexCount].texCoord[0] = UV[3][0];
    pVertices[vertexCount].texCoord[1] = UV[3][1];
    SDL_UnlockMutex(allSync.vertexMutex);
}
void tileMapVertexInit(Uint32 * pVertexCount, Vertex * pVertices)
{
    Uint32 tileSetCount = 0;
    getTileSetCount(&tileSetCount);
    print("tile set count: %u", tileSetCount);

    TILE_SET * pSet = NULL;
    getTileSetPtr(&pSet);

    SDL_LockMutex(allSync.vertexMutex);
    Uint32 tileWidth, tileHeight, mapRowCount, mapColCount, i, j, k, m;
    for (i = 0;i < tileSetCount;i++)
    {
        tileWidth = pSet[i].tileWidth;
        tileHeight = pSet[i].tileHeight;
        G_Texture_P * pTexture = getTexture(pSet->innerName);

        for (j = 0;j < pSet[i].mapCount;j++)
        {
            mapRowCount = pSet[i].maps[j].rowCount;
            mapColCount = pSet[i].maps[j].colCount;

            for (k = 0;k < mapRowCount;k++)
            for (m = 0;m < mapColCount;m++)
            {
                // print("texture ID: %u", pTexture->ID);
                // print("texture refCount: %u", pTexture->refCount);
                textureVertexInit_SetUV((float)pSet[i].maps[j].x + (float)tileWidth * m, (float)pSet[i].maps[j].y + (float)tileHeight * k, tileWidth, tileHeight, 0.91f, pVertexCount, pVertices, pSet[i].tileUV[pSet[i].maps[j].indeices[k * mapColCount + m]], pTexture);
            }
        }
    }
    SDL_UnlockMutex(allSync.vertexMutex);
}
void texturePosUpdate(float x, float y, Vertex * pVertices, Uint32 offset)
{
    vertexPosUpdate(x, y, pVertices, offset);
}
void vertexPosUpdate(float x, float y, Vertex * pVertices, Uint32 vertexCount)
{
    float NDCx = x / (allInOne.pExtent2D->height / 2);
    float NDCy = y / (allInOne.pExtent2D->height / 2);

    float offSetX = NDCx - pVertices[vertexCount].pos[0];
    float offSetY = NDCy - pVertices[vertexCount].pos[1];

    SDL_LockMutex(allSync.vertexMutex);

    pVertices[vertexCount].pos[0] += offSetX;
    pVertices[vertexCount].pos[1] += offSetY;
    
    vertexCount++;
    pVertices[vertexCount].pos[0] += offSetX;
    pVertices[vertexCount].pos[1] += offSetY;
    
    vertexCount++;
    pVertices[vertexCount].pos[0] += offSetX;
    pVertices[vertexCount].pos[1] += offSetY;
    
    vertexCount++;
    pVertices[vertexCount].pos[0] += offSetX;
    pVertices[vertexCount].pos[1] += offSetY;

    SDL_UnlockMutex(allSync.vertexMutex);
}
void indexInitialize(Uint16 * indices, Uint32 indicesCount)
{
    for (Uint32 i = 0;i < indicesCount;i++)
    {
        Uint32 index = i * 6;
        Uint16 serial = i * 4;
        indices[index] = serial;
        indices[index + 1] = serial + 1;
        indices[index + 2] = serial + 2;
        indices[index + 3] = serial + 2;
        indices[index + 4] = serial + 3;
        indices[index + 5] = serial;
    }
}
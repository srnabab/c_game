#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_buffer.h"

#include "SDL3/SDL_mutex.h"

extern VK_ALL allInOne;

static vec3 vertices_Color[4] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f}
};

static SDL_Mutex * vertexMutex = NULL;

void initVertexMutex(void)
{
    vertexMutex = SDL_CreateMutex();
}
void initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, Uint32 verticesCount)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * verticesCount;

    createBuffer(pMoveBuffer, pMoveBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    vkMapMemory(*pDevice, *pMoveBufferMemory, 0, bufferSize, 0, ppMovingBufferMapped);
    memcpy(*ppMovingBufferMapped, vertices, (size_t)bufferSize);
}
// void vertexPosInit(float x, float y, float width, float height, float depth, Uint32 vertexCount, vec3 ** ppVertices_Pos)
// {
//     // int WindowWidth = extent2D.width;
//     float WindowHeight = (float)allInOne.pExtent2D->height;
//     float xOffset = width / (WindowHeight / 2);
//     float yOffset = height / (WindowHeight / 2);

//     vec2 leftUp;
//     leftUp[0] = x / (WindowHeight / 2);
//     leftUp[1] = y / (WindowHeight / 2);
//     // vec2 rightUp; 
//     // rightUp[0] = leftUp[0] + xOffset;
//     // rightUp[1] = leftUp[1];
//     // vec2 rightDown;
//     // rightDown[0] = rightUp[0];
//     // rightDown[1] = rightUp[1] + yOffset;
//     // vec2 leftDown;
//     // leftDown[0] = leftUp[0];
//     // leftDown[1] = leftUp[1] + yOffset;

//     //left-up
//     (*ppVertices_Pos)[vertexCount][0] = leftUp[0];
//     (*ppVertices_Pos)[vertexCount][1] = leftUp[1];
//     //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber].pos[0], (*ppVertices)[startNumber].pos[1]);

//     //right-up
//     (*ppVertices_Pos)[vertexCount + 1][0] = leftUp[0] + xOffset;
//     (*ppVertices_Pos)[vertexCount + 1][1] = leftUp[1];
//     //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 1].pos[0], (*ppVertices)[startNumber + 1].pos[1]);

//     //right-dowm
//     (*ppVertices_Pos)[vertexCount + 2][0] = leftUp[0] + xOffset;
//     (*ppVertices_Pos)[vertexCount + 2][1] = leftUp[1] + yOffset;
//     //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 2].pos[0], (*ppVertices)[startNumber + 2].pos[1]);

//     //left-down
//     (*ppVertices_Pos)[vertexCount + 3][0] = leftUp[0];
//     (*ppVertices_Pos)[vertexCount + 3][1] = leftUp[1] + yOffset;
//     //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 3].pos[0], (*ppVertices)[startNumber + 3].pos[1]);
    
//     for (Uint32 i = vertexCount;i < vertexCount + 4;i++)
//     {
//         (*ppVertices_Pos)[i][2] = depth;
//     }
// }
// void vertexColorInit(Uint32 vertexCount, vec3 ** ppVertices_Color)
// {
//     memcpy((*ppVertices_Color) + vertexCount, vertices_Color, 4 * sizeof(vec3));
// }
// void vertexTexCoordInit(bool setUVZero, Uint32 vertexCount, vec2 ** ppVertices_TexCoord)
// {
//     if (setUVZero)
//     {
//         for (int i = vertexCount;i < 4 + vertexCount;i++)
//         {
//             (*ppVertices_TexCoord)[i][0] = 0.0f;
//             (*ppVertices_TexCoord)[i][1] = 0.0f;
//         }
//     }
//     else
//     {
//         (*ppVertices_TexCoord)[vertexCount][0] = 0.0f;
//         (*ppVertices_TexCoord)[vertexCount][1] = 1.0f;

//         (*ppVertices_TexCoord)[vertexCount + 1][0] = 1.0f;
//         (*ppVertices_TexCoord)[vertexCount + 1][1] = 1.0f;

//         (*ppVertices_TexCoord)[vertexCount + 2][0] = 1.0f;
//         (*ppVertices_TexCoord)[vertexCount + 2][1] = 0.0f;

//         (*ppVertices_TexCoord)[vertexCount + 3][0] = 0.0f;
//         (*ppVertices_TexCoord)[vertexCount + 3][1] = 0.0f;
//     }
// }
// void vertexInitialize(float x, float y, float width, float height, float depth, bool setUVZero, Uint32 pictureSequence, vec3 ** ppVertices_Pos, vec3 ** ppVertices_Color, vec2 ** ppVertices_TexCoord)
// {
//     vertexPosInit(x, y, width, height, depth, *pVertexCount, ppVertices_Pos);

//     vertexColorInit(vertexCount, allInOne.ppVertices_Color);

//     vertexTexCoordInit(setUVZero, vertexCount, allInOne.ppVertices_TexCoord);
// }

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
    SDL_LockMutex(vertexMutex);

    Uint32 vertexCount = *pVertexCount;
    textureOffsetsAdd(tempTexture, vertexCount);
    if (*pVertexCount < allInOne.maxVerticesCount) *pVertexCount += 4;
    else 
    {
        vertexCount = 0;
        *pVertexCount = 4;
    }

    SDL_UnlockMutex(vertexMutex);

    SDL_LockMutex(vertexMutex);
    vertexInitialize(x, y, width, height, depth, vertexCount, pVertices);
    SDL_UnlockMutex(vertexMutex);
}
void textureVertexInit_SetUV(float x, float y, float width, float height, float depth, Uint32 * pVertexCount, Vertex * pVertices, vec2 * UV, G_Texture_P * tempTexture)
{
    SDL_LockMutex(vertexMutex);

    Uint32 vertexCount = *pVertexCount;
    textureOffsetsAdd(tempTexture, vertexCount);
    if (*pVertexCount < allInOne.maxVerticesCount) *pVertexCount += 4;
    else 
    {
        vertexCount = 0;
        *pVertexCount = 4;
    }

    SDL_UnlockMutex(vertexMutex);

    SDL_LockMutex(vertexMutex);
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
    SDL_UnlockMutex(vertexMutex);
}
void texturePosUpdate(float x, float y, Vertex * pVertices, Uint32 * textureOffsets, Uint32 index)
{
    vertexPosUpdate(x, y, pVertices, textureOffsets[index]);
}
void vertexPosUpdate(float x, float y, Vertex * pVertices, Uint32 vertexCount)
{
    float NDCx = x / (allInOne.pExtent2D->height / 2);
    float NDCy = y / (allInOne.pExtent2D->height / 2);

    float offSetX = NDCx - pVertices[vertexCount].pos[0];
    float offSetY = NDCy - pVertices[vertexCount].pos[1];

    SDL_LockMutex(vertexMutex);

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

    SDL_UnlockMutex(vertexMutex);
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
void updatePosition(float x, float y, vec3 ** ppVertices_Pos, uint32_t pictureSequence)
{
    uint32_t startNumber = pictureSequence * 4;

    float NDCx = x / (allInOne.pExtent2D->height / 2);
    float NDCy = y / (allInOne.pExtent2D->height / 2);

    float offSetX = NDCx - (*ppVertices_Pos)[startNumber][0];
    float offSetY = NDCy - (*ppVertices_Pos)[startNumber][1];

    for (Uint32 i = startNumber;i < startNumber + 4;i++)
    {
        (*ppVertices_Pos)[i][0] += offSetX;
        (*ppVertices_Pos)[i][1] += offSetY;
    }
}
void deInitVertexMutex(void)
{
    SDL_DestroyMutex(vertexMutex);
}
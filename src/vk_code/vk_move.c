#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_buffer.h"

extern VK_ALL allInOne;

static vec3 vertices_Color[4] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f}
};

void initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, Uint32 verticesCount)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * verticesCount;

    createBuffer(pMoveBuffer, pMoveBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    vkMapMemory(*pDevice, *pMoveBufferMemory, 0, bufferSize, 0, ppMovingBufferMapped);
    memcpy(*ppMovingBufferMapped, vertices, (size_t)bufferSize);
}
void vertexPosInit(float x, float y, float width, float height, float depth, Uint32 pictureSequence, vec3 ** ppVertices_Pos)
{
    Uint32 startNumber = pictureSequence * 4;

    // int WindowWidth = extent2D.width;
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
    (*ppVertices_Pos)[startNumber][0] = leftUp[0];
    (*ppVertices_Pos)[startNumber][1] = leftUp[1];
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber].pos[0], (*ppVertices)[startNumber].pos[1]);

    //right-up
    (*ppVertices_Pos)[startNumber + 1][0] = leftUp[0] + xOffset;
    (*ppVertices_Pos)[startNumber + 1][1] = leftUp[1];
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 1].pos[0], (*ppVertices)[startNumber + 1].pos[1]);

    //right-dowm
    (*ppVertices_Pos)[startNumber + 2][0] = leftUp[0] + xOffset;
    (*ppVertices_Pos)[startNumber + 2][1] = leftUp[1] + yOffset;
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 2].pos[0], (*ppVertices)[startNumber + 2].pos[1]);

    //left-down
    (*ppVertices_Pos)[startNumber + 3][0] = leftUp[0];
    (*ppVertices_Pos)[startNumber + 3][1] = leftUp[1] + yOffset;
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 3].pos[0], (*ppVertices)[startNumber + 3].pos[1]);
    
    for (Uint32 i = startNumber;i < startNumber + 4;i++)
    {
        (*ppVertices_Pos)[i][2] = depth;
    }
}
void vertexColorInit(Uint32 pictureSequence, vec3 ** ppVertices_Color)
{
    Uint32 startNumber = pictureSequence * 4;

    memcpy((*ppVertices_Color) + startNumber, vertices_Color, 4 * sizeof(vec3));
}
void vertexTexCoordInit(bool setUVZero, Uint32 pictureSequence, vec2 ** ppVertices_TexCoord)
{
    Uint32 startNumber = pictureSequence * 4;

    if (setUVZero)
    {
        for (int i = startNumber;i < 4 + startNumber;i++)
        {
            (*ppVertices_TexCoord)[i][0] = 0.0f;
            (*ppVertices_TexCoord)[i][1] = 0.0f;
        }
    }
    else
    {
        (*ppVertices_TexCoord)[startNumber][0] = 0.0f;
        (*ppVertices_TexCoord)[startNumber][1] = 1.0f;

        (*ppVertices_TexCoord)[startNumber + 1][0] = 1.0f;
        (*ppVertices_TexCoord)[startNumber + 1][1] = 1.0f;

        (*ppVertices_TexCoord)[startNumber + 2][0] = 1.0f;
        (*ppVertices_TexCoord)[startNumber + 2][1] = 0.0f;

        (*ppVertices_TexCoord)[startNumber + 3][0] = 0.0f;
        (*ppVertices_TexCoord)[startNumber + 3][1] = 0.0f;
    }
}
void vertexInitialize(float x, float y, float width, float height, float depth, bool setUVZero, Uint32 pictureSequence, vec3 ** ppVertices_Pos, vec3 ** ppVertices_Color, vec2 ** ppVertices_TexCoord)
{
    vertexPosInit(x, y, width, height, depth, pictureSequence, ppVertices_Pos);

    vertexColorInit(pictureSequence, ppVertices_Color);

    vertexTexCoordInit(setUVZero, pictureSequence, ppVertices_TexCoord);
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
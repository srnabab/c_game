#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_buffer.h"

void initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, Uint32 verticesCount)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * verticesCount;

    createBuffer(pPhysicalDevice, pDevice, pMoveBuffer, pMoveBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    vkMapMemory(*pDevice, *pMoveBufferMemory, 0, bufferSize, 0, ppMovingBufferMapped);
    memcpy(*ppMovingBufferMapped, vertices, (size_t)bufferSize);
}
void vertexInitialize(float x, float y, float width, float height, float depth, bool setUVZero, VkExtent2D extent2D, Vertex ** ppVertices, Uint32 pictureSequence)
{
    Uint32 startNumber = pictureSequence * 4;

    // int WindowWidth = extent2D.width;
    float WindowHeight = (float)extent2D.height;
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
    (*ppVertices)[startNumber].pos[0] = leftUp[0];
    (*ppVertices)[startNumber].pos[1] = leftUp[1];
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber].pos[0], (*ppVertices)[startNumber].pos[1]);

    //right-up
    (*ppVertices)[startNumber + 1].pos[0] = leftUp[0] + xOffset;
    (*ppVertices)[startNumber + 1].pos[1] = leftUp[1];
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 1].pos[0], (*ppVertices)[startNumber + 1].pos[1]);

    //right-dowm
    (*ppVertices)[startNumber + 2].pos[0] = leftUp[0] + xOffset;
    (*ppVertices)[startNumber + 2].pos[1] = leftUp[1] + yOffset;
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 2].pos[0], (*ppVertices)[startNumber + 2].pos[1]);

    //left-down
    (*ppVertices)[startNumber + 3].pos[0] = leftUp[0];
    (*ppVertices)[startNumber + 3].pos[1] = leftUp[1] + yOffset;
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 3].pos[0], (*ppVertices)[startNumber + 3].pos[1]);
    
    for (Uint32 i = startNumber;i < startNumber + 4;i++)
    {
        (*ppVertices)[i].pos[2] = depth;
    }
    
    (*ppVertices)[startNumber].color[0] = 1.0f;
    (*ppVertices)[startNumber].color[1] = 0.0f;
    (*ppVertices)[startNumber].color[2] = 0.0f;

    (*ppVertices)[startNumber + 1].color[0] = 0.0f;
    (*ppVertices)[startNumber + 1].color[1] = 1.0f;
    (*ppVertices)[startNumber + 1].color[2] = 0.0f;

    (*ppVertices)[startNumber + 2].color[0] = 0.0f;
    (*ppVertices)[startNumber + 2].color[1] = 0.0f;
    (*ppVertices)[startNumber + 2].color[2] = 1.0f;

    (*ppVertices)[startNumber + 3].color[0] = 1.0f;
    (*ppVertices)[startNumber + 3].color[1] = 1.0f;
    (*ppVertices)[startNumber + 3].color[2] = 1.0f;

    if (setUVZero)
    {
        for (int i = startNumber;i < 4;i++)
        {
            (*ppVertices)[i].texCoord[0] = 0.0f;
            (*ppVertices)[i].texCoord[1] = 0.0f;
        }
    }
    else
    {
        (*ppVertices)[startNumber].texCoord[0] = 0.0f;
        (*ppVertices)[startNumber].texCoord[1] = 1.0f;

        (*ppVertices)[startNumber + 1].texCoord[0] = 1.0f;
        (*ppVertices)[startNumber + 1].texCoord[1] = 1.0f;

        (*ppVertices)[startNumber + 2].texCoord[0] = 1.0f;
        (*ppVertices)[startNumber + 2].texCoord[1] = 0.0f;

        (*ppVertices)[startNumber + 3].texCoord[0] = 0.0f;
        (*ppVertices)[startNumber + 3].texCoord[1] = 0.0f;
    }
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
void updatePosition(float x, float y, VkExtent2D * pExtent2D, Vertex ** ppVertices, uint32_t pictureSequence)
{
    uint32_t startNumber = pictureSequence * 4;

    float NDCx = x / (pExtent2D->height / 2);
    float NDCy = y / (pExtent2D->height / 2);

    float offSetX = NDCx - (*ppVertices)[startNumber].pos[0];
    float offSetY = NDCy - (*ppVertices)[startNumber].pos[1];

    (*ppVertices)[startNumber].pos[0] += offSetX;
    (*ppVertices)[startNumber].pos[1] += offSetY;

    (*ppVertices)[startNumber + 1].pos[0] += offSetX;
    (*ppVertices)[startNumber + 1].pos[1] += offSetY;

    (*ppVertices)[startNumber + 2].pos[0] += offSetX;
    (*ppVertices)[startNumber + 2].pos[1] += offSetY;

    (*ppVertices)[startNumber + 3].pos[0] += offSetX;
    (*ppVertices)[startNumber + 3].pos[1] += offSetY;
}
void reInitializePosition(int width, int height, VkExtent2D extent2D, Vertex ** ppVertices, Uint32 pictureSequence)
{
    Uint32 startNumber = pictureSequence * 4;

    // int WindowWidth = extent2D.width;
    int WindowHeight = extent2D.height;
    float xOffset = (float) width / (WindowHeight / 2);
    float yOffset = (float) height / (WindowHeight / 2);

    vec2 leftUp;
    leftUp[0] = (*ppVertices)[startNumber].pos[0];
    leftUp[1] = (*ppVertices)[startNumber].pos[1];
    /*vec2 rightUp; 
    rightUp[0] = leftUp[0] + xOffset;
    rightUp[1] = leftUp[1];
    vec2 rightDown;
    rightDown[0] = rightUp[0];
    rightDown[1] = rightUp[1] + yOffset;
    vec2 leftDown;
    leftDown[0] = leftUp[0];
    leftDown[1] = leftUp[1] + yOffset;*/

    //left-up
    (*ppVertices)[startNumber].pos[0] = leftUp[0];
    (*ppVertices)[startNumber].pos[1] = leftUp[1];
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber].pos[0], (*ppVertices)[startNumber].pos[1]);

    //right-up
    (*ppVertices)[startNumber + 1].pos[0] = leftUp[0] + xOffset;
    (*ppVertices)[startNumber + 1].pos[1] = leftUp[1];
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 1].pos[0], (*ppVertices)[startNumber + 1].pos[1]);

    //right-dowm
    (*ppVertices)[startNumber + 2].pos[0] = leftUp[0] + xOffset;
    (*ppVertices)[startNumber + 2].pos[1] = leftUp[1] + yOffset;
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 2].pos[0], (*ppVertices)[startNumber + 2].pos[1]);

    //left-down
    (*ppVertices)[startNumber + 3].pos[0] = leftUp[0];
    (*ppVertices)[startNumber + 3].pos[1] = leftUp[1] + yOffset;
}
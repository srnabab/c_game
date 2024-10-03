#include "vk_move.h"

void initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, uint32_t verticesCount)
{
    FuncCode code = initializeMovingBufferF;

    VkDeviceSize bufferSize = sizeof(vertices[0]) * verticesCount;

    resultVulkan(createBuffer(pPhysicalDevice, pDevice, pMoveBuffer, pMoveBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), code, 0);
    
    vkMapMemory(*pDevice, *pMoveBufferMemory, 0, bufferSize, 0, ppMovingBufferMapped);
    memcpy(*ppMovingBufferMapped, vertices, (size_t)bufferSize);
}
void positionInitialize(int x, int y, int width, int height, VkExtent2D extent2D, Vertex ** ppVertices, uint32_t pictureSequence)
{
    uint32_t startNumber = pictureSequence * 4;

    int WindowWidth = extent2D.width;
    int WindowHeight = extent2D.height;
    float xOffset = (float) width / (WindowWidth / 2);
    float yOffset = (float) height / (WindowHeight / 2);

    vec2 leftUp;
    leftUp[0] = (float)x / (WindowWidth / 2);
    leftUp[1] = (float)y / (WindowHeight / 2);
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
    //printf("x, y: %.2f, %.2f\n", (*ppVertices)[startNumber + 3].pos[0], (*ppVertices)[startNumber + 3].pos[1]);

    int a = 0;
}
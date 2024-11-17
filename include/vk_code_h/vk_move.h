#include "core.h"

#ifndef VK_MOVE_H
#define VK_MOVE_H

void initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, uint32_t verticesCount);
void positionInitialize(int x, int y, int width, int height, VkExtent2D extent2D, Vertex ** ppVertices, uint32_t pictureSequence);
void reInitializePosition(int width, int height, VkExtent2D extent2D, Vertex ** ppVertices, uint32_t pictureSequence);

#endif //vk_move.h
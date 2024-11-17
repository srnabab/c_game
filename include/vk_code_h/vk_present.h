#include "core.h"

#ifndef VK_PRESENT_H
#define VK_PRESENT_H

void recordCommandBuffer(VK_ALL * pAllInOne, uint32_t imageIndex);
void recordComputeCommandBuffer(VK_ALL * pAllInOne);
void updateUniformBuffer(uint32_t currentImage, VkExtent2D * pExtent2D, UniformBufferObject * pUbo, void *** pUniformBuffersMapped, float camera_X, float camera_Y, ComputeUniformBufferObject * pComputeUbo, void *** pppComputeUniformBufferMapped, float deltaTime);
void drawFrame(VK_ALL * pAllInOne);
void updatePosition(float x, float y, VkExtent2D * pExtent2D, Vertex ** ppVertices, uint32_t pictureSequence);

#endif
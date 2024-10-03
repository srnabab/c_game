#include "vk_all.h"

#ifndef VK_COMMANDPOOL_H
#define VK_COMMANDPOOL_H

void createCommandPool(VkDevice * pDevice, uint32_t graphicsFamilyIndice, VkCommandPool * pCommandPool);
void createCommandbuffer(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer ** pCommandBuffer);

#endif
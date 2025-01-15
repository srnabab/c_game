#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_COMMANDPOOL_H
#define VK_COMMANDPOOL_H 1

void G_CALL createCommandPool(VkDevice * pDevice, uint32_t graphicsFamilyIndice, VkCommandPool * pCommandPool);
void G_CALL createCommandbuffer(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer ** pCommandBuffer);

#endif
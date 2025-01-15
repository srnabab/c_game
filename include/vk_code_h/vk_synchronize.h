#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_SYNCHRONIZE_H
#define VK_SYNCHRONIZE_H 1

void createSemaphore(VkDevice * pDevice, VkSemaphore ** pSemaphore);
void createFence(VkDevice * pDevice, VkFence ** pFence);

#endif
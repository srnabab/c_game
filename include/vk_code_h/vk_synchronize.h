#include "vk_all.h"

#ifndef VK_SYNCHRONIZE_H
#define VK_SYNCHRONIZE_H

void createSemaphore(VkDevice * pDevice, VkSemaphore ** pSemaphore);
void createFence(VkDevice * pDevice, VkFence ** pFence);

#endif
#include "core.h"

#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

void createInstance(VkInstance * pInstance);
bool checkValidationLayerSupport(uint32_t pCount, const char ** pLayers);

#endif //vk_instance.h
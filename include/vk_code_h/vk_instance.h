#include "vk_all.h"

#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

void createInstance(VkInstance * pInstance);
static bool extensionSupportedCheck(uint32_t a, const char * const * pA, uint32_t b, VkExtensionProperties * pE);
bool checkValidationLayerSupport(uint32_t pCount, const char ** pLayers);

#endif //vk_instance.h
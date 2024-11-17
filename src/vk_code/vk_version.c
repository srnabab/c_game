#include "vk_version.h"

void vulkanVersion(void)
{
    FuncCode code = vulkanVersionF;
    uint32_t apiVersion = 0;
    resultVulkan(vkEnumerateInstanceVersion(&apiVersion), code, 0);

    uint32_t major = VK_VERSION_MAJOR(apiVersion);
    uint32_t minor = VK_VERSION_MINOR(apiVersion);
    uint32_t patch = VK_VERSION_PATCH(apiVersion);

    printf("Vulkan API Version: %u.%u.%u\n", major, minor, patch);
}
#include "vk_code_h/vk_version.h"
#include "vk_code_h/vk_judge.h"

#include "G_log.h"

void vulkanVersion(void)
{
    uint32_t apiVersion = 0;
    resultVulkan(vkEnumerateInstanceVersion(&apiVersion), 0);

    uint32_t major = VK_VERSION_MAJOR(apiVersion);
    uint32_t minor = VK_VERSION_MINOR(apiVersion);
    uint32_t patch = VK_VERSION_PATCH(apiVersion);

    print("Vulkan API Version: %u.%u.%u\n", major, minor, patch);
}
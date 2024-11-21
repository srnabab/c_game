#include "vk_instance.h"
#include "SDL3/SDL_vulkan.h"
#include "name.h"

extern SDL_Window * window;

static bool extensionSupportedCheck(uint32_t sdlExtensionCount, const char * const * sdlExtensions, uint32_t extensionCount, VkExtensionProperties * pExtensionProperties)
{
    uint32_t count = 0;
    for (uint32_t i = 0;i < sdlExtensionCount;i++)
    {
        for (uint32_t q = 0;q < extensionCount;q++)
        {
            if (SDL_strcmp(sdlExtensions[i], pExtensionProperties[q].extensionName) == 0)
            {
                count++;
                break;
            }
        }
    }
    //printf("count: %u\n", count);
    if (count != sdlExtensionCount)
    {
        logMessage("extension not supported");
        return true;
    }

    return false;
}
void createInstance(VkInstance * pInstance)
{
    FuncCode code = createInstanceF;

    VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = VK_NULL_HANDLE;
    appInfo.pApplicationName = APP_NAME;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    uint32_t layersCount = 1;
    const char * validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    if (!checkValidationLayerSupport(layersCount, validationLayers))
    {
        layersCount = 0;
        logMessage("validation layers error");
    }

    uint32_t sdlExtensionCount = 0;
    //printf("%p\n", window);
    const char * const * sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

    uint32_t extensionCount = 0;
    resultVulkan(vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE), code, 0);

    VkExtensionProperties * extensionsProperties = (VkExtensionProperties *)SDL_malloc(extensionCount * sizeof(VkExtensionProperties));
    resultVulkan(vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, extensionsProperties), code, 2, sdlExtensions, extensionsProperties);

    //printf("sdlExtensionNumber: %u\n", sdlExtensionCount);

    extensionSupportedCheck(sdlExtensionCount, sdlExtensions, extensionCount, extensionsProperties);

    VkInstanceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        VK_NULL_HANDLE, 
        0,
        &appInfo,
        layersCount,
        validationLayers,
        sdlExtensionCount,
        sdlExtensions
    };

    resultVulkan(vkCreateInstance(&createInfo, VK_NULL_HANDLE, pInstance), code, 0);

    SDL_free(extensionsProperties);

    logMessage("instance created");
}
bool checkValidationLayerSupport(uint32_t pCount, const char ** pLayers)
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, VK_NULL_HANDLE);

    VkLayerProperties * avaliableLayers = (VkLayerProperties *)SDL_malloc(layerCount * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers);

    for (uint32_t i = 0;i < pCount;i++)
    {
        bool layerFound = false;
        for (uint32_t q = 0;q < layerCount;q++)
        {
            if (!SDL_strcmp(pLayers[i], avaliableLayers[q].layerName))
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            SDL_free(avaliableLayers);
            return false;
        }
    }
    
    SDL_free(avaliableLayers);
    return true;
}
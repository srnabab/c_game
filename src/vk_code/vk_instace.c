#include "vk_instance.h"
#include "SDL3/SDL_vulkan.h"
#include "name.h"

extern SDL_Window * window;

static bool extensionSupportedCheck_Must(uint32_t neededExtensionCount, char ** neededExtensions, uint32_t extensionCount, VkExtensionProperties * pExtensionProperties)
{
    uint32_t count = -1;//u32 overflow
    for (uint32_t i = 0;i < neededExtensionCount;i++)
    {
        for (uint32_t q = 0;q < extensionCount;q++)
        {
            if (SDL_strcmp(neededExtensions[i], pExtensionProperties[q].extensionName) == 0)
            {
                count++;
                break;
            }
        }
        if (count != i)
        {
            pushMessage(SDL_MESSAGEBOX_WARNING, "Error", "Vulkan Error\nExtension: %s not supported by instance", neededExtensions[i]);
            return false;
        }
    }
    count++;
    if (count != neededExtensionCount)
    {
        logMessage("extension not supported");
        return false;
    }

    return true;
}
static bool * extensionSupportedCheck_Optional(uint32_t neededExtensionCount, char ** neededExtensions, uint32_t extensionCount, VkExtensionProperties * pExtensionProperties)
{
    uint32_t count = -1;//u32 overflow
    bool * group = (bool*)SDL_malloc(sizeof(bool) * neededExtensionCount);
    for (uint32_t i = 0;i < neededExtensionCount;i++)
    {
        group[i] = false;
        for (uint32_t q = 0;q < extensionCount;q++)
        {
            if (SDL_strcmp(neededExtensions[i], pExtensionProperties[q].extensionName) == 0)
            {
                count++;
                group[i] = true;
                break;
            }
        }
    }
    count++;

    return group;
}
static bool checkValidationLayerSupport(uint32_t pCount, const char ** pLayers)
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
static void setRequiredExtensions(uint32_t extensionCount, const char ** extensions, char ** requireExtensions)
{
    static uint32_t requiredExtensionCount = 0;
    for (int i = 0;i < extensionCount;i++)
    {
        requireExtensions[requiredExtensionCount + i] = (char*)extensions[i];
    }
    requiredExtensionCount += extensionCount;
}
static void setOptionalExtensions(uint32_t extensionCount, const char ** extensions, char ** OptionalExtensions)
{
    static uint32_t OptionalExtensionCount = 0;
    for (int i = 0;i < extensionCount;i++)
    {
        OptionalExtensions[OptionalExtensionCount + i] = (char*)extensions[i];
    }
    OptionalExtensionCount += extensionCount;
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
    const char * validationLayersGroup[1] = {
        "VK_LAYER_KHRONOS_validation"
    };
    const char ** validationLayers = NULL;
    if (checkValidationLayerSupport(layersCount, validationLayersGroup))
    {
        validationLayers = validationLayersGroup;
    }
    else
    {
        layersCount = 0;
        logMessage("validation layers not found");
    }

    uint32_t extensionCount = 0;
    resultVulkan(vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE), code, 0);

    VkExtensionProperties * extensionsProperties = (VkExtensionProperties *)SDL_malloc(extensionCount * sizeof(VkExtensionProperties));
    if (extensionsProperties == NULL)
    {
        pushMessage(SDL_MESSAGEBOX_ERROR, "Error", "alloc memory failed (VkExtensionProperties)");
        cleanup(code);
    }
    resultVulkan(vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, extensionsProperties), code, 1, extensionsProperties);

    uint32_t requiredExtensionCount = 0;

    uint32_t sdlExtensionCount = 0;
    const char * const * sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    requiredExtensionCount += sdlExtensionCount;

    char ** requiredExtension = (char **)SDL_malloc(sizeof(char*) * requiredExtensionCount);
    if (requiredExtension == NULL)
    {
        pushMessage(SDL_MESSAGEBOX_ERROR, "Error", "alloc memory failed (requiredExtension)");
        SDL_free(extensionsProperties);
        cleanup(code);
    }
    setRequiredExtensions(sdlExtensionCount, (const char**)sdlExtensions, requiredExtension);

    resultVulkan(!extensionSupportedCheck_Must(sdlExtensionCount, (char **)sdlExtensions, extensionCount, extensionsProperties), code, 2, requiredExtension, extensionsProperties);

    
    uint32_t optionalExtensionCount = 0;

    char ** optionalExtension = (char**)SDL_malloc(sizeof(char*) * optionalExtensionCount);
    if (optionalExtension == NULL)
    {
        pushMessage(SDL_MESSAGEBOX_ERROR, "Error", "alloc memory failed (requiredExtension)");
        SDL_free(requiredExtension);
        SDL_free(extensionsProperties);
        cleanup(code);
    }
    setOptionalExtensions(0, NULL, NULL);

    bool * enabledOptionalExtensions = extensionSupportedCheck_Optional(optionalExtensionCount, optionalExtension, extensionCount, extensionsProperties);

    SDL_free(extensionsProperties);
    requiredExtension = (char**)SDL_realloc(requiredExtension, sizeof(char*) * (optionalExtensionCount + requiredExtensionCount));
    for (int i = 0, count = 0;i < optionalExtensionCount;i++)
    {
        if (enabledOptionalExtensions[i])
        {
            requiredExtension[requiredExtensionCount + count] = optionalExtension[i];
            count++;
        }
        if (i == (optionalExtensionCount - 1))
        {
            requiredExtensionCount += count;
        }
    }
    SDL_free(optionalExtension);
    SDL_free(enabledOptionalExtensions);

    //printf("sdlExtensionNumber: %u\n", sdlExtensionCount);

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = VK_NULL_HANDLE;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = layersCount;
    createInfo.ppEnabledLayerNames = validationLayers;
    createInfo.enabledExtensionCount = requiredExtensionCount;
    createInfo.ppEnabledExtensionNames = (const char * const *)requiredExtension;

    resultVulkan(vkCreateInstance(&createInfo, VK_NULL_HANDLE, pInstance), code, 1, requiredExtension);

    SDL_free(requiredExtension);

    logMessage("instance created");
}
#include "vk_code_h/vk_surface.h"
#include "vk_code_h/vk_judge.h"

#include "SDL3/SDL_vulkan.h"

void createSurface(SDL_Window * pWindow, VkInstance * pInstance, VkSurfaceKHR * pSurface)
{
    FuncCode code = createSurfaceF;
    resultVulkan(!SDL_Vulkan_CreateSurface(pWindow, *pInstance, NULL, pSurface), code, 0);

    //printf("surface created\n");
}
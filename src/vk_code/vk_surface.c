#include "vk_code_h/vk_surface.h"
#include "vk_code_h/vk_struct.h"
#include "vk_code_h/vk_judge.h"

#include "SDL3/SDL_vulkan.h"

extern VK_ALL allInOne;

void createSurface(SDL_Window * pWindow)
{
    FuncCode code = createSurfaceF;
    resultVulkan(!SDL_Vulkan_CreateSurface(pWindow, *allInOne.pInstance, allInOne.pAllocationCallbacks, allInOne.pSurface2D), code, 0);

    //printf("surface created\n");
}
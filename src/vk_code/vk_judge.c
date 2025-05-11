#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_recreate.h"

#include "G_game.h"
#include "G_log.h"
#include "G_allocator.h"

extern bool game_is_running;

void resultVulkan(VkResult result, Uint32 num, ...)
{
    switch (result)
    {
        case VK_SUCCESS:
        return;

        case VK_SUBOPTIMAL_KHR:
        print("result: %d", result);
        // recreateSwapchain();
        break;

        case VK_ERROR_OUT_OF_DATE_KHR:
        print("result: %d", result);
        // recreateSwapchain();
        break;

        case VK_ERROR_SURFACE_LOST_KHR:
        print("result: %d", result);
        // recreateSwapchain();
        break;

        default:
        print("result: %d", result);
        if (num > 0)
        {
            va_list ap;
            va_start(ap, num);
            for (Uint32 i = 0;i < num;i++)
            {
                G_free(va_arg(ap, void *));
                print("freed(%u)", i);
            }
            va_end(ap);
        }
        goto end;
    }

// Quick Quit Needed
    if (0)
    {
end:
        game_is_running = false;
        destroy();
    }
}
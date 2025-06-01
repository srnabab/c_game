#include "G_game.h"
#include "G_map.h"
#include "G_log.h"
#include "G_struct.h"
#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;
extern bool game_is_running;
extern bool resolutionChanged;

// Render function to draw game objects
int render(void * arg) 
{
    print("render init\n");
    Uint32 render_frame = 0;
    Uint32 currentFrame = allInOne.currentFrame;
    G_Thread_Pool threadPool = {};
    G_Task task = {};
    Uint8 copy = 0;
    copy = createThreadPool(&threadPool, 3, false);
    if (copy == false) 
    {
        game_is_running = false;
        return -1;
    }

    while (game_is_running)
    {
        SDL_WaitSemaphore(allSync.renderSemaphore);

        if (resolutionChanged)
        {
            recreateSwapchain(currentFrame);
            resolutionChanged = false;
        }

        // bottomMoved = moveBottomImage(currentFrame);
        // task.arg = &currentFrame;
        // task.func = recordBufferCopy;
        // task.executeFunc
        copy = recordBufferCopy(currentFrame);

        drawFrame(First_Scene, currentFrame, allInOne.extent2D.width, allInOne.extent2D.height, false, copy);

        allInOne.currentFrame = (allInOne.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        currentFrame = allInOne.currentFrame;

        //print("render frames: %d ----%s", render_frame, timeNow);

        render_frame++;

        SDL_SignalSemaphore(allSync.signalSemaphore);
    }

    destroyThreadPool(&threadPool);
    return 0;
}
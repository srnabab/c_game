#include "G_game.h"
#include "G_map.h"
#include "G_log.h"
#include "G_struct.h"
#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;
extern bool game_is_running;
extern bool draw_done;

// Render function to draw game objects in the SDL window_2D
int render(void * arg) 
{
    print("render init\n");
    Uint32 render_frame = 0;
    Uint32 currentFrame = allInOne.currentFrame;
    bool bottomMoved = false;
    while (game_is_running)
    {
        SDL_WaitSemaphore(allSync.renderSemaphore);

        // if (resolutionChanged)
        // {
        //     recreateSwapchain(currentFrame);
        //     resolutionChanged = false;
        // }

        bottomMoved = moveBottomImage(currentFrame);

        drawFrame(First_Scene, currentFrame, allInOne.extent2D.width, allInOne.extent2D.height, bottomMoved);

        allInOne.currentFrame = (allInOne.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        currentFrame = allInOne.currentFrame;

        draw_done = true;

        //print("render frames: %d ----%s", render_frame, timeNow);

        render_frame++;

        SDL_SignalSemaphore(allSync.signalSemaphore);
    }
    return 0;
}
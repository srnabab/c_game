#include "G_game.h"
#include "G_map.h"
#include "G_log.h"
#include "G_test_if.h"
#include "G_struct.h"
#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;
extern bool game_is_running;
extern bool resolutionChanged;
extern Scene scene;

static void recordBufferCopyExecute(void * arg)
{
    G_Task * task = (G_Task *)arg;
    void ** datas = (void**)task->arg;
    bool (*func)(Uint32) = task->func;
    *(Uint8*)datas[1] = func(*(Uint32*)datas[0]);
}
// Render function to draw game objects
int render(void * arg) 
{
    print("render init\n");
    Uint32 render_frame = 0;
    Uint32 currentFrame = allInOne.currentFrame;
    G_Thread_Pool threadPool = {0};
    G_Task task = {0};
    Uint8 copy = 0;
    int * threadIndex = NULL;
    void * datas[4];
    copy = createThreadPool(&threadPool, 2, "renderTask", false);
    if (copy == false) 
    {
        game_is_running = false;
        SDL_SignalSemaphore(allSync.signalSemaphore);
    }

    while (game_is_running)
    {
        SDL_WaitSemaphore(allSync.renderSemaphore);

        if (resolutionChanged)
        {
            recreateSwapchain(currentFrame); 
            resolutionChanged = false;
        }

        copy = recordBufferCopy(currentFrame);

        drawFrame(scene, currentFrame, allInOne.extent2D.width, allInOne.extent2D.height, false, copy, &threadPool);

        allInOne.currentFrame = (allInOne.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        currentFrame = allInOne.currentFrame;

        //print("render frames: %d ----%s", render_frame, timeNow);

        render_frame++;

        SDL_SignalSemaphore(allSync.signalSemaphore);
    }

    destroyThreadPool(&threadPool);
    return 0;
}

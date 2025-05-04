#include "G_game.h"
#include "G_log.h"
#include "G_struct.h"

extern G_SYNC allSync;
extern bool game_is_running;

bool update_done, draw_done = false;

int signal_trans(void * arg)
{
    print("signal init\n");
    while (game_is_running)
    {
        if (update_done && draw_done)
        {
            // SDL_SignalCondition(done_cond);
            update_done = draw_done = false;
            SDL_SignalSemaphore(allSync.updateSemaphore);
            SDL_SignalSemaphore(allSync.renderSemaphore);
        }

        SDL_WaitSemaphore(allSync.signalSemaphore);
        SDL_WaitSemaphore(allSync.signalSemaphore);
    }

    return 0;
}
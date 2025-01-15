#include "SDL3/SDL_log.h"
#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "G_file/G_file.h"
#include "G_log.h"
#include "G_graphic.h"
#include "G_game.h"
#include "G_world.h"

#include "vk_code_h/vk_judge.h"

bool game_is_running = false;

SDL_Thread * sdl_pid_update = NULL;
SDL_Thread * sdl_pid_draw = NULL;
SDL_Thread * sdl_pid_signal = NULL;
// SDL_Thread * sdl_pid_control = NULL;

// SDL_Condition * done_cond = NULL;

SDL_Mutex * sdl_mutex = NULL;

SDL_Semaphore * main_semaphore1 = NULL;
SDL_Semaphore * main_semaphore2 = NULL;

extern bool update_done, draw_done;

uint64_t last_frame_time = 0;

bool input_end = false;
// Main function
int main(int argc, char* argv[]) 
{
    initFileSystem(argc, argv);
    initLog();
    game_is_running = initWindow();
    logMessage("game_is_running: %d", game_is_running);

    setup();

    SDL_Delay(3000);
    last_frame_time = SDL_GetPerformanceCounter();
    SDL_SignalSemaphore(main_semaphore1);
    SDL_SignalSemaphore(main_semaphore2);

    while (true) 
    {
        process_input(NULL);

        if (!game_is_running)
        {
            // SDL_WaitThread(sdl_pid_control, NULL);
            SDL_WaitThread(sdl_pid_signal, NULL);
            SDL_Log("signal end\n");
            SDL_WaitThread(sdl_pid_update, NULL);
            SDL_Log("update end\n");
            SDL_WaitThread(sdl_pid_draw, NULL);
            SDL_Log("draw end\n");

            cleanup(FuncCodeMax);
        }
        
        if (input_end)
            break;
    }
    //SDL_BroadcastCondition(main_cond);
    
    cleanWorld();
    destroy_window();

    return 0;
}
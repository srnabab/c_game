#include "constants.h"
#include "game.h"
#include "glfw3.h"
#include "flow.h"

bool game_is_running = false;

SDL_Thread * sdl_pid_update = NULL;
SDL_Thread * sdl_pid_draw = NULL;
SDL_Thread * sdl_pid_signal = NULL;
SDL_Thread * sdl_pid_control = NULL;

SDL_Condition * done_cond = NULL;

SDL_Mutex * sdl_mutex = NULL;

SDL_Semaphore * main_semaphore1 = NULL;
SDL_Semaphore * main_semaphore2 = NULL;

extern bool update_done, draw_done, signal_done;

uint64_t last_frame_time = 0;

// Main function
int main(int argc, char* args[]) 
{
    changeArgv_0();
    game_is_running = initWindow();
    printf("%d\n", game_is_running);

    setup();

    SDL_Delay(3000);
    last_frame_time = SDL_GetPerformanceCounter();
    SDL_SignalSemaphore(main_semaphore1);
    SDL_SignalSemaphore(main_semaphore2);

    while (game_is_running) 
    {
        process_input(NULL);
    }
    //SDL_BroadcastCondition(main_cond);
    

    SDL_WaitThread(sdl_pid_control, NULL);
    SDL_WaitThread(sdl_pid_signal, NULL);
    printf("signal end\n");
    SDL_WaitThread(sdl_pid_update, NULL);
    printf("update end\n");
    SDL_WaitThread(sdl_pid_draw, NULL);
    printf("draw end\n");

    SDL_DestroyCondition(done_cond);
    //SDL_DestroyCondition(main_cond);
    SDL_DestroyMutex(sdl_mutex);
    printf("window destroyed\n");

    //getchar();

    return 0;
}
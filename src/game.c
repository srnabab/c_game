#include "constants.h"
#include "game.h"
#include "flow.h"
#include "vulkan_code.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

extern SDL_Thread * sdl_pid_update, * sdl_pid_draw, * sdl_pid_signal, * sdl_pid_control;

//extern SDL_Condition * main_cond;
extern SDL_Condition * done_cond;

uint64_t frequency;

extern SDL_Mutex * sdl_mutex;
SDL_Mutex * sdl_mutex_2;

bool update_done, draw_done = false;
bool stop = false;

// Global variables
extern bool game_is_running;
extern uint64_t last_frame_time;
//SDL_Window *window = NULL;
//SDL_Renderer *renderer = NULL;

double testNum = 0.0f;

extern SDL_Window * window;
extern VK_ALL allInOne;

extern SDL_Semaphore * main_semaphore1;
extern SDL_Semaphore * main_semaphore2;
extern SDL_Semaphore * main_semaphore3;
// Setup function that runs once at the beginning of our program
void setup(void) 
{
    //main_cond = SDL_CreateCondition();
    frequency = SDL_GetPerformanceFrequency();
    SDL_StopTextInput(window);
    done_cond = SDL_CreateCondition();

    sdl_mutex = SDL_CreateMutex();
    sdl_mutex_2 = SDL_CreateMutex();

    main_semaphore1 = SDL_CreateSemaphore(0);
    main_semaphore2 = SDL_CreateSemaphore(0);
    //main_semaphore3 = SDL_CreateSemaphore(0);

    initVulkan();

    sdl_pid_control = SDL_CreateThread(&flow_control, "control", NULL);
    //SDL_Delay(1000);
    sdl_pid_update = SDL_CreateThread(&update, "update", NULL);
    sdl_pid_draw = SDL_CreateThread(&render, "render", NULL);
    sdl_pid_signal = SDL_CreateThread(&signal_trans, "signal", NULL);

    last_frame_time = SDL_GetTicks();
}

static bool cameraMove[4];
bool pictureMove[4];

// Function to poll SDL events and process keyboard input
int process_input(void * arg)
{
    static uint32_t preKeyState;
    //debug_printf("\nmain loop");

        SDL_Event event;

        while(SDL_PollEvent(&event))
        {   
            if (event.type == SDL_EVENT_MOUSE_MOTION | event.type == SDL_EVENT_MOUSE_BUTTON_DOWN | event.type == SDL_EVENT_MOUSE_BUTTON_UP)
                continue;

            if (event.type == SDL_EVENT_KEY_UP)
            {
                SDL_Keycode key = event.key.key;
                if (key == SDLK_RIGHT)
                {
                    cameraMove[3] = false;
                }
                if (key == SDLK_LEFT)
                {
                    cameraMove[2] = false;
                }
                if (key == SDLK_UP)
                {
                    cameraMove[0] = false;
                }
                if (key == SDLK_DOWN)
                {
                    cameraMove[1] = false;
                }
                if (key == SDLK_A)
                {
                    pictureMove[2] = false;
                }
                if (key == SDLK_D)
                {
                    pictureMove[3] = false;
                }
                if (key == SDLK_W)
                {
                    pictureMove[0] = false;
                }
                if (key == SDLK_S)
                {
                    pictureMove[1] = false;
                }
            }
            
            if (preKeyState == SDL_EVENT_KEY_DOWN && event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                if (key == SDLK_RIGHT)
                {
                    cameraMove[3] = true;
                }
                if (key == SDLK_LEFT)
                {
                    cameraMove[2] = true;
                }
                if (key == SDLK_UP)
                {
                    cameraMove[0] = true;
                }
                if (key == SDLK_DOWN)
                {
                    cameraMove[1] = true;
                }
                if (key == SDLK_A)
                {
                    pictureMove[2] = true;
                }
                if (key == SDLK_D)
                {
                    pictureMove[3] = true;
                }
                if (key == SDLK_W)
                {
                    pictureMove[0] = true;
                }
                if (key == SDLK_S)
                {
                    pictureMove[1] = true;
                }
            }

            if (preKeyState == SDL_EVENT_KEY_UP && event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                if (key == SDLK_RIGHT)
                {
                    cameraMove[3] = true;
                }
                if (key == SDLK_LEFT)
                {
                    cameraMove[2] = true;
                }
                if (key == SDLK_UP)
                {
                    cameraMove[0] = true;
                }
                if (key == SDLK_DOWN)
                {
                    cameraMove[1] = true;
                }
                if (key == SDLK_A)
                {
                    pictureMove[2] = true;
                }
                if (key == SDLK_D)
                {
                    pictureMove[3] = true;
                }
                if (key == SDLK_W)
                {
                    pictureMove[0] = true;
                }
                if (key == SDLK_S)
                {
                    pictureMove[1] = true;
                }
            }
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                // if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "", "Really Exit?", window) < 0)
                {
                    // fprintf(stderr, "Error show messagebox\n");
                    // exit(-1);
                }
                game_is_running = false;
                break;

                case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    SDL_SignalSemaphore(main_semaphore1);
                    SDL_SignalSemaphore(main_semaphore2);
                    game_is_running = false;
                }
                else if (event.key.key == SDLK_F11)
                {
                    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                    allInOne.pExtent2D->width = 1600;
                    allInOne.pExtent2D->height = 900;
                    SDL_SetWindowSize(window, allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                    printf("sdl width: %u, height: %u\n", allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                }
                else if (event.key.key == SDLK_1)
                {
                    allInOne.pExtent2D->width++;
                    allInOne.pExtent2D->height++;
                    SDL_SetWindowSize(window, allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                }
                break;
            }
            printf("preKey: %d, key: %d\n", preKeyState, event.type);
            preKeyState = event.type;
        }
    
}

// Update function with a fixed time step
int update(void * arg)
{
    uint32_t update_frame = 0;
    UniformBufferObject * pGraphicUbo = allInOne.pGraphicUbo;
    float * pCamera_X = allInOne.pCamera_X;
    float * pCamera_Y = allInOne.pCamera_Y;
    uint32_t currentFrame = *allInOne.pCurrentFrame;
    printf("update init\n");
    while (game_is_running)
    {
        //SDL_LockMutex(sdl_mutex);
        //int code = SDL_WaitConditionTimeout(main_cond, sdl_mutex, -1);
        SDL_WaitSemaphore(main_semaphore1);
        //printf("update code: %d\n", code);
        //SDL_UnlockMutex(sdl_mutex);

        // Get delta_time factor converted to seconds to be used to update objects
        float delta_time = (SDL_GetPerformanceCounter() - last_frame_time) / (float)frequency;
        last_frame_time = SDL_GetPerformanceCounter();
        //delta_time = 16.666f / 1000.0;

        // Store the milliseconds of the current frame to be used in the next one

        testNum += 2 * delta_time;

        if (cameraMove[0])
        {
            *pCamera_Y -= 0.2f * delta_time;
            //printf("camera y: %f, enabled: %d, delta time: %lf, last_frame_time: %lu\n", *allInOne.pCamera_Y, cameraMove[0], delta_time, last_frame_time);
        }
        if (cameraMove[1])
        {
            *pCamera_Y += 0.2f * delta_time;
        }
        if (cameraMove[2])
        {
            *pCamera_X += 0.2f * delta_time;
        }
        if (cameraMove[3])
        {
            *pCamera_X -= 0.2f * delta_time;
        }

        size_t bufferSize = sizeof((*allInOne.ppVertices)[0]) * *allInOne.pVerticesCount;
        SDL_LockMutex(sdl_mutex_2);
        if (pictureMove[0])
        {
            *allInOne.pPictureY -= 200 * delta_time;
            //printf("y: %f, enabled: %d, delta time: %lf, last_frame_time: %lu\n", *allInOne.pPictureY, pictureMove[0], delta_time, last_frame_time);
        }
        if (pictureMove[1])
        {
            *allInOne.pPictureY += 200 * delta_time;
        }
        if (pictureMove[2])
        {
            *allInOne.pPictureX += 200 * delta_time;
        }
        if (pictureMove[3])
        {
            *allInOne.pPictureX -= 200 * delta_time;
        }
        if (pictureMove[0] | pictureMove[1] | pictureMove[2] | pictureMove[3])
        {
            updatePosition(*allInOne.pPictureX, *allInOne.pPictureY, allInOne.pExtent2D, allInOne.ppVertices, 1);
            memcpy(*allInOne.ppMovingBufferMapped, *allInOne.ppVertices, bufferSize);
        }
        SDL_UnlockMutex(sdl_mutex_2);

        //updateUniformBuffer(*allInOne.pCurrentFrame, allInOne.pExtent2D, allInOne.pGraphicUbo, allInOne.pppGraphicUniformBufferMapped, *allInOne.pCamera_X, *allInOne.pCamera_Y, allInOne.pComputeUbo, allInOne.pppComputeUniformBufferMapped, delta_time);   
        //printf("time: %.2f\n", time);

        glm_mat4_identity(pGraphicUbo->model);
        //glm_rotate(pUbo->model, time * glm_rad(90.0f), (vec3){0.0f, 0.0f, 1.0f});

        glm_lookat((vec3){*pCamera_X, *pCamera_Y, 1.5f}, (vec3){*pCamera_X, *pCamera_Y, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pGraphicUbo->view);

        float aspect = (float)allInOne.pExtent2D->width / allInOne.pExtent2D->height;
        //printf("aspect : %.2f\n", aspect);
        glm_mat4_identity(pGraphicUbo->proj);
        //glm_perspective(glm_rad(45.0f), aspect, 0.1f, 10.0f, pUbo->proj);
        glm_ortho_vulkan(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f, pGraphicUbo->proj);

        pGraphicUbo->proj[1][1] *= -1;

        allInOne.pComputeUbo->deltaTime = delta_time;

        SDL_LockMutex(sdl_mutex_2);
        memcpy((*allInOne.pppGraphicUniformBufferMapped)[*allInOne.pCurrentFrame], pGraphicUbo, sizeof(UniformBufferObject));

        memcpy((*allInOne.pppComputeUniformBufferMapped)[*allInOne.pCurrentFrame], allInOne.pComputeUbo, sizeof(ComputeUniformBufferObject));
        SDL_UnlockMutex(sdl_mutex_2);

        //printf("test: %lf\n", testNum);
        //printf("delta time:%f\n", delta_time);
        update_frame++;

        update_done = true;
        //SDL_SignalSemaphore(semaphore1);
    }
}

// Render function to draw game objects in the SDL window
int render(void * arg) 
{
    printf("render init\n");
    uint32_t render_frame = 0;
    while (game_is_running)
    {
        SDL_WaitSemaphore(main_semaphore2);
        //SDL_Delay(2);

        drawFrame(&allInOne);
        //printf("test: %f\n", testNum);
        //printf("render frames: %d\n", render_frame);
        render_frame++;
        draw_done = true;
    }
}

static bool control_done = 0;

int signal_trans(void * arg)
{
    printf("signal init\n");
    while (game_is_running)
    {
        if (update_done && draw_done)
        {
            SDL_SignalCondition(done_cond);
        }
    }
    while (!game_is_running)
    {
        SDL_SignalCondition(done_cond);
        if (control_done)
            break;
    }
}

int flow_control(void * arg)
{
    //printf("main lock\n");
    while (game_is_running)
    {
        control_done = false;
        SDL_LockMutex(sdl_mutex);
        if (game_is_running)
        {
            control_done = SDL_WaitConditionTimeout(done_cond, sdl_mutex, -1);
        }
            update_done = draw_done = false;
            SDL_SignalSemaphore(main_semaphore1);
            SDL_SignalSemaphore(main_semaphore2);
            //SDL_BroadcastCondition(main_cond);
            //printf("main signal\n");
            
            SDL_UnlockMutex(sdl_mutex);
            //printf("unlock(%d)\n", code);
    }
}

// Function to destroy SDL window and renderer
void destroy_window(void) 
{
    SDL_DestroyCondition(done_cond);
    SDL_DestroyMutex(sdl_mutex);
    SDL_DestroyMutex(sdl_mutex_2);
    SDL_DestroySemaphore(main_semaphore1);
    SDL_DestroySemaphore(main_semaphore2);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
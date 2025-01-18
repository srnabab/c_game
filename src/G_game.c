#include "G_game.h"
#include "G_graphic.h"

#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_move.h"

#include "G_custom_math.h"

#include "G_music.h"
#include "G_world.h"
#include "G_timer.h"
#include "G_text.h"
#include "G_scene.h"
#include "G_pop_window.h"
#include "G_constants.h"
#include "G_stack.h"
#include "G_log.h"
#include "G_file/G_file.h"

extern SDL_Thread * sdl_pid_update, * sdl_pid_draw, * sdl_pid_signal, * sdl_pid_control;

uint64_t frequency;

extern SDL_Mutex * sdl_mutex;
SDL_Mutex * sdl_mutex_2;

static SDL_Mutex * pause_mutex = NULL;
static SDL_Condition * pause_condition = NULL;

bool update_done, draw_done = false;

// Global variables
extern bool game_is_running;
extern uint64_t last_frame_time;

double testNum = 0.0f;

extern SDL_Window * window;
extern SDL_DisplayID displayId;
extern VK_ALL allInOne;
extern Recreate recreateSwap;

extern SDL_Semaphore * main_semaphore1;
extern SDL_Semaphore * main_semaphore2;
SDL_Semaphore * signal_semaphore = NULL;

static SDL_MessageBoxData * boxData;

// Setup function that runs once at the beginning of our program
void setup(void) 
{
    frequency = SDL_GetPerformanceFrequency();

    SDL_StopTextInput(window);

    sdl_mutex = SDL_CreateMutex();
    sdl_mutex_2 = SDL_CreateMutex();

    pause_mutex = SDL_CreateMutex();
    pause_condition = SDL_CreateCondition();

    main_semaphore1 = SDL_CreateSemaphore(0);
    main_semaphore2 = SDL_CreateSemaphore(0);
    signal_semaphore = SDL_CreateSemaphore(0);
    //main_semaphore3 = SDL_CreateSemaphore(0);

    static SDL_MessageBoxButtonData buttons[2] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "No"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Yes"},
    };

    static SDL_MessageBoxData messageBoxData = {0};
    messageBoxData.flags = SDL_MESSAGEBOX_WARNING;
    messageBoxData.window = window;
    messageBoxData.title = "Do you want to quit?";
    messageBoxData.message = "Yes or No?";
    messageBoxData.numbuttons = 2;
    messageBoxData.buttons = buttons;
    messageBoxData.colorScheme = NULL;

    boxData = &messageBoxData;

    initWorld();

    initTimerSystem();
    initTextSystem();

    initVulkan();
    initPopWindow();
    initMusicManagement();
    loadMusic((char*)getPath(MainBackgroundMusic1Wav), "test");

    sdl_pid_update = SDL_CreateThread(&update, "update", NULL);
    sdl_pid_draw = SDL_CreateThread(&render, "render", NULL);
    sdl_pid_signal = SDL_CreateThread(&signal_trans, "signal", NULL);

    last_frame_time = SDL_GetTicks();
}

static bool cameraMove[4];
bool pictureMove[4];
static bool changeScene = false;

// static bool pause = false;
// static bool pause_signal_send = false;

bool scale = false;

static bool ballAdd = false;
static uint8_t leftButtonClickedTimes = 0;
static bool leftButtonEnabled = true;
static uint32_t ballCount = 2;

static uint32_t textLine = 0;
static bool textDisplay = false;

extern uint32_t logical_width;
extern uint32_t logical_height;

extern float physicalCoffectX;
extern float physicalCoffectY;

static Scene scene = First_Scene;
static Scene preScene = First_Scene;

// Function to poll SDL events and process keyboard input
int process_input(void * arg)
{
    static uint32_t preKeyState = 0;
    static uint8_t pressedKey = 0;
    static int buttonId = 0;
    //debug_printf("\nmain loop");

    if (willPopWindow())
    {
        preScene = scene;
        scene = Pause_Scene;
        
        popWindow();

        scene = preScene;
        preScene = Pause_Scene;
        SDL_SignalSemaphore(main_semaphore1);
        SDL_SignalSemaphore(main_semaphore2);
        // pause_signal_send = true;
    }

    const bool * keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_A])
    {
        pictureMove[2] = true;
    }

    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        SDL_Keycode key = event.key.key;
        logMessage("preKeyState: %u, keyState: %u, key: %s(%u)", preKeyState, event.type, SDL_GetKeyName(key), key);
        // logMessage("pressed Key:%u", pressedKey);
        
        if (event.type == SDL_EVENT_WINDOW_MINIMIZED)
        {
            ;
        }

        if (event.type == SDL_EVENT_WINDOW_RESTORED)
        {
            SDL_RaiseWindow(window);
        }
        
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.down && !event.key.repeat)
            {
                pressedKey++;
            }
            if (key == SDLK_ESCAPE)
            {
                Mix_HaltMusic();
                SDL_SignalSemaphore(main_semaphore1);
                SDL_SignalSemaphore(main_semaphore2);
                game_is_running = false;
            }
            if (key == SDLK_F11)
            {
                recreateSwap.pOldExtent2D->width = allInOne.pExtent2D->width;
                recreateSwap.pOldExtent2D->height = allInOne.pExtent2D->height;
                allInOne.pExtent2D->width = 1600;
                allInOne.pExtent2D->height = 900;
                
                SDL_SetWindowSize(window, allInOne.pExtent2D->width, allInOne.pExtent2D->height);

                physicalCoffectX = (float)allInOne.pExtent2D->width / logical_width;
                physicalCoffectY = (float)allInOne.pExtent2D->height / logical_height;
                
                logMessage("sdl width: %u, height: %u", allInOne.pExtent2D->width, allInOne.pExtent2D->height);
            }
            if (key == SDLK_F10)
            {
                logMessage("F10");

                SDL_DisplayMode displayMode = {0};
                
                SDL_GetClosestFullscreenDisplayMode(displayId, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 0, false, &displayMode);
                SDL_SetWindowFullscreen(window, 1);
                SDL_SetWindowFullscreenMode(window, &displayMode);
                SDL_RaiseWindow(window);

                logMessage("fullscreen");
            }
            if (key == SDLK_F9)
            {
                logMessage("F9");

                SDL_SetWindowFullscreen(window, 0);
                recreateSwap.pOldExtent2D->width = allInOne.pExtent2D->width;
                recreateSwap.pOldExtent2D->height = allInOne.pExtent2D->height;
                
                SDL_SetWindowSize(window, allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                SDL_RaiseWindow(window);

                logMessage("windowed");
            }
            if (key == SDLK_PAUSE)
            {
                if (scene == Pause_Scene)
                {
                    scene = preScene;
                    preScene = Pause_Scene;
                    SDL_SignalSemaphore(main_semaphore1);
                    SDL_SignalSemaphore(main_semaphore2);
                }
                else
                {
                    preScene = scene;
                    scene = Pause_Scene;
                }
            }
        }

        if (scene == First_Scene)
        {

            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                //logMessage("mouse moving: %d", event.type);
                // logMessage("mouse: (%f, %f)", event.motion.x, event.motion.y);
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                ;
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (changeScene)
                {
                    preScene = scene;
                    scene = Menu_Scene;
                    changeScene = false;
                }
                else if (leftButtonEnabled && (ballCount < BALLCOUNT))
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        leftButtonClickedTimes++;
                        ballCount++;
                        ballAdd = true;
                    }
                }
            }

            if (event.type == SDL_EVENT_KEY_UP)
            {
                if (preKeyState == SDL_EVENT_KEY_DOWN)
                {
                    // if (key == SDLK_Q)
                    // {
                    //     scale = true;
                    //     // logMessage("scale: %d", scale);
                    // }
                }
                if (key == SDLK_T)
                {
                    textLine++;
                    textDisplay = true;
                    logMessage("textline: %u", textLine);
                    // pushMessage(SDL_MESSAGEBOX_INFORMATION, "text line", "textline: %u\n", textLine);
                }
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
                if (key == SDLK_LCTRL)
                {
                    changeScene = false;
                }
            }

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
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
                if (key == SDLK_LCTRL)
                {
                    changeScene = true;
                }
                // if (key == SDLK_A)
                // {
                //     pictureMove[2] = true;
                // }
                // if (key == SDLK_D)
                // {
                //     pictureMove[3] = true;
                // }
                // if (key == SDLK_W)
                // {
                //     pictureMove[0] = true;
                // }
                // if (key == SDLK_S)
                // {
                //     pictureMove[1] = true;
                // }
            }
        }
        else if (scene == Menu_Scene)
        {
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (changeScene)
                {
                    preScene = scene;
                    scene = First_Scene;
                    changeScene = false;
                }
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (key == SDLK_LCTRL)
                {
                    changeScene = true;
                }
            }
        }

        switch (event.type)
        {
            case SDL_EVENT_QUIT:

            preScene = scene;
            scene = Pause_Scene;

            SDL_ShowMessageBox(boxData, &buttonId);

            if (buttonId == 2)
            {
                SDL_SignalSemaphore(main_semaphore1);
                SDL_SignalSemaphore(main_semaphore2);
                game_is_running = false;
            }
            else if (buttonId == 1)
            {
                scene = preScene;
                preScene = Pause_Scene;
                SDL_SignalSemaphore(main_semaphore1);
                SDL_SignalSemaphore(main_semaphore2);
            }
            break;

            case SDL_EVENT_KEY_UP:
            pressedKey--;
            break;
        }
        preKeyState = event.type;
    }

    SDL_PumpEvents();

    return 0;
}

static int test(void * arg)
{
    SDL_Log("test test: %d\n", *((int *)arg));
    return 0;
}

extern EmptyStack ballStack;
extern vec2 UVs[MAX_CHARACTERS][FOUR_POINT];

// Update function with a fixed time step
int update(void * arg)
{
    uint32_t update_frame = 0;
    UniformBufferObject * pGraphicUbo = allInOne.pGraphicUbo;
    float * pCamera_X = allInOne.pCamera_X;
    float * pCamera_Y = allInOne.pCamera_Y;
    
    bool playedMusic = false;

    // float accumulator = 0.0f;
    const float timeStep = 1000.0f / 120.0f;

    uint64_t delta_time_ns = 0;
    float delta_time = 0.0f;
    uint64_t totalTimeNs = 0;
    float totalTime = 0.0f;
    
    bool recovreyPause = false;

    SDL_Log("update init\n");
    while (game_is_running)
    {
        SDL_WaitSemaphore(main_semaphore1);
        if (preScene == Pause_Scene && recovreyPause)
        {
            last_frame_time = SDL_GetPerformanceCounter();
            recovreyPause = false;
        }

        uint64_t tempTime = SDL_GetPerformanceCounter();
        delta_time_ns = ((tempTime - last_frame_time) * 1000000000ULL) / frequency;
        last_frame_time = tempTime;
        totalTimeNs += delta_time_ns;
        delta_time = delta_time_ns / ((float)S_TO_NS);
        totalTime = totalTimeNs / ((float)S_TO_NS);
        
        // Get delta_time factor converted to seconds to be used to update objects
        if (scene == Pause_Scene)
        {
            recovreyPause = true;
        }
        else
        {
            static int id_click = 0;
            if (leftButtonClickedTimes)
            {
                leftButtonEnabled = false;
                if (intervalIsDone(f32_ms_to_ns(58.8), &id_click, 1))
                {
                    SDL_LockMutex(sdl_mutex_2);

                    leftButtonClickedTimes = 0;
                    leftButtonEnabled = true;
                    id_click = 0;

                    SDL_UnlockMutex(sdl_mutex_2);
                }
            }

            if (cameraMove[0])
            {
                *pCamera_Y -= 0.2f * delta_time;
                //logMessage("camera y: %f, enabled: %d, delta time: %lf, last_frame_time: %lu ----%s", *allInOne.pCamera_Y, cameraMove[0], delta_time, last_frame_time, timeNow);
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

            if (scene == First_Scene)
            {
                if (!Mix_PlayingMusic() && !playedMusic)
                {
                    playMusic("test");
                    Mix_VolumeMusic(0);
                    playedMusic = true;
                }

                accumlateTime(delta_time_ns);
                // logMessage("TotalTime: %fms", totalTime);

                if (textDisplay)
                {
                    uint32_t textLen;
                    if (textLine == 1)
                        getTextUV("一二三", &textLen);
                    if (textLine == 2)
                        getTextUV("哈哈哈哈哈哈哈哈哈", &textLen);

                    for (uint32_t i = 0;i < textLen;i++)
                    {
                        for (int x = 0;x < 4;x++)
                        {
                            (*allInOne.ppVertices)[8000 + i * 4 + x].texCoord[0] = UVs[i][x][0];
                            (*allInOne.ppVertices)[8000 + i * 4 + x].texCoord[1] = UVs[i][x][1];
                        }
                    }
                    textDisplay = false;
                }

                testNum += 2 * delta_time;

                static int id_test = 0;
                int test_a = -1;
                addTimerFunc(u32_s_to_ns(1), &id_test, 10, test, &test_a);
            

                // SDL_LockMutex(sdl_mutex_2);
                // if (pictureMove[0])
                // {
                //     *allInOne.pPictureY += 200 * delta_time;
                //     //logMessage("y: %f, enabled: %d, delta time: %lf, last_frame_time: %lu ----%s", *allInOne.pPictureY, pictureMove[0], delta_time, last_frame_time, timeNow);
                // }
                // if (pictureMove[1])
                // {
                //     *allInOne.pPictureY -= 200 * delta_time;
                // }
                // if (pictureMove[2])
                // {
                //     *allInOne.pPictureX -= 200 * delta_time;
                // }
                // if (pictureMove[3])
                // {
                //     *allInOne.pPictureX += 200 * delta_time;
                // }
                // if (scale)
                // {
                //     glm_scale_self(allInOne.ppVertices, 2.0f, 1);
                // }
                // if (pictureMove[0] | pictureMove[1] | pictureMove[2] | pictureMove[3] | scale)
                // {
                //     updatePosition(*allInOne.pPictureX, *allInOne.pPictureY, allInOne.pExtent2D, allInOne.ppVertices, 1);
                //     scale = false;
                // }
                // SDL_UnlockMutex(sdl_mutex_2);

                uint32_t count = *allInOne.pVerticesCount;
                // size_t bufferSize = sizeof(Vertex) * count;

                // uint32_t indiceCount = *allInOne.pIndicesCount;
                // size_t bufferSize2 = sizeof(uint16_t) * indiceCount;

                if (ballAdd)
                {
                    *allInOne.pVerticesCount += 4;
                    *allInOne.pIndicesCount += 6;
                    //*allInOne.ppVertices = (Vertex *)realloc(*allInOne.ppVertices, count * sizeof(Vertex));
                    int x = SDL_rand(250);
                    if (SDL_rand(2))
                    {
                        x *= -1;
                    }
                    SDL_LockMutex(sdl_mutex_2);
                    // float averagePhysicalCoffect = (physicalCoffectX + physicalCoffectY) / 2.0f;
                    vertexInitialize(x * physicalCoffectX, 280 * physicalCoffectY, 16 * physicalCoffectY, 16 * physicalCoffectY, 0.9, false, *allInOne.pExtent2D, allInOne.ppVertices, count / 4);
                    SDL_UnlockMutex(sdl_mutex_2);

                    ballStack.pushFn(&ballStack, &x);

                    //SDL_Log("indices count: %u\n", indiceCount);
                    //*allInOne.ppIndices = (uint16_t *)realloc(*allInOne.ppIndices, indiceCount * sizeof(uint16_t));

                    ballAdd = false;
                }
            
                static int id_timeStep = 0;
                while (intervalIsDone(f32_ms_to_ns(timeStep), &id_timeStep, -1))
                {
                    updateCircle(allInOne.pExtent2D, allInOne.ppVertices);
                    // accumulator -= timeStep;
                }
            }

            //updateUniformBuffer(*allInOne.pCurrentFrame, allInOne.pExtent2D, allInOne.pGraphicUbo, allInOne.pppGraphicUniformBufferMapped, *allInOne.pCamera_X, *allInOne.pCamera_Y, allInOne.pComputeUbo, allInOne.pppComputeUniformBufferMapped, delta_time);   
            //SDL_Log("time: %.2f\n", time);

            glm_mat4_identity(pGraphicUbo->model);
            //glm_rotate(pUbo->model, time * glm_rad(90.0f), (vec3){0.0f, 0.0f, 1.0f});

            glm_lookat((vec3){*pCamera_X, *pCamera_Y, 1.5f}, (vec3){*pCamera_X, *pCamera_Y, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pGraphicUbo->view);

            float aspect = (float)allInOne.pExtent2D->width / allInOne.pExtent2D->height;
            
            glm_mat4_identity(pGraphicUbo->proj);
            //glm_perspective(glm_rad(45.0f), aspect, 0.1f, 10.0f, pUbo->proj);
            glm_ortho_vulkan(-aspect, aspect, -1.0f, 1.0f, 0.1f, 100.0f, pGraphicUbo->proj);

            pGraphicUbo->proj[1][1] *= -1;

            allInOne.pComputeUbo->deltaTime = delta_time;

            SDL_LockMutex(sdl_mutex_2);
            memcpy(*allInOne.ppVertexBufferMemMapped, *allInOne.ppVertices, 2100 * 4 * sizeof(Vertex));
            memcpy(*allInOne.ppIndexBufferMemMapped, *allInOne.ppIndices, 2100 * 6 * sizeof(uint16_t));
            
            memcpy((*allInOne.pppGraphicUniformBufferMapped)[*allInOne.pCurrentFrame], pGraphicUbo, sizeof(UniformBufferObject));

            memcpy((*allInOne.pppComputeUniformBufferMapped)[*allInOne.pCurrentFrame], allInOne.pComputeUbo, sizeof(ComputeUniformBufferObject));

            allInOne.pImageRotate->rotation = totalTime * glm_rad(580.0f);
            

            SDL_UnlockMutex(sdl_mutex_2);

            //logMessage("test: %lf", testNum);
            //logMessage("delta time:%f", delta_time);
            update_frame++;
            
            update_done = true;
        }

        SDL_SignalSemaphore(signal_semaphore);
    }
    return 0;
}

// Render function to draw game objects in the SDL window
int render(void * arg) 
{
    SDL_Log("render init\n");
    uint32_t render_frame = 0;
    while (game_is_running)
    {
        SDL_WaitSemaphore(main_semaphore2);

        drawFrame(scene);
        draw_done = true;

        //logMessage("render frames: %d ----%s", render_frame, timeNow);
        render_frame++;

        SDL_SignalSemaphore(signal_semaphore);
    }
    return 0;
}

int signal_trans(void * arg)
{
    SDL_Log("signal init\n");
    while (game_is_running)
    {
        if (update_done && draw_done)
        {
            // SDL_SignalCondition(done_cond);
            update_done = draw_done = false;
            SDL_SignalSemaphore(main_semaphore1);
            SDL_SignalSemaphore(main_semaphore2);
        }

        SDL_WaitSemaphore(signal_semaphore);
        SDL_WaitSemaphore(signal_semaphore);
    }

    return 0;
}

// Function to destroy SDL window and renderer
void destroy_window(void) 
{
    deInitTimerSystem();
    deInitMusicManagement();
    deInitPopWindow();
    destroyLog();
    SDL_DestroyCondition(pause_condition);
    SDL_DestroyMutex(pause_mutex);
    SDL_DestroyMutex(sdl_mutex);
    SDL_DestroyMutex(sdl_mutex_2);
    SDL_DestroySemaphore(main_semaphore1);
    SDL_DestroySemaphore(main_semaphore2);
    SDL_Quit();
}
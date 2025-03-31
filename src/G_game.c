#include "G_game.h"
#include "G_graphic.h"

#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

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
#include "G_struct.h"
#include "G_TileMap/G_TileSet.h"
#include "G_test.h"

// Global variables
bool game_is_running = false;

extern SDL_Window * window_2D;
extern SDL_Window * window_3D;
extern SDL_DisplayID displayId;
extern VK_ALL allInOne;

// file variables
static SDL_Thread * sdl_pid_update;
static SDL_Thread * sdl_pid_draw;
static SDL_Thread * sdl_pid_signal;

static bool update_done, draw_done = false;

static double testNum = 0.0f;

G_SYNC allSync = {};

static SDL_MessageBoxData * boxData;

static void initAllSync(void)
{
    allSync.updateMutex = SDL_CreateMutex();
    allSync.renderMutex = SDL_CreateMutex();
    allSync.logMutex = SDL_CreateMutex();
    allSync.printMutex = SDL_CreateMutex();
    allSync.popWindowMutex = SDL_CreateMutex();
    allSync.textureMutex = SDL_CreateMutex();
    allSync.timerMutex = SDL_CreateMutex();
    allSync.descriptorUpdateMutex = SDL_CreateMutex();
    allSync.vertexMutex = SDL_CreateMutex();
    allSync.tileSetMutex = SDL_CreateMutex();

    allSync.updateSemaphore = SDL_CreateSemaphore(0);
    allSync.renderSemaphore = SDL_CreateSemaphore(0);
    allSync.vertexSemaphore = SDL_CreateSemaphore(0);
    allSync.signalSemaphore = SDL_CreateSemaphore(0);
    allSync.logSemaphore = SDL_CreateSemaphore(0);
    allSync.worldSemaphore = SDL_CreateSemaphore(0);
}
// Setup function that runs once at the beginning of our program
void setup(int argc, char* argv[]) 
{
#if TEST
    int res = TestAll();
    if (res != 0) 
    {
        exit(-1);
    }
#endif

    int arg = initFileSystem(argc, argv);
    initAllSync();

    initLog(arg);

    game_is_running = initWindow_2D();
    logMessage("game_is_running: %d", game_is_running);
    if (game_is_running == false) return;

    SDL_StopTextInput(window_2D);

    static SDL_MessageBoxButtonData buttons[2] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "No"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Yes"},
    };

    static SDL_MessageBoxData messageBoxData = {0};
    messageBoxData.flags = SDL_MESSAGEBOX_WARNING;
    messageBoxData.window = window_2D;
    messageBoxData.title = "Do you want to quit?";
    messageBoxData.message = "Yes or No?";
    messageBoxData.numbuttons = 2;
    messageBoxData.buttons = buttons;
    messageBoxData.colorScheme = NULL;

    boxData = &messageBoxData;

    initWorld();

    initTimerSystem();
    initTextSystem();
    initTileMapSystem();

    initVulkan();
    initPopWindow();
    initMusicManagement();
    loadMusic((char*)getPath(MainBackgroundMusic1Wav), "test");

    sdl_pid_update = SDL_CreateThread(&update, "update", NULL);
    sdl_pid_draw = SDL_CreateThread(&render, "render", NULL);
    sdl_pid_signal = SDL_CreateThread(&signal_trans, "signal", NULL);
}

static bool cameraMove[4];
//delete needed
static bool pictureMove[4];
static bool changeScene = false;

// static bool pause = false;
// static bool pause_signal_send = false;

//delete needed
static bool scale = false;

static bool ballAdd = false;
static uint8_t leftButtonClickedTimes = 0;
static bool leftButtonEnabled = true;
static Uint32 ballCount = 2;

static Uint32 textLine = 0;
static bool textDisplay = false;

extern float physicalCoffectX;
extern float physicalCoffectY;

static bool sceneChanged = false;

static Scene scene = First_Scene;
static Scene preScene = Pause_Scene;

static bool resolutionChanged = false; 

// Function to poll SDL events and process keyboard input
bool process_input(void)
{
    static Uint32 preKeyState = 0;
    static uint8_t pressedKey = 0;
    static int buttonId = 0;

    if (willPopWindow())
    {
        preScene = scene;
        scene = Pause_Scene;
        
        popWindow();

        scene = preScene;
        preScene = Pause_Scene;
        SDL_SignalSemaphore(allSync.updateSemaphore);
        SDL_SignalSemaphore(allSync.renderSemaphore);
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
            preScene = scene;
            scene = Pause_Scene;
            SDL_Delay(50);
        }

        if (event.type == SDL_EVENT_WINDOW_RESTORED)
        {
            SDL_RaiseWindow(window_2D);

            scene = preScene;
            preScene = Pause_Scene;
            SDL_SignalSemaphore(allSync.updateSemaphore);
            SDL_SignalSemaphore(allSync.renderSemaphore);
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
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
                SDL_SignalSemaphore(allSync.signalSemaphore);
                SDL_SignalSemaphore(allSync.signalSemaphore);
                SDL_SignalSemaphore(allSync.signalSemaphore);
                game_is_running = false;
                
                return true;
            }
            if (key == SDLK_F6)
            {
                preScene = scene;
                scene = Pause_Scene;
                SDL_Delay(50);

                allInOne.pOldExtent2D->width = allInOne.pExtent2D->width;
                allInOne.pOldExtent2D->height = allInOne.pExtent2D->height;
                allInOne.pExtent2D->width = 1600;
                allInOne.pExtent2D->height = 900;
                
                SDL_SetWindowSize(window_2D, allInOne.pExtent2D->width, allInOne.pExtent2D->height);

#if WINDOW_3D_DEBUG
                SDL_SetWindowSize(window_3D, allInOne.pExtent2D->width, allInOne.pExtent2D->height);
#endif

                resolutionChanged = true;

                physicalCoffectX = (float)allInOne.pExtent2D->width / LOGICAL_WIDTH;
                physicalCoffectY = (float)allInOne.pExtent2D->height / LOGICAL_HEIGHT;
                
                logMessage("sdl width: %u, height: %u", allInOne.pExtent2D->width, allInOne.pExtent2D->height);

                scene = preScene;
                preScene = Pause_Scene;
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
            }
            if (key == SDLK_F10)
            {
                logMessage("F10");

                preScene = scene;
                scene = Pause_Scene;
                SDL_Delay(50);

                SDL_DisplayMode displayMode = {0};

                SDL_GetClosestFullscreenDisplayMode(displayId, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 0, false, &displayMode);
                SDL_SetWindowFullscreen(window_2D, 1);
                SDL_SetWindowFullscreenMode(window_2D, &displayMode);
                SDL_RaiseWindow(window_2D);

                resolutionChanged = true;

                logMessage("fullscreen");

                scene = preScene;
                preScene = Pause_Scene;
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
            }
            if (key == SDLK_F9)
            {
                logMessage("F9");

                preScene = scene;
                scene = Pause_Scene;
                SDL_Delay(50);

                SDL_SetWindowFullscreen(window_2D, 0);
                allInOne.pOldExtent2D->width = allInOne.pExtent2D->width;
                allInOne.pOldExtent2D->height = allInOne.pExtent2D->height;

                SDL_SetWindowSize(window_2D, allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                SDL_RaiseWindow(window_2D);

                resolutionChanged = true;

                logMessage("windowed");

                scene = preScene;
                preScene = Pause_Scene;
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
            }
            if (key == SDLK_PAUSE)
            {
                if (scene == Pause_Scene)
                {
                    scene = preScene;
                    preScene = Pause_Scene;
                    SDL_SignalSemaphore(allSync.updateSemaphore);
                    SDL_SignalSemaphore(allSync.renderSemaphore);
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
                    sceneChanged = true;
                }
                else if (leftButtonEnabled && (ballCount < 2000/*ball count*/))
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
                    sceneChanged = true;
                }
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (key == SDLK_LCTRL)
                {
                    changeScene = true;
                }
            }
            if (event.type == SDL_EVENT_KEY_UP)
            {
                if (key == SDLK_LCTRL)
                {
                    changeScene = false;
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
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
                game_is_running = false;

                return true;
            }
            else if (buttonId == 1)
            {
                scene = preScene;
                preScene = Pause_Scene;
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
            }
            break;

            case SDL_EVENT_KEY_UP:
            pressedKey--;
            break;
        }
        preKeyState = event.type;
    }

    SDL_PumpEvents();

    if (!game_is_running) return true;

    return false;
}

static int test(void * arg)
{
    print("test test: %d\n", *((int *)arg));
    return 0;
}

extern G_Stack ballStack;
extern vec2 UVs[MAX_CHARACTERS][FOUR_POINT];

// Update function with a fixed time step
int update(void * arg)
{
    Uint32 update_frame = 0;
    UniformBufferObject * pGraphicUbo = allInOne.pGraphicUbo;
    UniformBufferObject * pGraphic3DUbo = allInOne.pGraphic3DUbo;
    UniformBufferObject * pUIUbo = allInOne.pUIUbo;
    float * pCamera_X = allInOne.pCamera_X;
    float * pCamera_Y = allInOne.pCamera_Y;
    
    bool playedMusic = false;

    Uint64 frequency = SDL_GetPerformanceFrequency();;
    Uint64 delta_time_ns = 0;
    float delta_time = 0.0f;
    Uint64 totalTimeNs = 0;
    float totalTime = 0.0f;
    
    bool recovreyPause = false;
    bool sceneCleaned = false;

    Uint32 currentFrame;

    Uint32 vertexStart = 0;
    Uint32 vertexEnd = *allInOne.pVertices2DCount;

    textureVertexInit(-32, -32, 64, 64, 0.2f, allInOne.pVertices2DCount, *allInOne.ppVertices2D, getTexture(TEXTURE_LOADING));
    
    tileMapVertexInit(allInOne.pVertices2DCount, *allInOne.ppVertices2D);
    addModelMatrix(0, 0, -8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(100, 0, -8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(0, 100, -8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(0, 0, 1, allInOne.pStaticModelPool, TEXTURE_BOTTOM);
        
    SDL_Delay(300);
    
    Uint64 last_frame_time = SDL_GetPerformanceCounter();
    SDL_SignalSemaphore(allSync.updateSemaphore);
    SDL_SignalSemaphore(allSync.renderSemaphore);

    print("update init\n");
    while (game_is_running)
    {
        SDL_WaitSemaphore(allSync.updateSemaphore);
        if (preScene == Pause_Scene && recovreyPause)
        {
            last_frame_time = SDL_GetPerformanceCounter();
            recovreyPause = false;
        }

        Uint64 tempTime = SDL_GetPerformanceCounter();
        delta_time_ns = ((tempTime - last_frame_time) * 1000000000ULL) / frequency;
        last_frame_time = tempTime;
        totalTimeNs += delta_time_ns;
        delta_time = delta_time_ns / ((float)S_TO_NS);
        totalTime = totalTimeNs / ((float)S_TO_NS);

        currentFrame = *allInOne.pCurrentFrame;

        // vertexStart = *allInOne.pVerticesCount;

        // emptyTextureRefCount();
        
        // Get delta_time factor converted to seconds to be used to update objects
        if (scene == Pause_Scene)
        {
            recovreyPause = true;
        }
        else
        {
            if (sceneChanged) 
            {
                sceneCleaned = false;
                sceneChanged = false;
            }
            if (!sceneCleaned && preScene != Pause_Scene)
            {
                // sceneCleaned = cleanScene(preScene);
                sceneCleaned = true;
                logMessage("clean scene");
            }

            static int id_click = 0;
            if (leftButtonClickedTimes)
            {
                leftButtonEnabled = false;
                if (intervalIsDone(f32_ms_to_ns(58.8), &id_click, 1))
                {
                    SDL_LockMutex(allSync.updateMutex);

                    leftButtonClickedTimes = 0;
                    leftButtonEnabled = true;
                    id_click = 0;

                    SDL_UnlockMutex(allSync.updateMutex);
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
                    Uint32 textLen = 0;
                    if (textLine == 1) getTextUV("一二三", &textLen);
                    else if (textLine == 2) getTextUV("哈哈哈哈哈哈哈哈哈", &textLen);

                    if (textLine < 3)
                    {
                        getTexture(TEXTURE_FONT)->refCount = 0;
                        for (Uint32 i = 0;i < textLen;i++)
                        {
                            textureVertexInit_SetUV(-300.0 + (float)i * 24.0, -100.0, 24, 24, 0.1f, allInOne.pVertices2DCount, *allInOne.ppVertices2D, UVs[i], getTexture(TEXTURE_FONT));
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
                //     glm_scale_self(allInOne.ppVertices2D, 2.0f, 1);
                // }
                // if (pictureMove[0] | pictureMove[1] | pictureMove[2] | pictureMove[3] | scale)
                // {
                //     updatePosition(*allInOne.pPictureX, *allInOne.pPictureY, allInOne.pExtent2D, allInOne.ppVertices2D, 1);
                //     scale = false;
                // }
                // SDL_UnlockMutex(sdl_mutex_2);

                // size_t bufferSize = sizeof(Vertex) * count;

                // Uint32 indiceCount = *allInOne.pIndicesCount;
                // size_t bufferSize2 = sizeof(uint16_t) * indiceCount;

                if (ballAdd)
                {
                    //*allInOne.ppVertices2D = (Vertex *)realloc(*allInOne.ppVertices2D, count * sizeof(Vertex));
                    int x = SDL_rand(250);
                    if (SDL_rand(2))
                    {
                        x *= -1;
                    }
                    // float averagePhysicalCoffect = (physicalCoffectX + physicalCoffectY) / 2.0f;
                    textureVertexInit(x * physicalCoffectX, 280 * physicalCoffectY, 16 * physicalCoffectY, 16 * physicalCoffectY, 0.9, allInOne.pVertices2DCount, *allInOne.ppVertices2D, getTexture(TEXTURE_CIRCLE));

                    ballStack.pushFn(&ballStack, &x);
                    //print("indices count: %u\n", indiceCount);
                    //*allInOne.ppIndices = (uint16_t *)realloc(*allInOne.ppIndices, indiceCount * sizeof(uint16_t));

                    ballAdd = false;
                }
            
                static int id_timeStep = 0;
                while (intervalIsDone(f32_s_to_ns(TIME_STEP), &id_timeStep, -1))
                {
                    updateCircle(); 
                    // accumulator -= timeStep;
                }
            }
            else if (scene == Menu_Scene)
            {
            }

            //updateUniformBuffer(*allInOne.pCurrentFrame, allInOne.pExtent2D, allInOne.pGraphicUbo, allInOne.pppGraphicUniformBufferMapped, *allInOne.pCamera_X, *allInOne.pCamera_Y, allInOne.pComputeUbo, allInOne.pppComputeUniformBufferMapped, delta_time);   
            //print("time: %.2f\n", time);

            float aspect2 = 1.0f  * ((float)allInOne.pExtent2D->height / 600.0f);
            float aspect = ((float)allInOne.pExtent2D->width / allInOne.pExtent2D->height) * aspect2;

            glm_mat4_identity(pGraphicUbo->model);
            glm_lookat((vec3){*pCamera_X, *pCamera_Y, 100.0f}, (vec3){*pCamera_X, *pCamera_Y, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pGraphicUbo->view);
            glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, 0.001f, 100.0f, pGraphicUbo->proj);
            // glm_ortho(-aspect, aspect, -1.0f, 1.0f, 0.001f, 100.0f, pGraphicUbo->proj);
            // pGraphicUbo->proj[1][1] *= -1;

            glm_mat4_identity(pGraphic3DUbo->model);
            // glm_scale(pGraphic3DUbo->model, (vec3){0.2266666f, 0.2266666f, 0.2266666f});
            // glm_scale(pGraphic3DUbo->model, (vec3){0.5f ,0.5f, 0.5f});
            // glm_rotate(pGraphic3DUbo->model, glm_rad(180.0f), (vec3){0.0f, 0.0f, 1.0f});
            // glm_translate(pGraphic3DUbo->model, (vec3){1.0f, 0.0f, 0.0f});
            glm_lookat((vec3){-*pCamera_X, 4.0f + -*pCamera_Y, 4.0f}, (vec3){-*pCamera_X, -*pCamera_Y, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, pGraphic3DUbo->view);
            glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, 0.001f, 100.0f, pGraphic3DUbo->proj);
            // glm_perspective(glm_rad(45.0f), aspect, 0.1f, 100.0f, pGraphic3DUbo->proj);
            // pGraphic3DUbo->proj[1][1] *= -1;

            glm_mat4_identity(pUIUbo->model);
            glm_lookat((vec3){0.0f, 0.0f, 100.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pUIUbo->view);
            glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, 0.001f, 100.0f, pUIUbo->proj);
 
            allInOne.pComputeUbo->deltaTime = delta_time;

            vertexEnd = *allInOne.pVertices2DCount;

            glm_mat4_copy(allInOne.pGraphic3DUbo->proj, allInOne.pSSGIubo->projectionMatrix);
            glm_mat4_copy(allInOne.pGraphic3DUbo->proj, allInOne.pSSGIubo->inverseProjectionMatrix);
            glm_mat4_inv(allInOne.pSSGIubo->inverseProjectionMatrix, allInOne.pSSGIubo->inverseProjectionMatrix);

            float x, y, z;
            x = SDL_randf();
            y = SDL_randf();
            z = SDL_randf();
            // print("x: %f, y: %f, z: %f", x, y, z);

            mat4 lightProj;
            // glm_ortho_vulkan(-10.0f, 10.0f, 10.0f * aspect2, -10.0f * aspect2, 0.1f, 100.0f, lightProj);
            // glm_vec3_copy((vec3){x, y, z}, allInOne.pSunubo->lightDirection);
            // glm_lookat((vec3){x, y, z}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, allInOne.pSunubo->lightSpace);
            glm_lookat((vec3){5.0f, 9.0f, 5.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, allInOne.pSunubo->lightSpace);
            // glm_mat4_copy(allInOne.pGraphic3DUbo->view, allInOne.pSunubo->lightSpace);
            glm_mul(allInOne.pGraphic3DUbo->proj, allInOne.pSunubo->lightSpace, allInOne.pSunubo->lightSpace);
            glm_vec3_copy((vec3){-5.0f, -9.0f, -5.0f}, allInOne.pSunubo->lightDirection);
            glm_vec3_copy((vec3){1.0f, 0.95f, 0.8f}, allInOne.pSunubo->lightColor);
            allInOne.pSunubo->lightIntensity = 2.0f;

            glm_mat4_copy(allInOne.pSunubo->lightSpace, allInOne.pLightSpaceUbo->lightSpace);
            // glm_mul(lightProj, allInOne.pGraphic3DUbo->view, allInOne.pLightSpaceUbo->lightSpace);

            allInOne.pSSGIubo->cameraPosition[0] = -*pCamera_X;
            allInOne.pSSGIubo->cameraPosition[1] = 4.0f + -*pCamera_Y;
            allInOne.pSSGIubo->cameraPosition[2] = 4.0f;

            allInOne.pSSGIubo->rayStepSize = 0.05f;
            allInOne.pSSGIubo->maxRaySteps = 64;
            allInOne.pSSGIubo->ssgiStrength = 0.2f;

            SDL_LockMutex(allSync.updateMutex);
            // if (updateVertex)
            // {
            //     memcpy(*allInOne.ppVertexBufferMemMapped, *allInOne.ppVertices2D, 2100 * 4 * (sizeof(vec3) + sizeof(vec3) + sizeof(vec2)) * 2);// update vertex buffer
            //     updateVertex = false;
            // }
            // else
            // {
            //     memcpy((*allInOne.ppVertexBufferMemMapped)[*allInOne.pCurrentFrame], *allInOne.ppVertices_Pos, 2100 * 4 * sizeof(vec3));// update position
            // }

            // if (vertexEnd > vertexStart) memcpy((Vertex*)(*allInOne.ppVertexBufferMemMapped)[currentFrame] + vertexStart, *allInOne.ppVertices2D + vertexStart, (vertexEnd - vertexStart) * sizeof(Vertex));// update vertex buffer
            // else if (vertexEnd < vertexStart) 
            // {
            //     memcpy((Vertex*)(*allInOne.ppVertexBufferMemMapped)[currentFrame] + vertexStart, *allInOne.ppVertices2D + vertexStart, (allInOne.maxVerticesCount - vertexStart) * sizeof(Vertex));// update vertex buffer
            //     memcpy((*allInOne.ppVertexBufferMemMapped)[currentFrame], *allInOne.ppVertices2D, vertexEnd * sizeof(Vertex));
            // }

            memcpy((*allInOne.pppGraphicUniformBufferMapped)[currentFrame], pGraphicUbo, sizeof(UniformBufferObject));
            memcpy((*allInOne.pppGraphic3DUniformBufferMapped)[currentFrame], pGraphic3DUbo, sizeof(UniformBufferObject));
            memcpy((*allInOne.pppUIUniformBufferMapped)[currentFrame], pUIUbo, sizeof(UniformBufferObject));

            memcpy((*allInOne.pppComputeUniformBufferMapped)[currentFrame], allInOne.pComputeUbo, sizeof(ComputeUniformBufferObject));
            memcpy((*allInOne.pppSSGIUniformBufferMapped)[currentFrame], allInOne.pSSGIubo, sizeof(SSGIUniformBufferObject));
            memcpy((*allInOne.pppSunUniformBufferMapped)[currentFrame], allInOne.pSunubo, sizeof(DirectionLight));
            memcpy((*allInOne.pppLightSpaceUniformBufferMapped)[currentFrame], allInOne.pLightSpaceUbo , sizeof(LightSpace));

            memcpy((*allInOne.ppVertexBuffer2DMemMapped)[currentFrame], *allInOne.ppVertices2D, vertexEnd * sizeof(Vertex));// update vertex buffer
            memcpy((*allInOne.ppVertexBuffer3DMemMapped)[currentFrame], *allInOne.ppVertices3D, 30000 * sizeof(Vertex));
            memcpy((*allInOne.ppIndexBuffer3DMemMapped)[currentFrame], *allInOne.ppIndices3D, 45000 * sizeof(Uint32));
            SDL_SignalSemaphore(allSync.vertexSemaphore);

            allInOne.pPushConstants->rotation = totalTime * glm_rad(580.0f);
            
            SDL_UnlockMutex(allSync.updateMutex);

            //logMessage("test: %lf", testNum);
            //logMessage("delta time:%f", delta_time);
            update_frame++;
            
            update_done = true;
        }

        SDL_SignalSemaphore(allSync.signalSemaphore);
    }
    return 0;
}

// Render function to draw game objects in the SDL window_2D
int render(void * arg) 
{
    print("render init\n");
    Uint32 render_frame = 0;
    while (game_is_running)
    {
        SDL_WaitSemaphore(allSync.renderSemaphore);

        if (resolutionChanged)
        {
            recreateSwapchain();
            resolutionChanged = false;
        }

        drawFrame(scene);

        *allInOne.pCurrentFrame = (*allInOne.pCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        draw_done = true;

        //logMessage("render frames: %d ----%s", render_frame, timeNow);

        render_frame++;

        SDL_SignalSemaphore(allSync.signalSemaphore);
    }
    return 0;
}

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

static void destroyAllSync(void)
{
    SDL_DestroyMutex(allSync.updateMutex);
    SDL_DestroyMutex(allSync.renderMutex);
    SDL_DestroyMutex(allSync.logMutex);
    SDL_DestroyMutex(allSync.printMutex);
    SDL_DestroyMutex(allSync.popWindowMutex);
    SDL_DestroyMutex(allSync.textureMutex);
    SDL_DestroyMutex(allSync.timerMutex);
    SDL_DestroyMutex(allSync.descriptorUpdateMutex);
    SDL_DestroyMutex(allSync.vertexMutex);
    SDL_DestroyMutex(allSync.tileSetMutex);

    SDL_DestroySemaphore(allSync.updateSemaphore);
    SDL_DestroySemaphore(allSync.renderSemaphore);
    SDL_DestroySemaphore(allSync.vertexSemaphore);
    SDL_DestroySemaphore(allSync.signalSemaphore);
    SDL_DestroySemaphore(allSync.logSemaphore);
    SDL_DestroySemaphore(allSync.worldSemaphore);
}
// Function to destroy SDL window_2D and renderer
void destroy(void) 
{
    SDL_WaitThread(sdl_pid_signal, NULL);
    print("signal end\n");
    SDL_SignalSemaphore(allSync.updateSemaphore);
    SDL_WaitThread(sdl_pid_update, NULL);
    print("update end\n");
    SDL_SignalSemaphore(allSync.vertexSemaphore);
    SDL_WaitThread(sdl_pid_draw, NULL);
    print("draw end\n");
    
    cleanWorld();

    deInitTimerSystem();

    deInitTileMapSystem();

    deInitMusicManagement();

    cleanVulkan(FuncCodeMax);

    deInitPopWindow();
    destroyLog();

    SDL_Delay(1000);
    destroyAllSync();
    SDL_Quit();
    exit(0);
}
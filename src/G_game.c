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
#include "G_map.h"
#include "G_entity.h"
#include "G_test.h"

// Global variables
bool game_is_running = false;
int _Code = 0;

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

static bool initAllSync(void)
{
    allSync.inputMutex = SDL_CreateMutex();
    if (allSync.inputMutex == NULL) return false;
    allSync.updateMutex = SDL_CreateMutex();
    if (allSync.updateMutex == NULL) return false;
    allSync.renderMutex = SDL_CreateMutex();
    if (allSync.renderMutex == NULL) return false;
    allSync.logMutex = SDL_CreateMutex();
    if (allSync.logMutex == NULL) return false;
    allSync.printMutex = SDL_CreateMutex();
    if (allSync.printMutex == NULL) return false;
    allSync.popWindowMutex = SDL_CreateMutex();
    if (allSync.popWindowMutex == NULL) return false;
    allSync.textureMutex = SDL_CreateMutex();
    if (allSync.textureMutex == NULL) return false;
    allSync.timerMutex = SDL_CreateMutex();
    if (allSync.timerMutex == NULL) return false;
    allSync.descriptorUpdateMutex = SDL_CreateMutex();
    if (allSync.descriptorUpdateMutex == NULL) return false;
    allSync.vertexMutex = SDL_CreateMutex();
    if (allSync.vertexMutex == NULL) return false;
    allSync.tileSetMutex = SDL_CreateMutex();
    if (allSync.tileSetMutex == NULL) return false;

    allSync.updateSemaphore = SDL_CreateSemaphore(0);
    if (allSync.updateSemaphore == NULL) return false;
    allSync.renderSemaphore = SDL_CreateSemaphore(0);
    if (allSync.renderSemaphore == NULL) return false;
    allSync.vertexSemaphore = SDL_CreateSemaphore(0);
    if (allSync.vertexSemaphore == NULL) return false;
    allSync.signalSemaphore = SDL_CreateSemaphore(0);
    if (allSync.signalSemaphore == NULL) return false;
    allSync.logSemaphore = SDL_CreateSemaphore(0);
    if (allSync.logSemaphore == NULL) return false;
    allSync.worldSemaphore = SDL_CreateSemaphore(0);
    if (allSync.worldSemaphore == NULL) return false;
    allSync.bottomSemaphore = SDL_CreateSemaphore(0);
    if (allSync.bottomSemaphore == NULL) return false;

    return true;
}
// Setup function that runs once at the beginning of our program
void setup(int argc, char* argv[]) 
{
    int res = 0;
#if TEST
    res = TestAll();
    if (res != 0) 
    {
        exit(-1);
    }
#endif

    int arg = initFileSystem(argc, argv);
    res = arg;
    if (res < 0) goto clean;

    res = initAllSync();
    if (res == false) 
    {
        res = -5;
        goto clean;
    }

    res = initLog(arg);
    if (res == false)
    {
        res = -6;
        goto clean;
    }

    game_is_running = initWindow_3D();
    print("game_is_running: %d", game_is_running);
    if (game_is_running == false)
    {
        res = -7;
        goto clean;
    }

    res = SDL_StopTextInput(window_3D);
    if (res == false)
    {
        res = -8;
        goto clean;
    }

    static SDL_MessageBoxButtonData buttons[2] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "No"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Yes"},
    };

    static SDL_MessageBoxData messageBoxData = {0};
    messageBoxData.flags = SDL_MESSAGEBOX_WARNING;
    messageBoxData.window = window_3D;
    messageBoxData.title = "Do you want to quit?";
    messageBoxData.message = "Yes or No?";
    messageBoxData.numbuttons = 2;
    messageBoxData.buttons = buttons;
    messageBoxData.colorScheme = NULL;

    boxData = &messageBoxData;

    res = initPopWindow();
    if (res == false)
    {
        res = -9;
        goto clean;
    }

    initWorld();

    initTimerSystem();
    res = initTextSystem();
    if (res == false)
    {
        res = -10;
        goto clean;
    }
    res = initTileMapSystem();
    if (res == false)
    {
        res = -11;
        goto clean;
    }

    initVulkan();

    res = initMusicManagement();
    if (res == false)
    {
        res = -12;
        goto clean;
    }

    loadMusic((char*)getPath(MainBackgroundMusic1Wav), "test");

    sdl_pid_update = SDL_CreateThread(&update, "update", NULL);
    sdl_pid_draw = SDL_CreateThread(&render, "render", NULL);
    sdl_pid_signal = SDL_CreateThread(&signal_trans, "signal", NULL);

clean:
    if (res < 0)
    {
        _Code = res;
        game_is_running = false;
        return;
    }
}

static bool cameraMove[4];
//delete needed
static bool changeScene = false;

// static bool pause = false;
// static bool pause_signal_send = false;

//delete needed
static bool scale = false;

// static bool ballAdd = false;
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
static bool resolutionChanged2 = false; 

static float mouse_x, mouse_y;

static const Uint32 resolutions[][2] = {
    {800, 600},
    {1280, 720},
    {1600, 900},
    {1920, 1080},
    {2560, 1440},
    {3840, 2160},
};
static int resolutionIndex = 0;

G_Entity mPoint = {};
G_Entity camera = {};

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
        mPoint.direction[2] = true;
    }

    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        SDL_Keycode key = event.key.key;
        // print("preKeyState: %u, keyState: %u, key: %s(%u)", preKeyState, event.type, SDL_GetKeyName(key), key);
        // print("pressed Key:%u", pressedKey);
        
        if (event.type == SDL_EVENT_WINDOW_MINIMIZED)
        {
            preScene = scene;
            scene = Pause_Scene;
            SDL_Delay(50);
        }

        if (event.type == SDL_EVENT_WINDOW_RESTORED)
        {
            SDL_RaiseWindow(window_3D);

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
                SDL_Delay(250);

                resolutionIndex = (resolutionIndex + 1) % (sizeof(resolutions) / sizeof(resolutions[0]));

                allInOne.oldExtent2D.width = allInOne.extent2D.width;
                allInOne.oldExtent2D.height = allInOne.extent2D.height;
                allInOne.extent2D.width = resolutions[resolutionIndex][0];
                allInOne.extent2D.height = resolutions[resolutionIndex][1];
                
                SDL_SetWindowSize(window_3D, allInOne.extent2D.width, allInOne.extent2D.height);

                resolutionChanged = true;
                resolutionChanged2 = true;

                physicalCoffectX = (float)allInOne.extent2D.width / LOGICAL_WIDTH;
                physicalCoffectY = (float)allInOne.extent2D.height / LOGICAL_HEIGHT;
                
                print("sdl width: %u, height: %u", allInOne.extent2D.width, allInOne.extent2D.height);

                scene = preScene;
                preScene = Pause_Scene;
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
            }
            if (key == SDLK_F10)
            {
                print("F10");

                preScene = scene;
                scene = Pause_Scene;
                SDL_Delay(50);

                SDL_DisplayMode displayMode = {0};

                SDL_GetClosestFullscreenDisplayMode(displayId, allInOne.extent2D.width, allInOne.extent2D.height, 0, false, &displayMode);
                SDL_SetWindowFullscreen(window_3D, 1);
                SDL_SetWindowFullscreenMode(window_3D, &displayMode);
                SDL_RaiseWindow(window_3D);

                resolutionChanged = true;

                print("fullscreen");

                scene = preScene;
                preScene = Pause_Scene;
                SDL_SignalSemaphore(allSync.updateSemaphore);
                SDL_SignalSemaphore(allSync.renderSemaphore);
            }
            if (key == SDLK_F9)
            {
                print("F9");

                preScene = scene;
                scene = Pause_Scene;
                SDL_Delay(50);

                SDL_SetWindowFullscreen(window_3D, 0);
                allInOne.oldExtent2D.width = allInOne.extent2D.width;
                allInOne.oldExtent2D.height = allInOne.extent2D.height;

                SDL_SetWindowSize(window_3D, allInOne.extent2D.width, allInOne.extent2D.height);
                SDL_RaiseWindow(window_3D);

                resolutionChanged = true;

                print("windowed");

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
                //print("mouse moving: %d", event.type);
                // print("mouse: (%f, %f)", event.motion.x, event.motion.y);

                SDL_LockMutex(allSync.inputMutex);
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                SDL_UnlockMutex(allSync.inputMutex);
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
                        // ballAdd = true;
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
                    //     // print("scale: %d", scale);
                    // }
                }
                if (key == SDLK_T)
                {
                    textLine++;
                    textDisplay = true;
                    print("textline: %u", textLine);
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
                    mPoint.direction[2] = false;
                }
                if (key == SDLK_D)
                {
                    mPoint.direction[3] = false;
                }
                if (key == SDLK_W)
                {
                    mPoint.direction[0] = false;
                }
                if (key == SDLK_S)
                {
                    mPoint.direction[1] = false;
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
                // if ((key == SDLK_LEFT) && (event.key.repeat == false))
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
                if (key == SDLK_A)
                {
                    mPoint.direction[2] = true;
                }
                if (key == SDLK_D)
                {
                    mPoint.direction[3] = true;
                }
                if (key == SDLK_W)
                {
                    mPoint.direction[0] = true;
                }
                if (key == SDLK_S)
                {
                    mPoint.direction[1] = true;
                }
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

    initEntity(&mPoint, 0.0f, 0.0f, 87.0f);
    
    bool playedMusic = false;

    Uint64 frequency = SDL_GetPerformanceFrequency();;
    Uint64 delta_time_ns = 0;
    float delta_time = 0.0f;
    Uint64 totalTimeNs = 0;
    float totalTime = 0.0f;
    
    bool recovreyPause = false;
    bool sceneCleaned = false;

    G_Point_Int tileCenter = {0, 0};
    ShapeConstants shapePushConstants = {};
    shapePushConstants.pos[0] = 0.0f;
    shapePushConstants.pos[1] = 0.0f;
    shapePushConstants.scale[0] = 0.053333333f;
    shapePushConstants.scale[1] = 0.053333333f;

    Uint32 currentFrame;

    Uint32 vertexStart = 0;
    Uint32 vertexEnd = allInOne.vertices2DCount;

    Uint32 rowCount = 0;
    Uint32 colCount = 0;

    int firstBottom_X = 0;
    int firstBottom_Y = 0;

    int baseX = 0;
    int baseY = 0;

    int32_t groupID = -1;

    textureVertexInit(-32, -32, 64, 64, 0.2f, &allInOne.vertices2DCount, allInOne.pVertices2D, getTexture(TEXTURE_LOADING));
    
    // tileMapVertexInit(&allInOne.vertices2DCount, allInOne.pVertices2D);
    addModelMatrix(0, 0, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(100, 0, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(0, 100, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(100, 100, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);

    setMapBottom(allInOne.extent2D.width, allInOne.extent2D.height, 0, 0, &rowCount, &colCount, &firstBottom_X, &firstBottom_Y, &baseX, &baseY, &groupID);
    memcpy(allInOne.pTimeMapTexCoordBufferMapped[0], allInOne.pTileMapUVs, sizeof(vec2) * VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP * (allInOne.bottomImageDrawStack.top + 1));
    memcpy(allInOne.pTimeMapTexCoordBufferMapped[1], allInOne.pTileMapUVs, sizeof(vec2) * VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP * (allInOne.bottomImageDrawStack.top + 1));
    // addModelMatrix(0, 100 / HEIGHT_FACTOR, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM);
    // addModelMatrix(-800, 100 / HEIGHT_FACTOR, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM);
    // addModelMatrix(0, 900 / HEIGHT_FACTOR, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM);

    glm_mat4_identity(allInOne.pTilemapUbo->model);
    glm_lookat((vec3){0.0f, 0.0f, 100.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, allInOne.pTilemapUbo->view);
    glm_ortho_vulkan(-800.0f / 800.0f, 800.0f / 800.0f, -800.0f / 800.0f, 800.0f / 800.0f, -0.001f, -100.0f, allInOne.pTilemapUbo->proj);
    memcpy(allInOne.ppTilemapUniformBufferMapped[0], allInOne.pTilemapUbo, sizeof(UniformBufferObject));
    memcpy(allInOne.ppTilemapUniformBufferMapped[1], allInOne.pTilemapUbo, sizeof(UniformBufferObject));

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
            while(SDL_TryWaitSemaphore(allSync.vertexSemaphore));

            last_frame_time = SDL_GetPerformanceCounter();
            recovreyPause = false;
        }

        Uint64 tempTime = SDL_GetPerformanceCounter();
        delta_time_ns = ((tempTime - last_frame_time) * 1000000000ULL) / frequency;
        last_frame_time = tempTime;
        totalTimeNs += delta_time_ns;
        delta_time = delta_time_ns / ((float)S_TO_NS);
        totalTime = totalTimeNs / ((float)S_TO_NS);

        currentFrame = allInOne.currentFrame;
        
        // particle 
        allInOne.pComputeUbo->deltaTime = delta_time;

        SDL_LockMutex(allSync.vertexMutex);

        memcpy(allInOne.ppComputeUniformBufferMapped[currentFrame], allInOne.pComputeUbo, sizeof(ComputeUniformBufferObject));
        
        SDL_UnlockMutex(allSync.vertexMutex);
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        float aspect2 = 1.0f  * ((float)allInOne.extent2D.height / 600.0f);
        float aspect = ((float)allInOne.extent2D.width / allInOne.extent2D.height) * aspect2;

        float x, y, z;
        float factor_x = LIGHT_HEIGHT / (allInOne.extent2D.width / 2);
        float factor_y = LIGHT_HEIGHT / (allInOne.extent2D.height / 2);
        
        // shadow map
        SDL_LockMutex(allSync.inputMutex);
        x = mouse_x - (allInOne.extent2D.width / 2);
        y = -mouse_y + (allInOne.extent2D.height / 2);
        // y = 0;
        SDL_UnlockMutex(allSync.inputMutex);
        x *= factor_x;
        y *= factor_y;
        z = SDL_sqrtf(SDL_powf(LIGHT_HEIGHT, 2) - SDL_powf(x, 2) - SDL_powf(y, 2));

        // print("x: %f, y: %f, z: %f", x, y, z);

        mat4 lightProj;
        glm_ortho_vulkan(-(SHADOW_SIZE / 600.0f) / 2.0f, (SHADOW_SIZE / 600.0f) / 2.0f, -(SHADOW_SIZE / 800.0f) / 2.0f, (SHADOW_SIZE / 800.0f) / 2.0f, -0.001f, -100.0f, lightProj);

        glm_lookat((vec3){x, y, z}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, allInOne.pLightSpaceUbo->lightSpace);
        glm_mul(lightProj, allInOne.pLightSpaceUbo->lightSpace, allInOne.pLightSpaceUbo->lightSpace);


        SDL_LockMutex(allSync.vertexMutex);

        memcpy(allInOne.ppLightSpaceUniformBufferMapped[currentFrame], allInOne.pLightSpaceUbo , sizeof(LightSpace));
        
        SDL_UnlockMutex(allSync.vertexMutex);
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        // 3d object
        glm_mat4_identity(pGraphic3DUbo->model);
        glm_lookat((vec3){*pCamera_X * aspect, 0.0f + *pCamera_Y * aspect2, 10.0f}, (vec3){*pCamera_X * aspect, *pCamera_Y * aspect2, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pGraphic3DUbo->view);
        glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, -0.001f, -100.0f, pGraphic3DUbo->proj);
        // glm_perspective(glm_rad(45.0f), aspect, 0.1f, 100.0f, pGraphic3DUbo->proj);
        // pGraphic3DUbo->proj[1][1] *= -1;

        glm_vec3_copy((vec3){-x, -y, -z}, allInOne.pSunubo->lightDirection);
        glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, allInOne.pSunubo->lightColor);
        allInOne.pSunubo->lightIntensity = 1.1f;

        glm_mat4_copy(allInOne.pLightSpaceUbo->lightSpace, allInOne.pSunubo->lightSpace);

        SDL_LockMutex(allSync.vertexMutex);

        memcpy(allInOne.ppSunUniformBufferMapped[currentFrame], allInOne.pSunubo, sizeof(DirectionLight));
        memcpy(allInOne.ppGraphic3DUniformBufferMapped[currentFrame], pGraphic3DUbo, sizeof(UniformBufferObject));
        
        SDL_UnlockMutex(allSync.vertexMutex);
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        // SSGI
        glm_mat4_copy(allInOne.pGraphic3DUbo->proj, allInOne.pSSGIubo->projectionMatrix);
        glm_mat4_copy(allInOne.pGraphic3DUbo->proj, allInOne.pSSGIubo->inverseProjectionMatrix);
        glm_mat4_inv(allInOne.pSSGIubo->inverseProjectionMatrix, allInOne.pSSGIubo->inverseProjectionMatrix);

        allInOne.pSSGIubo->cameraPosition[0] = *pCamera_X;
        allInOne.pSSGIubo->cameraPosition[1] = 0.0f + *pCamera_Y;
        allInOne.pSSGIubo->cameraPosition[2] = 10.0f;

        allInOne.pSSGIubo->rayStepSize = 0.05f;
        allInOne.pSSGIubo->maxRaySteps = 64;
        allInOne.pSSGIubo->ssgiStrength = 0.1f;

        SDL_LockMutex(allSync.vertexMutex);

        memcpy(allInOne.ppSSGIUniformBufferMapped[currentFrame], allInOne.pSSGIubo, sizeof(SSGIUniformBufferObject));

        SDL_UnlockMutex(allSync.vertexMutex);
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        // UI object
        glm_mat4_identity(pGraphicUbo->model);
        glm_lookat((vec3){*pCamera_X, *pCamera_Y, 100.0f}, (vec3){*pCamera_X, *pCamera_Y, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pGraphicUbo->view);
        glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, -0.001f, -100.0f, pGraphicUbo->proj);
        // glm_ortho(-aspect, aspect, -1.0f, 1.0f, 0.001f, 100.0f, pGraphicUbo->proj);
        // pGraphicUbo->proj[1][1] *= -1;

        glm_mat4_identity(pUIUbo->model);
        glm_lookat((vec3){0.0f, 0.0f, 100.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pUIUbo->view);
        glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, -0.001f, -100.0f, pUIUbo->proj);

        SDL_LockMutex(allSync.vertexMutex);

        memcpy(allInOne.pShapeConstants, &shapePushConstants, sizeof(ShapeConstants));
        memcpy(allInOne.ppGraphicUniformBufferMapped[currentFrame], pGraphicUbo, sizeof(UniformBufferObject));
        memcpy(allInOne.ppUIUniformBufferMapped[currentFrame], pUIUbo, sizeof(UniformBufferObject));

        SDL_UnlockMutex(allSync.vertexMutex);
        SDL_SignalSemaphore(allSync.vertexSemaphore);

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
                print("clean scene");
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

            if (resolutionChanged2)
            {
                rowCount = colCount = 0;
                setMapBottom(allInOne.extent2D.width, allInOne.extent2D.height, *pCamera_X * (allInOne.extent2D.width / 2), *pCamera_Y * (allInOne.extent2D.height / 2), &rowCount, &colCount, &firstBottom_X, &firstBottom_Y, &baseX, &baseY, &groupID);
                resolutionChanged2 = false;
            }

            if (cameraMove[0])
            {
                *pCamera_Y += 1.6f * delta_time;
                //print("camera y: %f, enabled: %d, delta time: %lf, last_frame_time: %lu ----%s", *allInOne.pCamera_Y, cameraMove[0], delta_time, last_frame_time, timeNow);
            }
            if (cameraMove[1])
            {
                *pCamera_Y -= 1.6f * delta_time;
            }
            if (cameraMove[2])
            {
                *pCamera_X -= 1.6f * delta_time;
                // *pCamera_X += 50.0f / 800.0f;
                // print("camera x: %f", *pCamera_X);
            }
            if (cameraMove[3])
            {
                *pCamera_X += 1.6f * delta_time;
                // *pCamera_X -= 50.0f / 800.0f;
                // print("camera x: %f", *pCamera_X);
            }
            if (cameraMove[0] || cameraMove[1] || cameraMove[2] || cameraMove[3])
            {
                setMapBottom(allInOne.extent2D.width, allInOne.extent2D.height, *pCamera_X * (allInOne.extent2D.width / 2), *pCamera_Y * (allInOne.extent2D.height / 2), &rowCount, &colCount, &firstBottom_X, &firstBottom_Y, &baseX, &baseY, &groupID);
                SDL_LockMutex(allSync.updateMutex);
                memcpy(allInOne.pTimeMapTexCoordBufferMapped[currentFrame], allInOne.pTileMapUVs, sizeof(vec2) * VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP * (allInOne.bottomImageDrawStack.top + 1));
                SDL_UnlockMutex(allSync.updateMutex);
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
                // print("TotalTime: %fms", totalTime);

                if (textDisplay)
                {
                    Uint32 textLen = 0;
                    if (textLine == 2) getTextUV("一二三", &textLen);
                    else if (textLine == 1) getTextUV("哈哈哈哈哈哈哈哈哈", &textLen);

                    if (textLine < 3)
                    {
                        getTexture(TEXTURE_FONT)->refCount = 0;
                        for (Uint32 i = 0;i < textLen;i++)
                        {
                            textureVertexInit_SetUV(-300.0 + (float)i * FONT_SIZE, -100.0, FONT_SIZE, FONT_SIZE, 0.1f, &allInOne.vertices2DCount, allInOne.pVertices2D, UVs[i], getTexture(TEXTURE_FONT));
                        }
                    }
                    textDisplay = false;
                }

                testNum += 2 * delta_time;

                static int id_test = 0;
                int test_a = -1;
                addTimerFunc(u32_s_to_ns(1), &id_test, 10, test, &test_a);

                EntityMove(&mPoint, delta_time);
                tileCenter = locatePoint(&mPoint, rowCount, colCount, firstBottom_X, firstBottom_Y, groupID);
                shapePushConstants.pos[0] = (float)tileCenter.x / (allInOne.extent2D.width / 2);
                shapePushConstants.pos[1] = (float)tileCenter.y / (allInOne.extent2D.height / 2);
                shapePushConstants.scale[0] = (float)18 / (allInOne.extent2D.width / 2);
                shapePushConstants.scale[1] = (float)18 / (allInOne.extent2D.height / 2);

                // print("mPoint: (%d, %d)", (int32_t)mPoint.position.x, (int32_t)mPoint.position.y);
                // SDL_LockMutex(sdl_mutex_2);
                // if (pictureMove[0])
                // {
                //     *allInOne.pPictureY += 200 * delta_time;
                //     //print("y: %f, enabled: %d, delta time: %lf, last_frame_time: %lu ----%s", *allInOne.pPictureY, pictureMove[0], delta_time, last_frame_time, timeNow);
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

                // size_t bufferSize = sizeof(Vertex332) * count;

                // Uint32 indiceCount = *allInOne.pIndicesCount;
                // size_t bufferSize2 = sizeof(uint16_t) * indiceCount;

                // if (ballAdd)
                // {
                //     //allInOne.pVertices2D = (Vertex332 *)realloc(allInOne.pVertices2D, count * sizeof(Vertex332));
                //     int x = SDL_rand(250);
                //     if (SDL_rand(2))
                //     {
                //         x *= -1;
                //     }
                //     // float averagePhysicalCoffect = (physicalCoffectX + physicalCoffectY) / 2.0f;
                //     textureVertexInit(x * physicalCoffectX, 280 * physicalCoffectY, 16 * physicalCoffectY, 16 * physicalCoffectY, 0.9, allInOne.vertices2DCount, allInOne.pVertices2D, getTexture(TEXTURE_CIRCLE));

                //     ballStack.pushFn(&ballStack, &x);
                //     //print("indices count: %u\n", indiceCount);
                //     //*allInOne.ppIndices = (uint16_t *)realloc(*allInOne.ppIndices, indiceCount * sizeof(uint16_t));

                //     ballAdd = false;
                // }
            
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

            vertexEnd = allInOne.vertices2DCount;
            //print("time: %.2f\n", time);
            SDL_LockMutex(allSync.updateMutex);

            memcpy(allInOne.pVertexBuffer2DMemMapped[currentFrame], allInOne.pVertices2D, vertexEnd * sizeof(Vertex332));// update vertex buffer
            memcpy(allInOne.pVertexBuffer3DMemMapped[currentFrame], allInOne.pVertices3D, 30000 * sizeof(Vertex332));
            memcpy(allInOne.pIndexBuffer3DMemMapped[currentFrame], allInOne.pIndices3D, 45000 * sizeof(Uint32));
            // SDL_SignalSemaphore(allSync.vertexSemaphore);

            allInOne.pPushConstants->rotation = totalTime * glm_rad(580.0f);
            
            SDL_UnlockMutex(allSync.updateMutex);

            //print("test: %lf", testNum);
            //print("delta time:%f", delta_time);
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
    Uint32 currentFrame = allInOne.currentFrame;
    bool bottomMoved = false;
    while (game_is_running)
    {
        SDL_WaitSemaphore(allSync.renderSemaphore);

        if (resolutionChanged)
        {
            recreateSwapchain(currentFrame);
            resolutionChanged = false;
        }

        bottomMoved = moveBottomImage(currentFrame);

        drawFrame(scene, currentFrame, allInOne.extent2D.width, allInOne.extent2D.height, bottomMoved);

        allInOne.currentFrame = (allInOne.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        currentFrame = allInOne.currentFrame;

        draw_done = true;

        //print("render frames: %d ----%s", render_frame, timeNow);

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
    if (_Code < 0)
    {
        if (window_3D != NULL)
        {
            char buffer[256];
            SDL_snprintf(buffer, 255, "Error %8x: %s", _Code, SDL_GetError());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", buffer, window_3D);
        }
        else
        print("Error %x: %s", _Code, SDL_GetError());
    }

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

    cleanVulkan();

    deInitPopWindow();
    destroyLog();

    SDL_Delay(1000);
    destroyAllSync();
    SDL_Quit();
    exit(_Code);
}
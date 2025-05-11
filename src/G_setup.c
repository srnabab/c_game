#include "G_game.h"
#include "G_graphic.h"
#include "G_file/G_file.h"
#include "G_log.h"
#include "G_pop_window.h"
#include "G_world.h"
#include "G_timer.h"
#include "G_text.h"
#include "G_music.h"
#include "G_struct.h"
#include "G_test.h"
#include "G_allocator.h"
#include "G_TileMap/G_TileSet.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_messagebox.h"

extern G_SYNC allSync;

// static bool windowBackgroud(void * userdata, SDL_Event * event)
// {
//     switch (event->type)
//     {
//         case SDL_EVENT_WILL_ENTER_BACKGROUND:
//             pauseCode();
//             SDL_Delay(20);
//             break;
    
//         default: return true;
//     }

// }
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

extern bool game_is_running;
extern int _Code;
extern SDL_MessageBoxData * boxData;
extern SDL_Window * window_3D;

// file variables
static SDL_Thread * sdl_pid_update = NULL;
static SDL_Thread * sdl_pid_draw = NULL;
static SDL_Thread * sdl_pid_signal = NULL;

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

#ifdef TRACE_PTR
    initMemoryRecord();
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

    // SDL_AddEventWatch(windowBackgroud, NULL);

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

    res = SDL_HasGamepad();
    if (res)
    {
        int32_t gamepadCount = 0;
        SDL_JoystickID * gamepadIDs = SDL_GetGamepads(&gamepadCount);
        SDL_OpenGamepad(gamepadIDs[0]);
    }

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

    SDL_Delay(1000);
    SDL_Log("residue memory:%llu, allocation: %u", G_totalAllocSize(), G_allocations());
    printResidueMemory();
    deleteRecord();
    exit(_Code);
}
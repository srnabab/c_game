#include "G_game.h"

#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_music.h"
#include "G_scene.h"
#include "G_pop_window.h"
#include "G_constants.h"
#include "G_log.h"
#include "G_file/G_file.h"
#include "G_struct.h"
#include "G_entity.h"

// Global variables
bool game_is_running = false;
int _Code = 0;

extern SDL_Window * window_3D;
extern SDL_DisplayID displayId;
extern VK_ALL allInOne;

G_SYNC allSync = {0};

SDL_MessageBoxData * boxData = NULL;

// static bool pause = false;
// static bool pause_signal_send = false;

bool ballAdd = false;
uint8_t leftButtonClickedTimes = 0;
bool leftButtonEnabled = true;
Uint32 ballCount = 2;

extern float physicalCoffectX;
extern float physicalCoffectY;

bool resolutionChanged = false; 
bool resolutionChanged2 = false; 

float mouse_x, mouse_y;

static const Uint32 resolutions[][2] = {
    {800, 600},
    {1280, 720},
    {1600, 900},
    {1920, 1080},
    {2560, 1440},
    {3840, 2160},
};
static int resolutionIndex = 0;

volatile bool keys[SDL_SCANCODE_COUNT + UINT8_MAX];
volatile bool preKeys[SDL_SCANCODE_COUNT + UINT8_MAX];
volatile bool repeatKeys[SDL_SCANCODE_COUNT + UINT8_MAX];

static Uint32 keyToScancode(Uint32 key)
{
    switch (key)
    {
        // Direct Character Mappings (usually ASCII based)
        case SDLK_A:            return SDL_SCANCODE_A;
        case SDLK_B:            return SDL_SCANCODE_B;
        case SDLK_C:            return SDL_SCANCODE_C;
        case SDLK_D:            return SDL_SCANCODE_D;
        case SDLK_E:            return SDL_SCANCODE_E;
        case SDLK_F:            return SDL_SCANCODE_F;
        case SDLK_G:            return SDL_SCANCODE_G;
        case SDLK_H:            return SDL_SCANCODE_H;
        case SDLK_I:            return SDL_SCANCODE_I;
        case SDLK_J:            return SDL_SCANCODE_J;
        case SDLK_K:            return SDL_SCANCODE_K;
        case SDLK_L:            return SDL_SCANCODE_L;
        case SDLK_M:            return SDL_SCANCODE_M;
        case SDLK_N:            return SDL_SCANCODE_N;
        case SDLK_O:            return SDL_SCANCODE_O;
        case SDLK_P:            return SDL_SCANCODE_P;
        case SDLK_Q:            return SDL_SCANCODE_Q;
        case SDLK_R:            return SDL_SCANCODE_R;
        case SDLK_S:            return SDL_SCANCODE_S;
        case SDLK_T:            return SDL_SCANCODE_T;
        case SDLK_U:            return SDL_SCANCODE_U;
        case SDLK_V:            return SDL_SCANCODE_V;
        case SDLK_W:            return SDL_SCANCODE_W;
        case SDLK_X:            return SDL_SCANCODE_X;
        case SDLK_Y:            return SDL_SCANCODE_Y;
        case SDLK_Z:            return SDL_SCANCODE_Z;

        case SDLK_1:            return SDL_SCANCODE_1;
        case SDLK_2:            return SDL_SCANCODE_2;
        case SDLK_3:            return SDL_SCANCODE_3;
        case SDLK_4:            return SDL_SCANCODE_4;
        case SDLK_5:            return SDL_SCANCODE_5;
        case SDLK_6:            return SDL_SCANCODE_6;
        case SDLK_7:            return SDL_SCANCODE_7;
        case SDLK_8:            return SDL_SCANCODE_8;
        case SDLK_9:            return SDL_SCANCODE_9;
        case SDLK_0:            return SDL_SCANCODE_0;

        case SDLK_RETURN:       return SDL_SCANCODE_RETURN;
        case SDLK_ESCAPE:       return SDL_SCANCODE_ESCAPE;
        case SDLK_BACKSPACE:    return SDL_SCANCODE_BACKSPACE;
        case SDLK_TAB:          return SDL_SCANCODE_TAB;
        case SDLK_SPACE:        return SDL_SCANCODE_SPACE;

        case SDLK_MINUS:        return SDL_SCANCODE_MINUS;
        case SDLK_EQUALS:       return SDL_SCANCODE_EQUALS;
        case SDLK_LEFTBRACKET:  return SDL_SCANCODE_LEFTBRACKET;
        case SDLK_RIGHTBRACKET: return SDL_SCANCODE_RIGHTBRACKET;
        case SDLK_BACKSLASH:    return SDL_SCANCODE_BACKSLASH;
        // Note: SDL_SCANCODE_NONUSHASH (50) often maps to the same key as BACKSLASH (49)
        // Note: SDL_SCANCODE_NONUSBACKSLASH (100) is a separate key on ISO layouts

        case SDLK_SEMICOLON:    return SDL_SCANCODE_SEMICOLON;
        case SDLK_APOSTROPHE:   return SDL_SCANCODE_APOSTROPHE;
        case SDLK_GRAVE:        return SDL_SCANCODE_GRAVE; // ` key
        case SDLK_COMMA:        return SDL_SCANCODE_COMMA;
        case SDLK_PERIOD:       return SDL_SCANCODE_PERIOD;
        case SDLK_SLASH:        return SDL_SCANCODE_SLASH;

        // Shifted Character Mappings (Mapped to the base scancode)
        case SDLK_EXCLAIM:      return SDL_SCANCODE_1; // !
        case SDLK_DBLAPOSTROPHE: return SDL_SCANCODE_APOSTROPHE; // "
        case SDLK_HASH:         return SDL_SCANCODE_3; // # (Might be NONUSHASH on UK)
        case SDLK_DOLLAR:       return SDL_SCANCODE_4; // $
        case SDLK_PERCENT:      return SDL_SCANCODE_5; // %
        case SDLK_AMPERSAND:    return SDL_SCANCODE_7; // &
        case SDLK_LEFTPAREN:    return SDL_SCANCODE_9; // (
        case SDLK_RIGHTPAREN:   return SDL_SCANCODE_0; // )
        case SDLK_ASTERISK:     return SDL_SCANCODE_8; // *
        case SDLK_PLUS:         return SDL_SCANCODE_EQUALS; // +
        case SDLK_COLON:        return SDL_SCANCODE_SEMICOLON; // :
        case SDLK_LESS:         return SDL_SCANCODE_COMMA; // <
        case SDLK_GREATER:      return SDL_SCANCODE_PERIOD; // >
        case SDLK_QUESTION:     return SDL_SCANCODE_SLASH; // ?
        case SDLK_AT:           return SDL_SCANCODE_2; // @
        case SDLK_CARET:        return SDL_SCANCODE_6; // ^
        case SDLK_UNDERSCORE:   return SDL_SCANCODE_MINUS; // _
        case SDLK_LEFTBRACE:    return SDL_SCANCODE_LEFTBRACKET; // {
        case SDLK_PIPE:         return SDL_SCANCODE_BACKSLASH; // | (Might be NONUSHASH or NONUSBACKSLASH)
        case SDLK_RIGHTBRACE:   return SDL_SCANCODE_RIGHTBRACKET; // }
        case SDLK_TILDE:        return SDL_SCANCODE_GRAVE; // ~ (Might be NONUSHASH on UK)

        case SDLK_DELETE:       return SDL_SCANCODE_DELETE; // ASCII DEL maps to Delete key

        // Scancode-based Keycodes (0x40000000 | scancode)
        case SDLK_CAPSLOCK:     return SDL_SCANCODE_CAPSLOCK;
        case SDLK_F1:           return SDL_SCANCODE_F1;
        case SDLK_F2:           return SDL_SCANCODE_F2;
        case SDLK_F3:           return SDL_SCANCODE_F3;
        case SDLK_F4:           return SDL_SCANCODE_F4;
        case SDLK_F5:           return SDL_SCANCODE_F5;
        case SDLK_F6:           return SDL_SCANCODE_F6;
        case SDLK_F7:           return SDL_SCANCODE_F7;
        case SDLK_F8:           return SDL_SCANCODE_F8;
        case SDLK_F9:           return SDL_SCANCODE_F9;
        case SDLK_F10:          return SDL_SCANCODE_F10;
        case SDLK_F11:          return SDL_SCANCODE_F11;
        case SDLK_F12:          return SDL_SCANCODE_F12;
        case SDLK_PRINTSCREEN:  return SDL_SCANCODE_PRINTSCREEN;
        case SDLK_SCROLLLOCK:   return SDL_SCANCODE_SCROLLLOCK;
        case SDLK_PAUSE:        return SDL_SCANCODE_PAUSE;
        case SDLK_INSERT:       return SDL_SCANCODE_INSERT;
        case SDLK_HOME:         return SDL_SCANCODE_HOME;
        case SDLK_PAGEUP:       return SDL_SCANCODE_PAGEUP;
        case SDLK_END:          return SDL_SCANCODE_END;
        case SDLK_PAGEDOWN:     return SDL_SCANCODE_PAGEDOWN;
        case SDLK_RIGHT:        return SDL_SCANCODE_RIGHT;
        case SDLK_LEFT:         return SDL_SCANCODE_LEFT;
        case SDLK_DOWN:         return SDL_SCANCODE_DOWN;
        case SDLK_UP:           return SDL_SCANCODE_UP;
        case SDLK_NUMLOCKCLEAR: return SDL_SCANCODE_NUMLOCKCLEAR;
        case SDLK_KP_DIVIDE:    return SDL_SCANCODE_KP_DIVIDE;
        case SDLK_KP_MULTIPLY:  return SDL_SCANCODE_KP_MULTIPLY;
        case SDLK_KP_MINUS:     return SDL_SCANCODE_KP_MINUS;
        case SDLK_KP_PLUS:      return SDL_SCANCODE_KP_PLUS;
        case SDLK_KP_ENTER:     return SDL_SCANCODE_KP_ENTER;
        case SDLK_KP_1:         return SDL_SCANCODE_KP_1;
        case SDLK_KP_2:         return SDL_SCANCODE_KP_2;
        case SDLK_KP_3:         return SDL_SCANCODE_KP_3;
        case SDLK_KP_4:         return SDL_SCANCODE_KP_4;
        case SDLK_KP_5:         return SDL_SCANCODE_KP_5;
        case SDLK_KP_6:         return SDL_SCANCODE_KP_6;
        case SDLK_KP_7:         return SDL_SCANCODE_KP_7;
        case SDLK_KP_8:         return SDL_SCANCODE_KP_8;
        case SDLK_KP_9:         return SDL_SCANCODE_KP_9;
        case SDLK_KP_0:         return SDL_SCANCODE_KP_0;
        case SDLK_KP_PERIOD:    return SDL_SCANCODE_KP_PERIOD;
        case SDLK_APPLICATION:  return SDL_SCANCODE_APPLICATION;
        case SDLK_POWER:        return SDL_SCANCODE_POWER;
        case SDLK_KP_EQUALS:    return SDL_SCANCODE_KP_EQUALS;
        case SDLK_F13:          return SDL_SCANCODE_F13;
        case SDLK_F14:          return SDL_SCANCODE_F14;
        case SDLK_F15:          return SDL_SCANCODE_F15;
        case SDLK_F16:          return SDL_SCANCODE_F16;
        case SDLK_F17:          return SDL_SCANCODE_F17;
        case SDLK_F18:          return SDL_SCANCODE_F18;
        case SDLK_F19:          return SDL_SCANCODE_F19;
        case SDLK_F20:          return SDL_SCANCODE_F20;
        case SDLK_F21:          return SDL_SCANCODE_F21;
        case SDLK_F22:          return SDL_SCANCODE_F22;
        case SDLK_F23:          return SDL_SCANCODE_F23;
        case SDLK_F24:          return SDL_SCANCODE_F24;
        case SDLK_EXECUTE:      return SDL_SCANCODE_EXECUTE;
        case SDLK_HELP:         return SDL_SCANCODE_HELP;
        case SDLK_MENU:         return SDL_SCANCODE_MENU;
        case SDLK_SELECT:       return SDL_SCANCODE_SELECT;
        case SDLK_STOP:         return SDL_SCANCODE_STOP; // Also see SDLK_AC_STOP
        case SDLK_AGAIN:        return SDL_SCANCODE_AGAIN;
        case SDLK_UNDO:         return SDL_SCANCODE_UNDO; // Also see SDLK_AC_UNDO
        case SDLK_CUT:          return SDL_SCANCODE_CUT;
        case SDLK_COPY:         return SDL_SCANCODE_COPY;
        case SDLK_PASTE:        return SDL_SCANCODE_PASTE;
        case SDLK_FIND:         return SDL_SCANCODE_FIND; // Also see SDLK_AC_SEARCH
        case SDLK_MUTE:         return SDL_SCANCODE_MUTE;
        case SDLK_VOLUMEUP:     return SDL_SCANCODE_VOLUMEUP;
        case SDLK_VOLUMEDOWN:   return SDL_SCANCODE_VOLUMEDOWN;
        case SDLK_KP_COMMA:     return SDL_SCANCODE_KP_COMMA;
        case SDLK_KP_EQUALSAS400: return SDL_SCANCODE_KP_EQUALSAS400;
        case SDLK_ALTERASE:     return SDL_SCANCODE_ALTERASE;
        case SDLK_SYSREQ:       return SDL_SCANCODE_SYSREQ;
        case SDLK_CANCEL:       return SDL_SCANCODE_CANCEL; // Also see SDLK_AC_CANCEL
        case SDLK_CLEAR:        return SDL_SCANCODE_CLEAR;
        case SDLK_PRIOR:        return SDL_SCANCODE_PRIOR;
        case SDLK_RETURN2:      return SDL_SCANCODE_RETURN2;
        case SDLK_SEPARATOR:    return SDL_SCANCODE_SEPARATOR;
        case SDLK_OUT:          return SDL_SCANCODE_OUT;
        case SDLK_OPER:         return SDL_SCANCODE_OPER;
        case SDLK_CLEARAGAIN:   return SDL_SCANCODE_CLEARAGAIN;
        case SDLK_CRSEL:        return SDL_SCANCODE_CRSEL;
        case SDLK_EXSEL:        return SDL_SCANCODE_EXSEL;
        case SDLK_KP_00:        return SDL_SCANCODE_KP_00;
        case SDLK_KP_000:       return SDL_SCANCODE_KP_000;
        case SDLK_THOUSANDSSEPARATOR: return SDL_SCANCODE_THOUSANDSSEPARATOR;
        case SDLK_DECIMALSEPARATOR: return SDL_SCANCODE_DECIMALSEPARATOR;
        case SDLK_CURRENCYUNIT: return SDL_SCANCODE_CURRENCYUNIT;
        case SDLK_CURRENCYSUBUNIT: return SDL_SCANCODE_CURRENCYSUBUNIT;
        case SDLK_KP_LEFTPAREN: return SDL_SCANCODE_KP_LEFTPAREN;
        case SDLK_KP_RIGHTPAREN: return SDL_SCANCODE_KP_RIGHTPAREN;
        case SDLK_KP_LEFTBRACE: return SDL_SCANCODE_KP_LEFTBRACE;
        case SDLK_KP_RIGHTBRACE: return SDL_SCANCODE_KP_RIGHTBRACE;
        case SDLK_KP_TAB:       return SDL_SCANCODE_KP_TAB;
        case SDLK_KP_BACKSPACE: return SDL_SCANCODE_KP_BACKSPACE;
        case SDLK_KP_A:         return SDL_SCANCODE_KP_A;
        case SDLK_KP_B:         return SDL_SCANCODE_KP_B;
        case SDLK_KP_C:         return SDL_SCANCODE_KP_C;
        case SDLK_KP_D:         return SDL_SCANCODE_KP_D;
        case SDLK_KP_E:         return SDL_SCANCODE_KP_E;
        case SDLK_KP_F:         return SDL_SCANCODE_KP_F;
        case SDLK_KP_XOR:       return SDL_SCANCODE_KP_XOR;
        case SDLK_KP_POWER:     return SDL_SCANCODE_KP_POWER;
        case SDLK_KP_PERCENT:   return SDL_SCANCODE_KP_PERCENT;
        case SDLK_KP_LESS:      return SDL_SCANCODE_KP_LESS;
        case SDLK_KP_GREATER:   return SDL_SCANCODE_KP_GREATER;
        case SDLK_KP_AMPERSAND: return SDL_SCANCODE_KP_AMPERSAND;
        case SDLK_KP_DBLAMPERSAND: return SDL_SCANCODE_KP_DBLAMPERSAND;
        case SDLK_KP_VERTICALBAR: return SDL_SCANCODE_KP_VERTICALBAR;
        case SDLK_KP_DBLVERTICALBAR: return SDL_SCANCODE_KP_DBLVERTICALBAR;
        case SDLK_KP_COLON:     return SDL_SCANCODE_KP_COLON;
        case SDLK_KP_HASH:      return SDL_SCANCODE_KP_HASH;
        case SDLK_KP_SPACE:     return SDL_SCANCODE_KP_SPACE;
        case SDLK_KP_AT:        return SDL_SCANCODE_KP_AT;
        case SDLK_KP_EXCLAM:    return SDL_SCANCODE_KP_EXCLAM;
        case SDLK_KP_MEMSTORE:  return SDL_SCANCODE_KP_MEMSTORE;
        case SDLK_KP_MEMRECALL: return SDL_SCANCODE_KP_MEMRECALL;
        case SDLK_KP_MEMCLEAR:  return SDL_SCANCODE_KP_MEMCLEAR;
        case SDLK_KP_MEMADD:    return SDL_SCANCODE_KP_MEMADD;
        case SDLK_KP_MEMSUBTRACT: return SDL_SCANCODE_KP_MEMSUBTRACT;
        case SDLK_KP_MEMMULTIPLY: return SDL_SCANCODE_KP_MEMMULTIPLY;
        case SDLK_KP_MEMDIVIDE: return SDL_SCANCODE_KP_MEMDIVIDE;
        case SDLK_KP_PLUSMINUS: return SDL_SCANCODE_KP_PLUSMINUS;
        case SDLK_KP_CLEAR:     return SDL_SCANCODE_KP_CLEAR;
        case SDLK_KP_CLEARENTRY: return SDL_SCANCODE_KP_CLEARENTRY;
        case SDLK_KP_BINARY:    return SDL_SCANCODE_KP_BINARY;
        case SDLK_KP_OCTAL:     return SDL_SCANCODE_KP_OCTAL;
        case SDLK_KP_DECIMAL:   return SDL_SCANCODE_KP_DECIMAL;
        case SDLK_KP_HEXADECIMAL: return SDL_SCANCODE_KP_HEXADECIMAL;
        case SDLK_LCTRL:        return SDL_SCANCODE_LCTRL;
        case SDLK_LSHIFT:       return SDL_SCANCODE_LSHIFT;
        case SDLK_LALT:         return SDL_SCANCODE_LALT;
        case SDLK_LGUI:         return SDL_SCANCODE_LGUI;
        case SDLK_RCTRL:        return SDL_SCANCODE_RCTRL;
        case SDLK_RSHIFT:       return SDL_SCANCODE_RSHIFT;
        case SDLK_RALT:         return SDL_SCANCODE_RALT;
        case SDLK_RGUI:         return SDL_SCANCODE_RGUI;
        case SDLK_MODE:         return SDL_SCANCODE_MODE;

        // Consumer Page / Mobile Keycodes (0x40000000 | scancode)
        case SDLK_SLEEP:        return SDL_SCANCODE_SLEEP;
        case SDLK_WAKE:         return SDL_SCANCODE_WAKE;
        case SDLK_CHANNEL_INCREMENT: return SDL_SCANCODE_CHANNEL_INCREMENT;
        case SDLK_CHANNEL_DECREMENT: return SDL_SCANCODE_CHANNEL_DECREMENT;
        case SDLK_MEDIA_PLAY:   return SDL_SCANCODE_MEDIA_PLAY;
        case SDLK_MEDIA_PAUSE:  return SDL_SCANCODE_MEDIA_PAUSE;
        case SDLK_MEDIA_RECORD: return SDL_SCANCODE_MEDIA_RECORD;
        case SDLK_MEDIA_FAST_FORWARD: return SDL_SCANCODE_MEDIA_FAST_FORWARD;
        case SDLK_MEDIA_REWIND: return SDL_SCANCODE_MEDIA_REWIND;
        case SDLK_MEDIA_NEXT_TRACK: return SDL_SCANCODE_MEDIA_NEXT_TRACK;
        case SDLK_MEDIA_PREVIOUS_TRACK: return SDL_SCANCODE_MEDIA_PREVIOUS_TRACK;
        case SDLK_MEDIA_STOP:   return SDL_SCANCODE_MEDIA_STOP;
        case SDLK_MEDIA_EJECT:  return SDL_SCANCODE_MEDIA_EJECT;
        case SDLK_MEDIA_PLAY_PAUSE: return SDL_SCANCODE_MEDIA_PLAY_PAUSE;
        case SDLK_MEDIA_SELECT: return SDL_SCANCODE_MEDIA_SELECT;
        case SDLK_AC_NEW:       return SDL_SCANCODE_AC_NEW;
        case SDLK_AC_OPEN:      return SDL_SCANCODE_AC_OPEN;
        case SDLK_AC_CLOSE:     return SDL_SCANCODE_AC_CLOSE;
        case SDLK_AC_EXIT:      return SDL_SCANCODE_AC_EXIT;
        case SDLK_AC_SAVE:      return SDL_SCANCODE_AC_SAVE;
        case SDLK_AC_PRINT:     return SDL_SCANCODE_AC_PRINT;
        case SDLK_AC_PROPERTIES: return SDL_SCANCODE_AC_PROPERTIES;
        case SDLK_AC_SEARCH:    return SDL_SCANCODE_AC_SEARCH;
        case SDLK_AC_HOME:      return SDL_SCANCODE_AC_HOME;
        case SDLK_AC_BACK:      return SDL_SCANCODE_AC_BACK;
        case SDLK_AC_FORWARD:   return SDL_SCANCODE_AC_FORWARD;
        case SDLK_AC_STOP:      return SDL_SCANCODE_AC_STOP;
        case SDLK_AC_REFRESH:   return SDL_SCANCODE_AC_REFRESH;
        case SDLK_AC_BOOKMARKS: return SDL_SCANCODE_AC_BOOKMARKS;
        case SDLK_SOFTLEFT:     return SDL_SCANCODE_SOFTLEFT;
        case SDLK_SOFTRIGHT:    return SDL_SCANCODE_SOFTRIGHT;
        case SDLK_CALL:         return SDL_SCANCODE_CALL;
        case SDLK_ENDCALL:      return SDL_SCANCODE_ENDCALL;

        // Less common/Ambiguous mappings
        case SDLK_PLUSMINUS:    return SDL_SCANCODE_KP_PLUSMINUS; // Map to keypad version?
        // International keys have SDLK values but often depend heavily on layout
        // SDL_SCANCODE_INTERNATIONAL1 through 9, SDL_SCANCODE_LANG1 through 9 don't have direct SDLK_ equivalents listed

        // Extended Keys (No direct Scancode equivalent provided)
        case SDLK_LEFT_TAB:
        case SDLK_LEVEL5_SHIFT:
        case SDLK_MULTI_KEY_COMPOSE:
        case SDLK_LMETA:
        case SDLK_RMETA:
        case SDLK_LHYPER:
        case SDLK_RHYPER:       // Fall through intentionally

        // Unknown / Default
        case SDLK_UNKNOWN:      // Fall through intentionally
        default:
            return SDL_SCANCODE_UNKNOWN; // Return 0 or SDL_SCANCODE_UNKNOWN
    }
}
static void setKeysByScancode(void)
{
    Uint32 i = 0;
    const bool * keyState = SDL_GetKeyboardState(NULL);
    for (i = 0;i < SDL_SCANCODE_COUNT;i++)
    {
        // preKeys[i] = keys[i];
        if (keyState[i]) keys[i] = true;
    }
}
void pauseCode(void)
{
    keys[SDL_SCANCODE_PAUSE] = false;
    preKeys[SDL_SCANCODE_PAUSE] = true;
}
// Function to poll SDL events and process keyboard 
bool minimize = false;
bool process_input(void)
{
    static Uint32 preKeyState = 0;
    static uint8_t pressedKey = 0;
    static int buttonId = 0;
    static int pause = 0;

    if (game_is_running == false)
    {
        return true;
    }

    if (willPopWindow())
    {
        pauseCode();
        pause = (pause + 1) % 2;
        
        popWindow();

        pauseCode();
        pause = (pause + 1) % 2;
    }


    SDL_Event event;

    if (minimize)
    {
        SDL_WaitEvent(&event);
        SDL_SignalSemaphore(allSync.updateSemaphore);
        print("pause: %d", minimize);
        pauseCode();
        minimize = (minimize + 1) % 2;
    }

    while(SDL_PollEvent(&event))
    {
        SDL_Keycode key = event.key.key;
        // print("preKeyState: %u, keyState: %u, key: %s(%u)", preKeyState, event.type, SDL_GetKeyName(key), key);
        // print("pressed Key:%u", pressedKey);
        // print("event type: %u", event.type);

        Uint32 scancode = 0;

        switch (event.type)
        {
            case SDL_EVENT_QUIT:

            pauseCode();
            pause = (pause + 1) % 2;

            SDL_ShowMessageBox(boxData, &buttonId);

            if (buttonId == 2)
            {
                pauseCode();
                pause = (pause + 1) % 2;

                game_is_running = false;

                return true;
            }
            else if (buttonId == 1)
            {
                pauseCode();
                pause = (pause + 1) % 2;
            }
            break;

            case SDL_EVENT_WINDOW_MINIMIZED:
            pauseCode();
            minimize = (minimize + 1) % 2;
            break;

            case SDL_EVENT_WINDOW_RESTORED:
            SDL_RaiseWindow(window_3D);
            resolutionChanged = true;
            pauseCode();
            minimize = (minimize + 1) % 2;
            break;

            case SDL_EVENT_MOUSE_MOTION:
            SDL_LockMutex(allSync.inputMutex);
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            SDL_UnlockMutex(allSync.inputMutex);
            break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
            break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (leftButtonEnabled && (ballCount < 2000/*ball count*/))
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    leftButtonClickedTimes++;
                    ballCount++;
                    ballAdd = true;
                }
            }
            break;

            case SDL_EVENT_KEY_DOWN:
            if (event.key.down && !event.key.repeat)
            {
                pressedKey++;
            }
            scancode = keyToScancode(key);

            preKeys[scancode] = keys[scancode];
            keys[scancode] = true;
            repeatKeys[scancode] = event.key.repeat;

            switch (key)
            {
                case SDLK_ESCAPE:
                    return true;

                case SDLK_F6:
                    pauseCode();
                    pause = (pause + 1) % 2;

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
                    pauseCode();
                    pause = (pause + 1) % 2;
                    break;

                case SDLK_F10:
                    print("F10");

                    pauseCode();
                    pause = (pause + 1) % 2;

                    SDL_DisplayMode displayMode = {0};

                    SDL_GetClosestFullscreenDisplayMode(displayId, allInOne.extent2D.width, allInOne.extent2D.height, 0, false, &displayMode);
                    SDL_SetWindowFullscreen(window_3D, 1);
                    SDL_SetWindowFullscreenMode(window_3D, &displayMode);
                    SDL_RaiseWindow(window_3D);

                    resolutionChanged = true;

                    print("fullscreen");

                    pauseCode();
                    pause = (pause + 1) % 2;
                    break;

                case SDLK_F9:
                    print("F9");

                    pauseCode();
                    pause = (pause + 1) % 2;

                    SDL_SetWindowFullscreen(window_3D, 0);
                    allInOne.oldExtent2D.width = allInOne.extent2D.width;
                    allInOne.oldExtent2D.height = allInOne.extent2D.height;

                    SDL_SetWindowSize(window_3D, allInOne.extent2D.width, allInOne.extent2D.height);
                    SDL_RaiseWindow(window_3D);

                    resolutionChanged = true;

                    print("windowed");
                    pauseCode();
                    pause = (pause + 1) % 2;
                    break;

                default:
                    break;
            }
            break;

            case SDL_EVENT_KEY_UP:            
            scancode = keyToScancode(key);
            preKeys[scancode] = keys[scancode];
            keys[scancode] = false;
            repeatKeys[scancode] = event.key.repeat;
            pressedKey--;
            break;
            
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            
            switch (event.gaxis.axis)
            {
                case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                print("left trigger: %d", event.gaxis.value);
                break;

                case SDL_GAMEPAD_AXIS_LEFTX:
                print("left axis x: %d", event.gaxis.value);
                break;

                case SDL_GAMEPAD_AXIS_LEFTY:
                print("left axis y: %d", event.gaxis.value);
                break;

                case SDL_GAMEPAD_AXIS_RIGHTX:
                print("right axis x: %d", event.gaxis.value);
                break;

                case SDL_GAMEPAD_AXIS_RIGHTY:
                print("right axis y: %d", event.gaxis.value);
                break;

                case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                print("right trigger: %d", event.gaxis.value);
                break;
            }
            break;

            case SDL_EVENT_GAMEPAD_BUTTON_UP:
            scancode = event.gbutton.button + SDL_SCANCODE_COUNT;
            preKeys[scancode] = keys[scancode];
            keys[scancode] = false;
            repeatKeys[scancode] =  event.gbutton.down;
            break;

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            scancode = event.gbutton.button + SDL_SCANCODE_COUNT;
            preKeys[scancode] = keys[scancode];
            keys[scancode] = true;
            repeatKeys[scancode] =  event.gbutton.down;

            switch (event.gbutton.button)
            {
                case SDL_GAMEPAD_BUTTON_WEST:
                print("X");
                break;

                case SDL_GAMEPAD_BUTTON_SOUTH:
                print("A");
                break;

                case SDL_GAMEPAD_BUTTON_NORTH:
                print("Y");
                break;

                case SDL_GAMEPAD_BUTTON_EAST:
                print("B");
                break;

                case SDL_GAMEPAD_BUTTON_DPAD_UP:
                print("up");
                break;

                case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                print("down");
                break;

                case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                print("left");
                break;

                case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                print("right");
                break;

                case SDL_GAMEPAD_BUTTON_LEFT_STICK:
                print("LS");
                break;

                case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
                print("RS");
                break;

                case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                print("LB");
                break;

                case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                print("RB");
                break;

                case SDL_GAMEPAD_BUTTON_MISC1:
                print("screenshoot");
                break;

                case SDL_GAMEPAD_BUTTON_GUIDE:
                print("big xbox");
                break;

                case SDL_GAMEPAD_BUTTON_START:
                print("right small");
                break;

                case SDL_GAMEPAD_BUTTON_BACK:
                print("left small");
                break;
            }

            default:
            break;
        }
        preKeyState = event.type;
    }

    setKeysByScancode();

    // SDL_PumpEvents();
    // if (preKeys[SDL_SCANCODE_T])
    // print("key: %d, preState: %d, repeat: %d (T)", keys[SDL_SCANCODE_T], preKeys[SDL_SCANCODE_T], repeatKeys[SDL_SCANCODE_T]);

    if (game_is_running) return false;
    else return true;
}

#ifndef SCENE_H
#define SCENE_H 1

#include "SDL3/SDL_begin_code.h"

enum _Scene
{
    First_Scene,
    Pause_Scene,
    Menu_Scene,
};
typedef enum _Scene Scene;

struct _Scene_Pack
{
    Scene scene;
    const char* backgroundMusic;
};
typedef struct _Scene_Pack Scene_Pack;

#include "SDL3/SDL_close_code.h"

#endif
#include "SDL3/SDL_stdinc.h"

#ifndef SCENE_H
#define SCENE_H 1

#include "SDL3/SDL_begin_code.h"

enum _Scene
{
    First_Scene = 1,
    Pause_Scene,
    Menu_Scene,
    MAX_Scene = 0x7FFFFFFF
};
typedef enum _Scene Scene;

struct _SceneParameter
{
    Scene scene;
    Uint32 draw3d;
    Uint32 draw2d;
    Uint32 SSGI;
    struct _SceneParameter * next;
};
typedef struct _SceneParameter SceneParameter;

extern bool SDLCALL registerScene(Scene scene, bool draw3d, bool draw2d, bool SSGI);
extern SceneParameter * SDLCALL getSceneParameter(Scene scene);
extern bool SDLCALL deleteScene(Scene scene);
extern void SDLCALL deleteAllScene(void);

#include "SDL3/SDL_close_code.h"

#endif

#include "G_scene.h"

#include "vk_code_h/vk_struct.h"
#include "vk_code_h/vk_descriptorPool.h"

extern VK_ALL allInOne;
static Scene_Pack pack[3];

bool cleanScene(Scene preScene)
{
    return true;
}
bool loadScene(Scene scene)
{
    switch (scene)
    {
        case First_Scene:
        break;
        
        case Menu_Scene:
        break;
        
        case Pause_Scene:
        break;
    }

    return true;
}
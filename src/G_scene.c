#include "G_scene.h"
#include "G_allocator.h"

#include "vk_code_h/vk_descriptorPool.h"
#include "vk_code_h/vk_all_struct.h"
#include <string.h>

extern VK_ALL allInOne;
static SceneParameter * head;

static SceneParameter * findScene(Scene scene)
{
    SceneParameter * node = head;
    while (node != NULL) 
    {
        if (node->next == NULL) return node;
        if (node->next->scene == scene) return node;
        node = node->next;
    }

    return head;
}
bool registerScene(Scene scene, bool draw3d, bool draw2d, bool SSGI)
{
    if (head == NULL)
    {
        head = G_malloc(sizeof(SceneParameter));
        if (head == NULL) return false;

        head->scene = MAX_Scene;
        head->next = NULL;
    }

    SceneParameter * node = findScene(scene);
    if (node->next)
    if (node->next->scene == scene)
    {
        node = node->next;
        node->draw2d = draw2d;
        node->draw3d = draw3d;
        node->SSGI = SSGI;

        return true;
    }

    node->next = G_malloc(sizeof(SceneParameter));
    if (node->next == NULL) 
    {
        return false;
    }
    node = node->next;
    node->next = NULL;
    node->draw2d = draw2d;
    node->draw3d = draw3d;
    node->SSGI = SSGI;
    node->scene = scene;

    return true;
}
SceneParameter * getSceneParameter(Scene scene)
{
    return findScene(scene)->next;
}
bool deleteScene(Scene scene)
{
    SceneParameter * node = findScene(scene);
    SceneParameter * temp = node;
    if (node->next->scene == scene)
    {
        node = node->next;
        temp->next = node->next;
        G_free(node);
        return true;
    }

    return false;
}
void deleteAllScene(void)
{
    SceneParameter * node = head;
    SceneParameter * temp = head;
    while (node != NULL) 
    {
        temp = node->next;
        G_free(node);
        node = temp;
    }
}

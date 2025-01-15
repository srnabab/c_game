#include "G_pop_window.h"
#include "SDL3/SDL_mutex.h"

extern SDL_Window * window;

typedef struct _PopWindow
{
    SDL_MessageBoxFlags flags;
    char title[255];
    char message[255];
} PopWindow;

typedef struct _PopList
{
    PopWindow * node;
    struct _PopList * next;
} PopList;

static PopList * root = NULL;
static SDL_Mutex * mutex = NULL;

bool initPopWindow(void)
{
    root = (PopList*)SDL_malloc(sizeof(PopList));
    root->next = NULL;
    root->node = NULL;
    mutex = SDL_CreateMutex();
    return true;
}
static PopList * toNull(PopList * next)
{
    if (next->node == NULL)
    {
        return next;
    }
    else
    {
        return toNull(next->next);
    }
}
static bool insertMessage(SDL_MessageBoxFlags flags, const char * title, const char * message)
{
    SDL_LockMutex(mutex);
    PopWindow * node = (PopWindow*)SDL_malloc(sizeof(PopWindow));
    if (node == NULL)
        return false;
        
    node->flags = flags;
    SDL_strlcpy(node->title, title, 255);
    SDL_strlcpy(node->message, message, 255);

    PopList * listNode = toNull(root);
    listNode->node = node;
    listNode->next = (PopList*)SDL_malloc(sizeof(PopList));
    if (listNode->next == NULL)
    {
        SDL_free(node);
        return false;
    }

    listNode->next->next = NULL;
    listNode->next->node = NULL;
    SDL_UnlockMutex(mutex);

    return true;
}

void pushMessage(SDL_MessageBoxFlags flags, const char * title, char *fmt, ...)
{
    char message[255];
    va_list arg;
    va_start(arg, fmt);
    SDL_vsnprintf(message, 255, fmt, arg);
    va_end(arg);

    insertMessage(flags, title, message);
}

static void cleanList(void)
{
    PopList * next = root;
    while (next->next != NULL)
    {
        SDL_free(next->node);
        PopList * temp = next;
        next = next->next;
        SDL_free(temp);
    }
    root = (PopList*)SDL_malloc(sizeof(PopList));
    root->next = NULL;
    root->node = NULL;
}
bool willPopWindow(void)
{
    if (root->node != NULL)
        return true;
    
    return false;
}
void popWindow(void)
{
    SDL_LockMutex(mutex);
    PopList * next = root;
    while (next->node != NULL)
    {
        SDL_ShowSimpleMessageBox(next->node->flags, next->node->title, next->node->message, window);
        next = next->next;
    }
    cleanList();
    
    SDL_UnlockMutex(mutex);
}
bool deInitPopWindow(void)
{
    cleanList();
    SDL_free(root);
    SDL_DestroyMutex(mutex);
    return true;
}
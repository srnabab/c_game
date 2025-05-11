#include "G_pop_window.h"
#include "G_struct.h"
#include "G_allocator.h"

extern SDL_Window * window_3D;
extern G_SYNC allSync;

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

bool initPopWindow(void)
{
    root = (PopList*)G_malloc(sizeof(PopList));
    if (root == NULL) return false;

    root->next = NULL;
    root->node = NULL;
    
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
    SDL_LockMutex(allSync.popWindowMutex);
    PopWindow * node = (PopWindow*)G_malloc(sizeof(PopWindow));
    if (node == NULL)
        return false;
        
    node->flags = flags;
    SDL_strlcpy(node->title, title, 255);
    SDL_strlcpy(node->message, message, 255);

    PopList * listNode = toNull(root);
    listNode->node = node;
    listNode->next = (PopList*)G_malloc(sizeof(PopList));
    if (listNode->next == NULL)
    {
        G_free(node);
        return false;
    }

    listNode->next->next = NULL;
    listNode->next->node = NULL;
    SDL_UnlockMutex(allSync.popWindowMutex);

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
        G_free(next->node);
        PopList * temp = next;
        next = next->next;
        G_free(temp);
    }
    root = (PopList*)G_malloc(sizeof(PopList));
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
    SDL_LockMutex(allSync.popWindowMutex);
    PopList * next = root;
    while (next->node != NULL)
    {
        SDL_ShowSimpleMessageBox(next->node->flags, next->node->title, next->node->message, window_3D);
        next = next->next;
    }
    cleanList();
    
    SDL_UnlockMutex(allSync.popWindowMutex);
}
bool deInitPopWindow(void)
{
    cleanList();
    G_free(root);
    return true;
}
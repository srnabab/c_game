#include "content_manager/content_manager.h"

#include "G_constants.h"

#include "G_file/G_file.h"
#include "G_log.h"

#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_thread.h"
#include "SDL_complement.h"

static Uint8 setArgments(int argc, char * argv[])
{
    Uint8 log = 0x0;
    for (int i = 1;i < argc;i++)
    {
        if (SDL_strcmp(argv[i], "-Log-print") == 0)
        {
            log |= LOG_ENABLED;
        }
        else if (SDL_strcmp(argv[i], "-Log-txt") == 0)
        {
            log |= (LOG_ENABLED | LOG_TXT);
        }
    }
    return log;
}
static char rootPath[255];

static bool setRootPath(char * programPath)
{
    SDL_strlcpy(rootPath, programPath, 255);

    char * slash = SDL_strrchr(rootPath, SEPRATOR_C);
    if (slash == NULL) return false;
    *(slash + 1) = '\0';

    return true;
}
const char * getRootPath(void)
{
    return rootPath;
}

static char PathTemp[MAX_FILES][MAX_PATHLEN];
static const char (*Path)[MAX_PATHLEN] = NULL;
static bool initPath(void)
{
    char pathFilePath[MAX_PATHLEN];

    memset(PathTemp, 0, sizeof(PathTemp));
    for (int i = 0;i < MAX_FILES;i++)
    {
        memcpy(PathTemp[i], rootPath, MAX_PATHLEN);
    }
    memcpy(pathFilePath, rootPath, MAX_PATHLEN);

    SDL_strlcat(pathFilePath, SEPRATOR"Path", 255);

    SDL_IOStream * io  = NULL;
    if ((io = SDL_IOFromFile(pathFilePath, "r")) == NULL)
    {
        return false;
    }
    char buffer[255];
    int fileCount = 0;
    while (SDL_IOgets(buffer, MAX_PATHLEN, io) != NULL)
    {
        buffer[SDL_strcspn(buffer, ":")] = '\0';
        PathType type = None;
        type = pathCompare(buffer);

        if (type)
        {
            SDL_IOgets(buffer, MAX_PATHLEN, io);

            buffer[SDL_strcspn(buffer, "\n")] = '\0';
            SDL_strlcat(PathTemp[type], buffer, 255);
            SDL_Log(PathTemp[type]);
            fileCount++;
        }
        memset(buffer, 0, MAX_PATHLEN);
    }
    SDL_CloseIO(io);
    Path = (const char (*)[MAX_PATHLEN])PathTemp;
    return true;
}
static bool setPath(void)
{
    char * path = (char*)rootPath;
    generatePath(0, &path);

    return true;
}

int initFileSystem(int argc, char * argv[])
{
    if (!setRootPath(argv[0])) return -1;
    if (!setPath()) return -2;
    Uint8 place1 = 0x0; 
    Uint8 place2 = 0x0;
    Uint8 place3 = 0x0;
    Uint8 log = setArgments(argc, argv);
    if (!initPath()) return -4;

    return (place1 << 24) | (place2 << 16) | (place3 << 8) | log;
}
const char * getPath(PathType type)
{
    return Path[type];
}
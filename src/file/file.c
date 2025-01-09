#include "file/file.h"
#include "log.h"
#include "std_c.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_thread.h"
#include "SDL_complement.h"

uint8_t log_print = 0;
uint8_t log_txt = 0;

extern bool logOutEnabled;
extern bool logEnabled;

static bool setArgments(int argc, char * argv[])
{
    for (int i = 1;i < argc;i++)
    {
        if (SDL_strcmp(argv[i], "-Log-print") == 0)
        {
            logEnabled = true;
            logOutEnabled = false;
            log_print = i;
        }
        else if (SDL_strcmp(argv[i], "-Log-txt") == 0)
        {
            logEnabled = true;
            logOutEnabled = true;
            log_txt = i;
        }
        else
        {
            logEnabled = false;
        }
    }
    return true;
}
static char rootPath[255];

static bool setRootPath(char * programPath)
{
    SDL_strlcpy(rootPath, programPath, 255);

#if defined(_WIN32)
    char * slash = SDL_strrchr(rootPath, '\\');
#elif defined(__linux__)
    char * slash = SDL_strrchr(rootPath, '/');
#endif
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

#if defined(_WIN32)
    SDL_strlcat(pathFilePath, "\\Path", 255);
#elif defined(__linux__)
    SDL_strlcat(pathFilePath, "/Path", 255);
#endif

    SDL_IOStream * io  = NULL;
    if ((io = SDL_IOFromFile(pathFilePath, "r")) == NULL)
    {
        return false;
    }
    char buffer[255];
    int fileCount = 0;
    while (SDL_IOgets(buffer, MAX_PATHLEN, io) != NULL)
    {
        if (buffer[0] == '\r' || buffer[0] == '\n') continue;
        buffer[SDL_strcspn(buffer, ":")] = '\0';
        PathType type = None;
        type = pathCompare(buffer);

        if (type)
        {
            SDL_IOgets(buffer, MAX_PATHLEN, io);
#if defined(_WIN32)
            buffer[SDL_strcspn(buffer, "\r")] = '\0';
#elif defined(__linux__)
            buffer[SDL_strcspn(buffer, "\n")] = '\0';
#endif
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
static int systemRun(void * arg)
{
    return system((char*)arg);
}
static bool setPath(void)
{
    char path[512] = {0};
    
#if defined(_WIN32)
    SDL_snprintf(path, 512, "%s%s %s %s%s", rootPath, "ContentManager.exe", rootPath, rootPath, "Content.db");
#elif defined(__linux__)
    SDL_snprintf(path, 512, "%s%s %s %s%s", rootPath, "ContentManager", rootPath, rootPath, "Content.db");
#endif


    SDL_Thread * thread = SDL_CreateThread(systemRun, "contentManager", path);
    int status;
    SDL_WaitThread(thread, &status);
    // printf("%d\n", status);
    // system(path);

    return true;
}

int initFileSystem(int argc, char * argv[])
{
    if (!setRootPath(argv[0])) return -1;
    if (!setPath()) return -2;
    if (!setArgments(argc, argv)) return -3;
    if (!initPath()) return -4;

    return 0;
}
const char * getPath(PathType type)
{
    return Path[type];
}
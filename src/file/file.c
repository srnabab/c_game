#include "file/file.h"
#include "log.h"
#include "std_c.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
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

static char PathTemp[MAX_FILES][MAX_PATHLEN];
static const char (*Path)[MAX_PATHLEN] = NULL;
static bool initPath(char * argv)
{
    char pathFilePath[MAX_PATHLEN];
    SDL_strlcpy(pathFilePath, argv, 255);

    char * slash = SDL_strrchr(pathFilePath, '\\');
    if (slash == NULL) return false;
    *(slash + 1) = '\0';

    memset(PathTemp, 0, sizeof(PathTemp));
    for (int i = 0;i < MAX_FILES;i++)
    {
        memcpy(PathTemp[i], pathFilePath, MAX_PATHLEN);
    }

    SDL_strlcat(pathFilePath, "\\Path", 255);
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
        buffer[SDL_strcspn(buffer, "\r")] = '\0';
        PathType type = None;
        type = pathCompare(buffer);

        if (type)
        {
            SDL_IOgets(buffer, MAX_PATHLEN, io);
            buffer[SDL_strcspn(buffer, "\r")] = '\0';
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
int initFileSystem(int argc, char * argv[])
{
    if (!setArgments(argc, argv)) return -2;
    if (!initPath(argv[0])) return -3;

    return 0;
}
const char * getPath(PathType type)
{
    return Path[type];
}
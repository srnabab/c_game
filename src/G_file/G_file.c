#include "SDL3/SDL_stdinc.h"
#include "SDL_error.h"
#include "content_manager/content_manager.h"

#include "textureG/textureG.h"

#include "G_constants.h"

#include "G_file/G_file.h"
#include "G_log.h"

#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_thread.h"
#include "SDL_complement.h"

static Uint8 setArgments(int argc, char * argv[])
{
    if (argc == 1) 
    {
        SDL_Log(".exe\n[-Log-print](to console)\n[-Log-txt](to log file)\n"
        "[-FontImage] [-F] [FontPath] [-C] [Channels] [-S] [fontSize] [-H] [HashTablePath] [-P] [PngPath]");
    }
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
        else if (SDL_strcmp(argv[i], "-FontImage") == 0)
        {
            char fontPath[255] = {0};
            char pngPath[255] = {0};
            char hashPath[255] = {0};
            int fontSize = 60;
            int channel = 4;
            i++;
            for (;i < argc;i++)
            {
                if (SDL_strcmp(argv[i], "-F") == 0)
                {
                    strcpy(fontPath, argv[++i]);
                }
                else if (SDL_strcmp(argv[i], "-P") == 0)
                {
                    strcpy(pngPath, argv[++i]);
                }
                else if (SDL_strcmp(argv[i], "-H") == 0)
                {
                    strcpy(hashPath, argv[++i]);
                }
                else if (SDL_strcmp(argv[i], "-S") == 0)
                {
                    fontSize = atoi(argv[++i]);
                }
                else if (SDL_strcmp(argv[i], "-C") == 0)
                {
                    channel = SDL_atoi(argv[++i]);
                }
            }

            int failed = 0;
            exit(textureGenerate(fontPath, hashPath, pngPath, channel, fontSize, &failed));
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
            // SDL_Log(PathTemp[type]);
            fileCount++;
        }
        memset(buffer, 0, MAX_PATHLEN);
    }
    SDL_CloseIO(io);
    Path = (const char (*)[MAX_PATHLEN])PathTemp;
    freePathHashTable();
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
    Uint8 place1 = 0x0; 
    Uint8 place2 = 0x0;
    Uint8 place3 = 0x0;
    Uint8 log = setArgments(argc, argv);
    if (!setRootPath(argv[0])) return -1;
    if (!setPath()) return -2;
    if (!initPath()) return -4;

    return (place1 << 24) | (place2 << 16) | (place3 << 8) | log;
}
const char * getPath(PathType type)
{
    return Path[type];
}

bool readFile(const char * file, char * data, Sint64 * fileSize)
{
    Sint64 result = 0;
    SDL_IOStream * io = SDL_IOFromFile(file, "rb");
    bool success = false;

    if (io == NULL) 
    {
        SDL_Log("Error: Failed to open file %s: %s", file, SDL_GetError());
        goto cleanup;
    }

    if (data == NULL)
    {
        result = SDL_SeekIO(io, 0, SDL_IO_SEEK_END);
        if (result == -1)
        {
            SDL_Log("Error: Failed to seek to end of file %s: %s", file, SDL_GetError());
            goto cleanup;
        }

        result = SDL_TellIO(io);
        if (result == -1)
        {
            SDL_Log("Error: Failed to get file size for %s: %s", file, SDL_GetError());
            goto cleanup;
        }

        *fileSize = result;
        if (result == 0)
        {
            SDL_Log("Warning: File %s is empty.", file);
            success = true; // 视为空文件为成功读取
            goto cleanup;
        }
    }
    else
    {
        result = SDL_SeekIO(io, 0, SDL_IO_SEEK_SET);
        if (result == -1)
        {
            SDL_Log("Error: Failed to seek to beginning of file %s: %s", file, SDL_GetError());
            goto cleanup;
        }

        result = SDL_ReadIO(io, data, *fileSize);
        if (SDL_GetIOStatus(io) == SDL_IO_STATUS_ERROR)
        {
            SDL_Log("Error: Failed to read from file %s: %s", file, SDL_GetError());
            goto cleanup;
        }
    }

    success = true;

    cleanup:
    if (io) if (SDL_CloseIO(io) == false) SDL_Log("Warning: Failed to close file %s: %s", file, SDL_GetError());
    
    return success;
}

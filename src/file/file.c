#include "file/file.h"
#include "log.h"
#include "std_c.h"

uint8_t log_print = 0;
uint8_t log_txt = 0;

extern bool logOutEnabled;
extern bool logEnabled;

static bool setArgments(void)
{
    for (int i = 1;i < __argc;i++)
    {
        if (strcmp(__argv[i], "-Log-print") == 0)
        {
            logEnabled = true;
            logOutEnabled = false;
            log_print = i;
        }
        else if (strcmp(__argv[i], "-Log-txt") == 0)
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
static bool changeArgv_0(void)
{
    char * slash = strrchr(__argv[0], '\\');

    if (slash == NULL) return false;

    *(slash + 1) = '\0';

    //printf("root directory: %s\n", __argv[0]);
    return true;
}

static char PathTemp[MAX_FILES][MAX_PATHLEN];
static const char (*Path)[MAX_PATHLEN] = NULL;
static bool initPath(void)
{
    char pathFilePath[MAX_PATHLEN];
    strcpy(pathFilePath, __argv[0]);

    memset(PathTemp, 0, sizeof(PathTemp));
    for (int i = 0;i < MAX_FILES;i++)
    {
        memcpy(PathTemp[i], pathFilePath, MAX_PATHLEN);
    }

    strcat(pathFilePath, "\\Path");
    FILE * fp = NULL;
    if ((fp = fopen(pathFilePath, "r")) == NULL)
    {
        return false;
    }
    char buffer[256];
    int fileCount = 0;
    while (fgets(buffer, MAX_PATHLEN, fp) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        PathType type = None;
        type = pathCompare(buffer);

        if (type)
        {
            fgets(buffer, MAX_PATHLEN, fp);
            buffer[strcspn(buffer, "\n")] = '\0';
            strcat(PathTemp[type], buffer);
            puts(PathTemp[type]);
            fgets(buffer, MAX_PATHLEN, fp);
            fileCount++;
        }
    }
    fclose(fp);
    Path = (const char (*)[MAX_PATHLEN])PathTemp;
    return true;
}
int initFileSystem(void)
{
    if (!changeArgv_0()) return -1;
    if (!setArgments()) return -2;
    if (!initPath()) return -3;

    return 0;
}
const char * getPath(PathType type)
{
    return Path[type];
}
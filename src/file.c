#include "file.h"
#include "debug.h"

void changeArgv_0(void)
{
    char * slash = strrchr(__argv[0], '\\');

    *(slash + 1) = '\0';
    if (DEBUG)
        printf("root directory: %s \n", __argv[0]);
}
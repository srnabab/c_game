#include "file/path_compare.h"

#ifndef FILE_H
#define FILE_H

#define MAX_PATHLEN 255
#define MAX_FILES 65535

int initFileSystem(void);
const char * getPath(PathType type);

#endif
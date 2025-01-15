#include "G_begin_code.h"
#include "G_file/path_compare.h"

#ifndef G_FILE_H
#define G_FILE_H 1

#define MAX_PATHLEN 255
#define MAX_FILES 65535

extern int initFileSystem(int argc, char * argv[]);
extern const char * getRootPath(void);
extern const char * getPath(PathType type);

#endif
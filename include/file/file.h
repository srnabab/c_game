#include "file/path_compare.h"

#ifndef FILE_H
#define FILE_H

#define MAX_PATHLEN 255
#define MAX_FILES 65535

#if defined(SEPRATOR)
#undef SEPRATOR
#endif

#if defined(SEPRATOR_C)
#undef SEPRATOR_C
#endif

#if defined(END_SIGN)
#undef END_SIGN
#endif

#if defined(_WIN32)
#define SEPRATOR "\\"
#define SEPRATOR_C '\\'
#define END_SIGN "\r"
#define END_SIGN_C '\r'
#elif defined(__linux__)
#define SEPRATOR "/"
#define SEPRATOR_C '/'
#define END_SIGN "\n"
#define END_SIGN_C '\n'
#endif

int initFileSystem(int argc, char * argv[]);
const char * getRootPath(void);
const char * getPath(PathType type);

#endif
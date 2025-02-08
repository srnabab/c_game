#include "SDL3/SDL_stdinc.h"
#include "sqlite3/sqlite3.h"

#ifndef CONTENT_MANAGER_H
#define CONTENT_MANAGER_H

#include "SDL3/SDL_begin_code.h"

struct _DB_Path
{
    sqlite3 * db;
    char * A_path;// absolutely path
    Uint16 R_Begin;// relative path
    Uint16 LenGetId;// where to begin get short file path
};
typedef struct _DB_Path DB_Path;

// for fixed file: font, log...
struct _Fixed_File
{
    char * alias;
    char * name;
};
typedef struct _Fixed_File Fixed_File;

// max row count in database
#define MAX_ROW 1024

extern int SDLCALL generatePath(int argc, char * argv[]);

#include "SDL3/SDL_close_code.h"

#endif
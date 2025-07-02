#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_filesystem.h"
#include "sqlite3/sqlite3.h"
#include "G_file/G_file_type.h"
#include "uthash/uthash.h"

#ifndef CONTENT_MANAGER_H
#define CONTENT_MANAGER_H

struct FileTypeHashTable
{
    FileType fileType;
    char suffix[64];
    UT_hash_handle hh;
};
typedef struct FileTypeHashTable FileTypeHashTable;

#include "SDL3/SDL_begin_code.h"

struct _DB_Path
{
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

extern int SDLCALL createNewUUID(unsigned char * uuidStr);
extern void SDLCALL blake3HashContent(const unsigned char * content, const size_t len, Uint8 * output);
extern void SDLCALL initFileTypeHashTable(void);
extern bool SDLCALL insertIntoImageLoadParameter(const Uint8 * hash, const unsigned char * uuid);
extern bool SDLCALL insertDeletedRowIntoDeletedRow(void);
extern bool SDLCALL tableExistJudge(void);
extern int SDLCALL generatePath(int argc, char * argv[]);
extern bool SDLCALL insertNode_2(const char * alias, const char * name);
extern bool SDLCALL updateDatabase(DB_Path * pPack);
extern int SDLCALL getRowsCount(void);
extern bool SDLCALL insertNode3(int mainTableRowID);
extern SDL_PathType SDLCALL getPathType(const int id);
extern char* SDLCALL getPathByID(const int ID);
extern char* SDLCALL getName(const int ID);
extern int SDLCALL findSameDeletedRow(const char * fileName, int type, Uint32 fileType, char * innerName);
extern bool SDLCALL deleteRow3(int ID);

#include "SDL3/SDL_close_code.h"

#endif

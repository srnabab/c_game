#include "content_manager/content_manager.h"
#include "G_constants.h"

#include "G_file_type.h"

// #undef TRACE_PTR 
#include "G_allocator.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_iostream.h"

#include "SDL_stdinc.h"
#include "SDL_timer.h"
#include "sqlite3/sqlite3.h"
#include "sqlite3/sqlite3_alloc_func.h"
#include "uthash/uthash.h"

sqlite3 * db = NULL;
static size_t PathBeginLocation = 0;
bool existInDatabase[MAX_ROW];

// only for two different tables: ContentPath, AliasNamePair
static bool createTableContentPath(void)
{
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS ContentPath (\
                                ID INTEGER PRIMARY KEY AUTOINCREMENT, \
                                RelativePath TEXT NOT NULL UNIQUE, \
                                FileName TEXT, \
                                InnerName TEXT UNIQUE, \
                                ModifiedTime INTERGER,\
                                TYPE INTERGER,\
                                FileType INTERGER, \
                                ParentID INTEGER, \
                                FOREIGN KEY (ParentID) REFERENCES DirectoryTree(ID));";

    if (sqlite3_exec(db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to create table: %s\n", sqlite3_errmsg(db));
        return false;
    }

    return true;
}
static bool createTableAliasNamePair(void)
{
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS AliasNamePair (\
                                ID INTEGER PRIMARY KEY AUTOINCREMENT, \
                                Alias TEXT NOT NULL UNIQUE, \
                                Name TEXT);";

    if (sqlite3_exec(db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to create table: %s\n", sqlite3_errmsg(db));
        return false;
    }

    return true;
}
static bool createTableDeletedRow(void)
{
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS DeletedRow (\
                                ID INTEGER PRIMARY KEY AUTOINCREMENT, \
                                FileName TEXT UNIQUE, \
                                InnerName TEXT UNIQUE, \
                                TYPE INTERGER,\
                                FileType INTERGER);";

    if (sqlite3_exec(db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to create table: %s\n", sqlite3_errmsg(db));
        return false;
    }

    return true;
}
static int setSQLiteMem(void)
{
    sqlite3_mem_methods mem_methods = {0};
    mem_methods.xMalloc = SDL_SQLite_malloc;
    mem_methods.xFree = SDL_SQLite_free;
    mem_methods.xRealloc = SDL_SQLite_realloc;
    mem_methods.xSize = SDL_SQLite_memSize;
    mem_methods.xRoundup = SDL_SQLite_RoundUp;
    mem_methods.xInit = SDL_SQLite_Init;
    mem_methods.xShutdown = SDL_SQLite_shutDown;
    mem_methods.pAppData = NULL;

    return sqlite3_config(SQLITE_CONFIG_MALLOC, &mem_methods);
}
// judge shader file by suffix
static bool isShader(const char * name)
{
    static const char * shaderSuffix[] = {
        "frag", "vert", "comp", "tesc", "tese",
        "rgen", "rint", "rahit", "rchit", "rmiss",
        "rcall", "mesh", "task",
    };
    static const int count = (int)(sizeof(shaderSuffix) / sizeof(shaderSuffix[0]));
    int i;

    for (i = 0;i < count;i++) if (SDL_strcmp(name, shaderSuffix[i]) == 0) return true;
    
    return false;
}

static void writePathFile(const char * PathPath, Fixed_File * template, int templateCount)
{    
    int rowsCount = 0;
    rowsCount = getRowsCount() * 2;
    SDL_IOStream * io = NULL;
    io = SDL_IOFromFile(PathPath, "wb");
    int fixedFileCount = 0;
    int here = 0;
    int i, j;
    for (i = 0;i < templateCount;i++)
    {
        if (*template[i].name == '\0') 
        {
            char alias[255];
            SDL_snprintf(alias, 255, "[%s]:\n", template[i].alias);
            SDL_WriteIO(io, alias, SDL_strlen(alias));
            SDL_WriteIO(io, "\n\n", SDL_strlen("\n\n"));
            fixedFileCount++;
            here = i;
        }
    }
    here++;
    for (i = here;i < templateCount;i++)
    {
        char alias[255];
        SDL_snprintf(alias, 255, "[%s]:\n", template[i].alias);
        SDL_WriteIO(io, alias, SDL_strlen(alias));
        // SDL_Log(alias);
        SDL_snprintf(alias, 255, "%s\n\n", template[i].name);
        SDL_WriteIO(io, alias, SDL_strlen(alias));
        // SDL_Log(alias);
        fixedFileCount++;
        here = i;
    }
    for (i = 0;i < rowsCount;i++)
    {
        if (getPathType(i) == SDL_PATHTYPE_FILE)
        {
            char * temp = getPathByID(i);
            // if (fixedFileCount != templateCount)
            // {

            // }
            char alias[255];
            bool jump = false;

            SDL_strlcpy(alias, SDL_strrchr(temp, SEPRATOR_C) + 1, 255);
            if (fixedFileCount != templateCount)
            {
                for (j = 0;j < templateCount;j++)
                {
                    if (SDL_strcmp(alias, template[j].name) == 0)
                    {
                        SDL_strlcpy(alias, template[j].alias, 255);
                        fixedFileCount++;
                        jump = true;
                    }
                }
            }
            // SDL_Log(alias);
            bool font = false;
            bool shader = false;

            if (!jump)
            {
                alias[0] = (char)SDL_toupper(alias[0]);
                char * ptr2 = SDL_strrchr(temp, '.');

                if (ptr2 != NULL)
                {
                    if (SDL_strcmp(ptr2, ".spv") == 0)
                    {
                        *ptr2 = '\0';
                        font = true;
                        ptr2 = SDL_strchr(temp, '.');
                        if (isShader(ptr2 + 1)) shader = true;
                    }

                    char * ptr;
                    if (font) ptr = SDL_strchr(alias, '.');
                    else ptr = SDL_strrchr(alias, '.');
                    SDL_strlcpy(ptr, ptr2 + 1, 255);
                    *ptr = (char)SDL_toupper(*ptr);
                    if (shader) SDL_strlcat(alias, "Shader", 255);
                }
            }
            char test[255];
            SDL_snprintf(test, 255, "[%s]:\n", alias);
            SDL_WriteIO(io, test, SDL_strlen(test));

            if (shader) SDL_strlcat(temp, ".spv\n", 255);
            else SDL_strlcat(temp, "\n", 255);

            SDL_WriteIO(io, temp, SDL_strlen(temp));
            SDL_WriteIO(io, "\n", SDL_strlen("\n"));
            G_free(temp);
        }
    }
    SDL_CloseIO(io);
}
int generatePath(int argc, char * argv[])
{
    Uint64 timeStart = SDL_GetPerformanceCounter();

    int res = 0;
    bool closeDB = false;

    char dataBasePath[255];
    SDL_strlcpy(dataBasePath, argv[0], 255);

    char * slash = SDL_strrchr(dataBasePath, SEPRATOR_C);
    if (slash != NULL) *(slash + 1) = '\0';
    
    PathBeginLocation = SDL_strlen(dataBasePath);

    char ContentPath[255] = {0};
    char PathPath[255] = {0};

    SDL_strlcpy(ContentPath, dataBasePath, 255);
    SDL_strlcat(ContentPath, "Content", 255);
    
    SDL_strlcpy(PathPath, dataBasePath, 255);
    SDL_strlcat(PathPath, "Path", 255);
    SDL_strlcat(dataBasePath, "Content.db", 255);

    initFileTypeHashTable();
    setSQLiteMem();

    Fixed_File template[] = {
        {"MainFont", "SourceHanSansSC-VF.ttf"},
        {"EmojiFont", "OpenMoji-color-glyf_colr_0.ttf"},
        {"Font1", ""},
        {"FontHashTable1", ""},
        {"FontPng1", ""},
        {"DepthImage", ""},
        {"CustomePath1", ""},
        {"CustomePath2", ""},
        {"CustomePath3", ""},
        {"CustomePath4", ""},
        {"LogPath", "log"SEPRATOR"log0.txt"},
        {"PathPath", "Path"},
    };
    int templateCount = (int)(sizeof(template) / sizeof(template[0]));

    if (sqlite3_open(dataBasePath, &db) != SQLITE_OK)
    {
        SDL_Log("failed to open database %s\n", sqlite3_errmsg(db));
        res = -1;
        goto cleanup;
    }

    closeDB = true;

    if (!tableExistJudge())
    {
        if (createTableAliasNamePair() == false) 
        {
            res = -2;
            goto cleanup;
        }

        bool result = true;
        for (int i = 0;i < templateCount;i++)
        {
            result = insertNode_2(template[i].alias, template[i].name);
            if (result == false) 
            {
                res = -3;
                goto cleanup;
            }
        }
    }

    if (createTableContentPath() == false)
    {
        res = -4;
        goto cleanup;
    }

    createTableDeletedRow();

    DB_Path pack = {0};
    pack.R_Begin = (Uint16)PathBeginLocation;
    pack.LenGetId = pack.R_Begin;
    pack.A_path = ContentPath;

    updateDatabase(&pack);

    writePathFile(PathPath, template, templateCount);

    Uint64 ns = ((SDL_GetPerformanceCounter() - timeStart) * 1000000000ULL) / SDL_GetPerformanceFrequency();
    SDL_Log("time used: %llu ns, %lf ms, %lf s\n", ns, (double)ns / 1000000ULL, (double)ns / 1000000000ULL);

    cleanup:

    sqlite3_close(db);

    return res;
}

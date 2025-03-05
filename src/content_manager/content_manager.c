#include "content_manager/content_manager.h"
#include "G_constants.h"

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_iostream.h"

#include "sqlite3/sqlite3_alloc_func.h"

static int PathBeginLocation = 0;
static bool existInDatabase[MAX_ROW];

// only for two different tables: ContentPath, AliasNamePair
static void createTable(const char * tableName, sqlite3 * db)
{
    if (SDL_strcmp(tableName, "ContentPath") == 0)
    {
        const char *createTableSQL = "CREATE TABLE IF NOT EXISTS ContentPath (\
                                    ID INTEGER PRIMARY KEY AUTOINCREMENT, \
                                    Name TEXT NOT NULL UNIQUE, \
                                    ModifiedTime INTERGER,\
                                    TYPE INTERGER,\
                                    ParentID INTEGER, \
                                    FOREIGN KEY (ParentID) REFERENCES DirectoryTree(ID));";

        if (sqlite3_exec(db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) 
        {
            SDL_Log("Failed to create table: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return;
        }
    }
    else if (SDL_strcmp(tableName, "AliasNamePair") == 0)
    {
        const char *createTableSQL = "CREATE TABLE IF NOT EXISTS AliasNamePair (\
                                    ID INTEGER PRIMARY KEY AUTOINCREMENT, \
                                    Alias TEXT NOT NULL UNIQUE, \
                                    Name TEXT);";

        if (sqlite3_exec(db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) 
        {
            SDL_Log("Failed to create table: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return;
        }
    }
}
// insert to AliasNamePair
static void insertNode_2(sqlite3 *db, const char * alias, const char * name)
{
    static const char * insertSQL = "INSERT OR IGNORE INTO AliasNamePair (Alias, Name) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, alias, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_finalize(stmt);
}
// insert to minest ID row in ContentPath
static void insertNode(sqlite3 *db, const char *name, int parentID, Sint64 timeStamp, int type) 
{
    static const char *insertSQL = "INSERT INTO ContentPath (Name, ParentID, ModifiedTime, TYPE) VALUES (?, ?, ?, ?);";
    static const char *insertSQL_ID = "INSERT INTO ContentPath (ID, Name, ParentID, ModifiedTime, TYPE) VALUES (?, ?, ?, ?, ?);";
    static const char *findMinestIDSQL = "SELECT t1.ID + 1 AS first_unused_id \
                                        FROM ( \
                                        SELECT ID \
                                        FROM ContentPath \
                                        ORDER BY ID \
                                        ) AS t1 \
                                        LEFT JOIN ( \
                                        SELECT ID \
                                        FROM ContentPath \
                                        ORDER BY ID \
                                        ) AS t2 ON t1.ID + 1 = t2.ID \
                                        WHERE t2.ID IS NULL \
                                        LIMIT 1";
    sqlite3_stmt *stmt;
    int gap_id = 0;

    if (sqlite3_prepare_v2(db, findMinestIDSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        gap_id = sqlite3_column_int(stmt, 0);
    }
    else if (rc == SQLITE_DONE)
    {
        gap_id = -1;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_finalize(stmt);

    if (gap_id == -1)
    {
        if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL) != SQLITE_OK) 
        {
            SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            return;
        }

        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, parentID);
        sqlite3_bind_int64(stmt, 3, timeStamp) ;
        sqlite3_bind_int(stmt, 4, type);

        if (sqlite3_step(stmt) != SQLITE_DONE) 
        {
            SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
            return;
        }
        sqlite3_finalize(stmt);
    }
    else
    {
        if (sqlite3_prepare_v2(db, insertSQL_ID, -1, &stmt, NULL) != SQLITE_OK) 
        {
            SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            return;
        }

        sqlite3_bind_int(stmt, 1, gap_id);
        sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, parentID);
        sqlite3_bind_int64(stmt, 4, timeStamp);
        sqlite3_bind_int(stmt, 5, type);

        if (sqlite3_step(stmt) != SQLITE_DONE) 
        {
            SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
            return;
        }
        sqlite3_finalize(stmt);
    }

    return;
}
// judge if table AliasNamePair exist
static bool tableExistJudge(sqlite3 * db)
{
    static const char * SQL = "SELECT name \
                            FROM sqlite_master \
                            WHERE type='table' \
                            AND name='AliasNamePair';";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return true;
    }

    return false;
}
// get ID in ContentPath by Name
static int getID(sqlite3 *db, const char * name)
{
    static const char *findSQL = "SELECT ID FROM ContentPath WHERE Name = ?;";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, findSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        rc = sqlite3_column_int(stmt, 0);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = -1;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        rc = -1;
    }

    sqlite3_finalize(stmt);

    return rc;
}
// get ParentID in ContentPath by ID
static int getParentID(sqlite3 *db, const int ID)
{
    static const char *findSQL = "SELECT ParentID FROM ContentPath WHERE ID = ?;";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, findSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, ID);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        rc = sqlite3_column_int(stmt, 0);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = -1;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        rc = -1;
    }

    sqlite3_finalize(stmt);

    return rc;
}
// get MOdifiedTime in ContentPath by ID
static Sint64 getModifyTime(sqlite3 * db, const int ID)
{
    const char * SQL = "SELECT ModifiedTime FROM ContentPath WHERE ID = ?";
    sqlite3_stmt * stmt;
    int rc = 0;
    Sint64 timeStamp = 0;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -3;
    }

    sqlite3_bind_int(stmt, 1, ID);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        timeStamp = sqlite3_column_int64(stmt, 0);
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        timeStamp = -1;
    }

    sqlite3_finalize(stmt);
    return timeStamp;
}
// get row TYPE(folder / file) in ContentPath by ID
static SDL_PathType getPathType(sqlite3 * db, const int id)
{
    const char * SQL = "SELECT TYPE FROM ContentPath WHERE ID = ?";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -3;
    }

    sqlite3_bind_int(stmt, 1, id);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        rc = sqlite3_column_int(stmt, 0);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = -1;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        rc = -1;
    }

    sqlite3_finalize(stmt);

    return rc;
}
// get Name in ContentPath by ID
static char * getName(sqlite3 * db, const int ID)
{
    const char * SQL = "SELECT Name FROM ContentPath WHERE ID = ?";
    sqlite3_stmt * stmt;
    

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, ID);

    int rc = sqlite3_step(stmt);
    char * name;
    if (rc == SQLITE_ROW)
    {
        const unsigned char * temp = sqlite3_column_text(stmt, 0);
        name = (char *)SDL_malloc(strlen((const char*)temp) + 1);
        SDL_strlcpy(name, (const char*)temp, 255);
    }
    else if (rc == SQLITE_DONE)
    {
        name = NULL;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        name = NULL;
    }

    sqlite3_finalize(stmt);
    return name;
}
// update ModifiedTime in ContentPath by ID
static void updateModifyTime(sqlite3 * db, Sint64 timeStamp, const int ID)
{
    const char * SQL = "UPDATE ContentPath SET ModifiedTime = ? WHERE ID = ?";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int64(stmt, 1, timeStamp);
    sqlite3_bind_int(stmt, 2, ID);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_finalize(stmt);

    return;
}
// update ParentID in ContentPath by ID (parent folder changed)
static void updateParentID(sqlite3 * db, int parentID, int ID)
{
    const char * SQL = "UPDATE ContentPath SET ParentID = ? WHERE ID = ?";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int64(stmt, 1, parentID);
    sqlite3_bind_int(stmt, 2, ID);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_finalize(stmt);

    return;
}
// recursion to get all file and folder in Content Folder and create database
static SDL_EnumerationResult SDLCALL createFolderDatabase(void *userdata, const char *dirname, const char *fname)
{
    if (fname != NULL)
    {
        SDL_PathInfo info = {0};
        char temp_A_Path[255];
        SDL_strlcpy(temp_A_Path, ((DB_Path*)userdata)->A_path, 255);

        SDL_strlcat(temp_A_Path, SEPRATOR, 255);

        SDL_strlcat(temp_A_Path, fname, 255);
        SDL_GetPathInfo(temp_A_Path, &info);
        // SDL_Log(temp_A_Path);
        // SDL_Log(temp_A_Path + ((DB_Path*)userdata)->R_Begin);
        // SDL_Log("type: %d, size: %llu, dirname:%s, fname:%s, begin: %u", info.type, info.size, dirname, fname, ((DB_Path*)userdata)->R_Begin);
        insertNode(((DB_Path*)userdata)->db, temp_A_Path + ((DB_Path*)userdata)->R_Begin, getID(((DB_Path*)userdata)->db, ((DB_Path*)userdata)->A_path + ((DB_Path*)userdata)->LenGetId),
                     info.modify_time, (int)info.type);
        if (info.type == SDL_PATHTYPE_DIRECTORY)
        {
            DB_Path pack = {0};
            pack.LenGetId = ((DB_Path*)userdata)->R_Begin;
            pack.R_Begin = pack.LenGetId + SDL_strlen(dirname + ((DB_Path*)userdata)->R_Begin);
            pack.db = ((DB_Path*)userdata)->db;
            pack.A_path = temp_A_Path;
            SDL_EnumerateDirectory(temp_A_Path, createFolderDatabase, &pack);
        }
    }
    else if (fname == NULL)
    {
        SDL_Log("End");
        return SDL_ENUM_SUCCESS;
    }

    return SDL_ENUM_CONTINUE;
}
// recursion to update all file and folder in Content Folder and create database
static SDL_EnumerationResult SDLCALL updateFolderDatabase(void *userdata, const char *dirname, const char *fname)
{
    if (fname != NULL)
    {
        SDL_PathInfo info = {0};
        char temp_A_Path[255];
        SDL_strlcpy(temp_A_Path, ((DB_Path*)userdata)->A_path, 255);

        SDL_strlcat(temp_A_Path, SEPRATOR, 255);

        SDL_strlcat(temp_A_Path, fname, 255);
        SDL_GetPathInfo(temp_A_Path, &info);
        // SDL_Log(temp_A_Path);
        // SDL_Log(temp_A_Path + ((DB_Path*)userdata)->R_Begin);
        // SDL_Log("type: %d, size: %llu, dirname:%s, fname:%s, begin: %u", info.type, info.size, dirname, fname, ((DB_Path*)userdata)->R_Begin);

        sqlite3 * tempDB = ((DB_Path*)userdata)->db;
        const char * temp_R_Path = temp_A_Path + ((DB_Path*)userdata)->R_Begin;
        int ID = getID(tempDB, temp_R_Path);
        int ParentID = getID(tempDB, ((DB_Path*)userdata)->A_path + ((DB_Path*)userdata)->LenGetId);

        if (ID < 0)
        {
            insertNode(tempDB, temp_R_Path, ParentID,
                     info.modify_time, (int)info.type);
            existInDatabase[getID(tempDB, temp_R_Path)] = true;
        }
        else if (info.modify_time > getModifyTime(tempDB, ID))
        {
            updateModifyTime(tempDB, info.modify_time, ID);
            if (getParentID(tempDB, ID) != ParentID)
            {
                updateParentID(tempDB, ParentID, ID);
            }
            existInDatabase[ID] = true;
        }
        else
        {
            if (getParentID(tempDB, ID) != ParentID)
            {
                updateParentID(tempDB, ParentID, ID);
            }
            existInDatabase[ID] = true;
        }

        if (info.type == SDL_PATHTYPE_DIRECTORY)
        {
            DB_Path pack = {0};
            pack.LenGetId = ((DB_Path*)userdata)->R_Begin;
            pack.R_Begin = pack.LenGetId + SDL_strlen(dirname + ((DB_Path*)userdata)->R_Begin);
            pack.db = ((DB_Path*)userdata)->db;
            pack.A_path = temp_A_Path;
            SDL_EnumerateDirectory(temp_A_Path, updateFolderDatabase, &pack);
        }
    }
    else if (fname == NULL)
    {
        // SDL_Log("End");
        return SDL_ENUM_SUCCESS;
    }

    return SDL_ENUM_CONTINUE;
}
// delete row in ContentPath by ID
static bool deleteRow(sqlite3 *db, const int ID)
{
    const char * SQL = "DELETE FROM ContentPath WHERE ID = ?";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_int(stmt, 1, ID);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    sqlite3_finalize(stmt);

    return true;
}
// get row count in ContentPath
static int getRowsCount(sqlite3 * db)
{
    const char * SQL = "SELECT COUNT(*) FROM ContentPath";
    sqlite3_stmt * stmt;
    int rc = 0;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -3;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        rc = sqlite3_column_int(stmt, 0);
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        rc = -1;
    }

    sqlite3_finalize(stmt);
    return rc;
}
// get absolutely path by joint relative path 
static char * getPathByID(sqlite3 * db, const int ID)
{
    int IDs[15];
    IDs[14] = ID;
    int parentID = getParentID(db, ID);
    IDs[13] = parentID;
    int i = 13;
    while (parentID != 1)
    {
        parentID = getParentID(db, parentID);
        IDs[--i] = parentID;
    }
    char *path = (char *)SDL_calloc(255, 1);
    i++;
    char * temp = getName(db, IDs[i]);
    SDL_strlcat(path, temp, 255);
    // SDL_Log(path);
    SDL_free(temp);
    i++;
    for (;i < 15;i++)
    {
        char * temp = getName(db, IDs[i]);
        SDL_strlcat(path, SDL_strchr(temp, SEPRATOR_C), 255);

        // SDL_strlcat(path, temp, 255);
        // SDL_Log(path);
        SDL_free(temp);
    }
    // char * temp = getName(db, IDs[i]);
    // SDL_strlcat(path, temp, 255);
    // // SDL_snprintf(path, 255, "%s ", temp);
    // SDL_Log(path);
    // SDL_free(temp);

    return path;
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
    const char * shaderSuffix[] = {
        "frag", "vert", "comp", "tesc", "tese",
        "rgen", "rint", "rahit", "rchit", "rmiss",
        "rcall", "mesh", "task",
    };
    const int count = (int)(sizeof(shaderSuffix) / sizeof(shaderSuffix[0]));

    for (int i = 0;i < count;i++) if (SDL_strcmp(name, shaderSuffix[i]) == 0) return true;
    
    return false;
}
int generatePath(int argc, char * argv[])
{
    char dataBasePath[255];

    SDL_strlcpy(dataBasePath, argv[0], 255);

    char * slash = SDL_strrchr(dataBasePath, SEPRATOR_C);
    if (slash != NULL) *(slash + 1) = '\0';
    
    PathBeginLocation = SDL_strlen(dataBasePath);

    char ContentPath[255] = {0};
    SDL_strlcpy(ContentPath, dataBasePath, 255);

    SDL_strlcat(ContentPath, "Content", 255);
    // SDL_Log(ContentPath);
    
    char PathPath[255] = {0};
    SDL_strlcpy(PathPath, dataBasePath, 255);

    SDL_strlcat(PathPath, "Path", 255);
    // SDL_Log(PathPath);

    SDL_strlcat(dataBasePath, "Content.db", 255);
    // SDL_Log(dataBasePath);

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

    sqlite3 * db;

    if (sqlite3_open(dataBasePath, &db) != SQLITE_OK)
    {
        SDL_Log("failed to open database %s\n", sqlite3_errmsg(db));
    }
    if (!tableExistJudge(db))
    {
        createTable("AliasNamePair", db);
        for (int i = 0;i < templateCount;i++)
        {
            insertNode_2(db, template[i].alias, template[i].name);
        }
        // SDL_Log("create AliasNamePair");
    }

    createTable("ContentPath", db);

    DB_Path pack = {0};

    pack.R_Begin = (Uint16)PathBeginLocation;
    pack.LenGetId = pack.R_Begin;

    pack.db = db;
    pack.A_path = ContentPath;

    SDL_PathInfo info = {0};
    SDL_GetPathInfo(ContentPath, &info);

    existInDatabase[0] = true;

    int ID = getID(db, ContentPath + pack.R_Begin);

    int rowsCount = 0;
    if (ID == 1)
    {
        if (info.modify_time > getModifyTime(db, ID))
        {
            updateModifyTime(db, info.modify_time, ID);
            // SDL_Log("update");
        }
        existInDatabase[ID] = true;

        // SDL_Log("count: %d", rowsCount);
        SDL_EnumerateDirectory(ContentPath, updateFolderDatabase, &pack);

        rowsCount = getRowsCount(db);
        rowsCount *= 2;
        // SDL_Log("count: %d", rowsCount);
        for (int i = 0;i < MAX_ROW;i++)
        {
            if (!existInDatabase[i])
            {
                deleteRow(db, i);
            }
            if (i > rowsCount) break;
        }
        
    }
    else 
    {
        insertNode(db, ContentPath + pack.R_Begin, 0, info.modify_time, (int)info.type);

        SDL_EnumerateDirectory(ContentPath, createFolderDatabase, &pack);
    }

    rowsCount = getRowsCount(db) * 2;
    SDL_IOStream * io = NULL;
    io = SDL_IOFromFile(PathPath, "wb");
    int fixedFileCount = 0;
    int here = 0;
    for (int i = 0;i < templateCount;i++)
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
    for (int i = here;i < templateCount;i++)
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
    for (int i = 0;i < rowsCount;i++)
    {
        if (getPathType(db, i) == SDL_PATHTYPE_FILE)
        {
            char * temp = getPathByID(db, i);
            // if (fixedFileCount != templateCount)
            // {

            // }
            char alias[255];
            bool jump = false;

            SDL_strlcpy(alias, SDL_strrchr(temp, SEPRATOR_C) + 1, 255);
            if (fixedFileCount != templateCount)
            {
                for (int j = 0;j < templateCount;j++)
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
                alias[0] = SDL_toupper(alias[0]);
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
                    *ptr = SDL_toupper(*ptr);
                    if (shader) SDL_strlcat(alias, "Shader", 255);
                }
            }
            char test[255];
            SDL_snprintf(test, 255, "[%s]:\n", alias);
            // SDL_Log(test);
            SDL_WriteIO(io, test, SDL_strlen(test));

            if (shader)
            {
                SDL_strlcat(temp, ".spv\n", 255);
            }
            else
            {
                SDL_strlcat(temp, "\n", 255);
            }
            // SDL_Log(temp);
            SDL_WriteIO(io, temp, SDL_strlen(temp));
            SDL_WriteIO(io, "\n", SDL_strlen("\n"));
            SDL_free(temp);
        }
    }
    SDL_CloseIO(io);

    sqlite3_close(db);

    return 0;
}
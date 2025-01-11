#include "contentManager/content_manager.h"
#include "sqlite3/sqlite3_alloc_func.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "sqlite3/sqlite3.h"

typedef struct _DB_Path
{
    sqlite3 * db;
    char * A_path;
    uint16_t R_Begin;
    uint16_t LenGetId;
} DB_Path;

#define MAX_ROW 1024

static int PathBeginLocation = 0;
static bool existInDatabase[MAX_ROW];

// the returned text should be freed by SDL_Free()
const char * setStatementByName(const char * tableName, char * SQLstatement)
{
    char * SQL = (char *)SDL_calloc(100 + SDL_strlen(SQLstatement), 1);
    SDL_Log(tableName);
    SDL_Log(SQLstatement);
    SDL_snprintf(SQL, 100 + SDL_strlen(SQLstatement), SQLstatement, tableName);
    SDL_Log(SQL);

    return SQL;
}

void initDatabase(const char *databasePath, const char * tableName, sqlite3 ** db)
{
    if (sqlite3_open(databasePath, db) != SQLITE_OK)
    {
        SDL_Log("failed to open database %s\n", sqlite3_errmsg(*db));
        return;
    }

    const char *createTableSQL = setStatementByName(tableName, "CREATE TABLE IF NOT EXISTS %s (\
                                ID INTEGER PRIMARY KEY AUTOINCREMENT, \
                                Name TEXT NOT NULL UNIQUE, \
                                ModifiedTime INTERGER,\
                                TYPE INTERGER,\
                                ParentID INTEGER, \
                                FOREIGN KEY (ParentID) REFERENCES DirectoryTree(ID));");

    if (sqlite3_exec(*db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to create table: %s\n", sqlite3_errmsg(*db));
        SDL_free((void*)createTableSQL);
        sqlite3_close(*db);
        return;
    }

    SDL_free((void*)createTableSQL);

    return;
}
void insertNode(sqlite3 *db, const char *name, int parentID, int64_t timeStamp, int type) 
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
int getID(sqlite3 *db, const char * name)
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

int getParentID(sqlite3 *db, const int ID)
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
int64_t getModifyTime(sqlite3 * db, const int ID)
{
    const char * SQL = "SELECT ModifiedTime FROM ContentPath WHERE ID = ?";
    sqlite3_stmt * stmt;
    int rc = 0;
    int64_t timeStamp = 0;

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
void updateModifyTime(sqlite3 * db, int64_t timeStamp, const int ID)
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
void updateParentID(sqlite3 * db, int parentID, int ID)
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
SDL_EnumerationResult SDLCALL createFolderDatabase(void *userdata, const char *dirname, const char *fname)
{
    if (fname != NULL)
    {
        SDL_PathInfo info = {0};
        char temp_A_Path[255];
        SDL_strlcpy(temp_A_Path, ((DB_Path*)userdata)->A_path, 255);

#if defined(_WIN32)
        SDL_strlcat(temp_A_Path, "\\", 255);
#elif defined(__linux__)
        SDL_strlcat(temp_A_Path, "/", 255);
#endif

        SDL_strlcat(temp_A_Path, fname, 255);
        SDL_GetPathInfo(temp_A_Path, &info);
        SDL_Log(temp_A_Path);
        SDL_Log("type: %d, size: %llu, dirname:%s, fname:%s, begin: %u", info.type, info.size, dirname, fname, ((DB_Path*)userdata)->R_Begin);
        insertNode(((DB_Path*)userdata)->db, temp_A_Path + ((DB_Path*)userdata)->R_Begin, getID(((DB_Path*)userdata)->db, ((DB_Path*)userdata)->A_path + ((DB_Path*)userdata)->LenGetId),
                     info.modify_time, (int)info.type);
        if (info.type == SDL_PATHTYPE_DIRECTORY)
        {
            DB_Path pack = {0};
            pack.LenGetId = ((DB_Path*)userdata)->R_Begin;
            pack.R_Begin = pack.LenGetId + SDL_strlen(dirname + ((DB_Path*)userdata)->R_Begin) + 1;
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
SDL_EnumerationResult SDLCALL updateFolderDatabase(void *userdata, const char *dirname, const char *fname)
{
    if (fname != NULL)
    {
        SDL_PathInfo info = {0};
        char temp_A_Path[255];
        SDL_strlcpy(temp_A_Path, ((DB_Path*)userdata)->A_path, 255);

#if defined(_WIN32)
        SDL_strlcat(temp_A_Path, "\\", 255);
#elif defined(__linux__)
        SDL_strlcat(temp_A_Path, "/", 255);
#endif

        SDL_strlcat(temp_A_Path, fname, 255);
        SDL_GetPathInfo(temp_A_Path, &info);
        SDL_Log(temp_A_Path);
        SDL_Log("type: %d, size: %llu, dirname:%s, fname:%s, begin: %u", info.type, info.size, dirname, fname, ((DB_Path*)userdata)->R_Begin);

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
            pack.R_Begin = pack.LenGetId + SDL_strlen(dirname + ((DB_Path*)userdata)->R_Begin) + 1;
            pack.db = ((DB_Path*)userdata)->db;
            pack.A_path = temp_A_Path;
            SDL_EnumerateDirectory(temp_A_Path, updateFolderDatabase, &pack);
        }
    }
    else if (fname == NULL)
    {
        SDL_Log("End");
        return SDL_ENUM_SUCCESS;
    }

    return SDL_ENUM_CONTINUE;
}
bool deleteRow(sqlite3 *db, const int ID)
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
int getRowsCount(sqlite3 * db)
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
int main(int argc, char * argv[])
{
    if (argc < 3) return -11;

    sqlite3_mem_methods mem_methods = {0};
    mem_methods.xMalloc = SDL_SQLite_malloc;
    mem_methods.xFree = SDL_SQLite_free;
    mem_methods.xRealloc = SDL_SQLite_realloc;
    mem_methods.xSize = SDL_SQLite_memSize;
    mem_methods.xRoundup = SDL_SQLite_RoundUp;
    mem_methods.xInit = SDL_SQLite_Init;
    mem_methods.xShutdown = SDL_SQLite_shutDown;
    mem_methods.pAppData = NULL;

    sqlite3_config(SQLITE_CONFIG_MALLOC, &mem_methods);

    sqlite3 * db;
    initDatabase(argv[2], "ContentPath", &db);
    
    DB_Path pack = {0};

    char ContentPath[255] = {0};
    SDL_strlcpy(ContentPath, argv[1], 255);

    PathBeginLocation = SDL_strlen(ContentPath);

    pack.R_Begin = (uint16_t)PathBeginLocation;
    pack.LenGetId = pack.R_Begin;

    SDL_strlcat(ContentPath, "Content", 255);
    SDL_Log(ContentPath);

    pack.db = db;
    pack.A_path = ContentPath;

    SDL_PathInfo info = {0};
    SDL_GetPathInfo(ContentPath, &info);

    existInDatabase[0] = true;

    int ID = getID(db, ContentPath + pack.R_Begin);

    if (ID == 1)
    {
        if (info.modify_time > getModifyTime(db, ID))
        {
            updateModifyTime(db, info.modify_time, ID);
            SDL_Log("update");
        }
        existInDatabase[ID] = true;

        int rowsCount = getRowsCount(db);
        SDL_Log("count: %d", rowsCount);
        SDL_EnumerateDirectory(ContentPath, updateFolderDatabase, &pack);

        rowsCount = getRowsCount(db);
        rowsCount *= 2;
        SDL_Log("count: %d", rowsCount);
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

    sqlite3_close(db);

    return 0;
}
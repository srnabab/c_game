#include "contentManager/content_manager.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "sqlite3/sqlite3.h"

static int PathBeginLocation = 0;

// the returned text should be freed by SDL_Free()
// const char * chooseTableName(const char * tableName, char * SQLstatement)
// {
//     char * SQL = (char *)SDL_calloc(100 + SDL_strlen(SQLstatement), 1);
//     SDL_strlcpy(SQL, SQLstatement);
//     SDL_snprintf(SQL, 100 + SDL_strlen(SQLstatement), SQL, tableName);

//     return SQL;
// }
void initDatabase(const char *databasePath, sqlite3 ** db)
{
    if (sqlite3_open(databasePath, db) != SQLITE_OK)
    {
        SDL_Log("failed to open database %s\n", sqlite3_errmsg(*db));
    }

    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS ContentPath (\
                                ID INTEGER PRIMARY KEY AUTOINCREMENT, \
                                Name TEXT NOT NULL, \
                                ModifiedTime INTERGER,\
                                ParentID INTEGER, \
                                FOREIGN KEY (ParentID) REFERENCES DirectoryTree(ID));";

    if (sqlite3_exec(*db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to create table: %s\n", sqlite3_errmsg(*db));
        // SDL_free(createTableSQL);
        sqlite3_close(*db);
    }

    // SDL_free(createTableSQL);
}
void insertNode(sqlite3 *db, const char *name, int parentID, int64_t timeStamp) 
{
    static const char *insertSQL = "INSERT INTO ContentPath (Name, ParentID, ModifiedTime) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, parentID);
    sqlite3_bind_int64(stmt, 3, timeStamp) ;

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
}
int getID(sqlite3 *db, const char * name)
{
    static const char *findSQL = "SELECT ID FROM ContentPath WHERE Name = ?;";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, findSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -3;
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
        rc = -2;
    }

    sqlite3_finalize(stmt);

    return rc;
}
typedef struct _DB_Path
{
    sqlite3 * db;
    char * A_path;
    uint16_t R_Begin;
    uint16_t LenGetId;
} DB_Path;
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
                     info.modify_time);
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
int main(int argc, char * argv[])
{
    sqlite3 * db;
    initDatabase(argv[2], &db);
    
    DB_Path pack = {0};

    char ContentPath[255] = {0};
    SDL_strlcpy(ContentPath, argv[1], 255);

    PathBeginLocation = SDL_strlen(ContentPath);

    pack.R_Begin = (uint16_t)PathBeginLocation;
    pack.LenGetId = pack.R_Begin;

    SDL_strlcat(ContentPath, "Content", 255);
    SDL_Log(ContentPath);

    SDL_PathInfo info = {0};
    SDL_GetPathInfo(ContentPath, &info);
    insertNode(db, ContentPath + pack.R_Begin, 0, info.modify_time);

    pack.db = db;
    pack.A_path = ContentPath;
    SDL_EnumerateDirectory(ContentPath, createFolderDatabase, &pack);

    // SDL_Delay(5000);

    sqlite3_close(db);

    return 0;
}
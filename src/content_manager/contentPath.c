#include "content_manager/content_manager.h"
#include "G_constants.h"

#include "G_file_type.h"

#include "G_allocator.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_iostream.h"

#include "SDL_stdinc.h"
#include "SDL_timer.h"
#include "sqlite3/sqlite3.h"
#include "sqlite3/sqlite3_alloc_func.h"
#include "uthash/uthash.h"
#include <rpcdce.h>
#include <string.h>

extern sqlite3 * db;
extern bool existInDatabase[MAX_ROW];
static FileTypeHashTable * fileTypeHash = NULL;

void initFileTypeHashTable(void)
{
    Uint32 i;

    static FileTypeHashTable allFileType[] = {
        {Folder, "", {0}},
        {Obj, ".obj", {0}},
        {Mtl, ".mtl", {0}},
        {Png, ".png", {0}},
        {Tsdi, ".tsdi", {0}},
        {Tsd, ".tsd", {0}},
        {Ttf, ".ttf", {0}},
        {Wav, ".wav", {0}},
        {Shader, ".spv", {0}},
        {Txt, ".txt", {0}},
        {HashTable, ".hash", {0}}, 
        {Unknown, ".unknow", {0}},
    };

    for (i = 0;i < Unknown;i++)
    {
        HASH_ADD_STR(fileTypeHash, suffix, allFileType + i);
    }
}
static FileType findFileType(const char * suffix)
{
    if (suffix == NULL) return Folder;
    FileTypeHashTable * temp = NULL;
    HASH_FIND_STR(fileTypeHash, suffix, temp);
    if (temp) return temp->fileType;
    return Unknown;
}
static int getID(const char * name, char * UUID)
{
    static const char *findSQL = "SELECT ID FROM ContentPath WHERE RelativePath = ?;";
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
        unsigned char * str = sqlite3_column_text(stmt, 0);
        memcpy(UUID, str, 38);
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
static Sint64 getModifyTime(const int ID)
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
// get row count in ContentPath
int getRowsCount(void)
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
// delete row in ContentPath by ID
static bool deleteRow(const int ID)
{
    const char * SQL = "DELETE FROM ContentPath WHERE ID = ?";
    sqlite3_stmt * stmt = NULL;
    int res = 0;
    bool finalize = false;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        res = -1;
        goto cleanup;
    }
    finalize = true;

    res |= sqlite3_bind_int(stmt, 1, ID);
    if (res)
    {
        SDL_Log("Failed to bind: %s\n", sqlite3_errmsg(db));
        res = -4;
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        res = -5;
        goto cleanup;
    }

    cleanup:

    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return res < 0 ? false : true;
}
static bool insertNodeInsertSQL_ID(const char *name, const char * parentID, Sint64 timeStamp, int type, Uint32 fileType, const char * ID)
{
    static const char *insertSQL_ID = "INSERT INTO ContentPath (ID, ParentID, RelativePath, FileName, TYPE, FileSize, ContentHash, ModifiedTime, LastSeenTime, FileType) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    bool finalize = false;
    int res = SQLITE_OK;
    sqlite3_stmt * stmt = NULL;

    if (sqlite3_prepare_v2(db, insertSQL_ID, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }
    finalize = true;

    char * slash = SDL_strrchr(name, SEPRATOR_C);
    if (slash) slash++;
    else slash = name;

    res |= sqlite3_bind_int(stmt, 1, gap_id);
    res |= sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    res |= sqlite3_bind_text(stmt, 3, slash, -1, SQLITE_STATIC);
    res |= sqlite3_bind_int(stmt, 4, parentID);
    res |= sqlite3_bind_int64(stmt, 5, timeStamp);
    res |= sqlite3_bind_int(stmt, 6, type);
    res |= sqlite3_bind_int(stmt, 7, fileType);
    if (res)
    {
        SDL_Log("Failed to bind: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }

    res = true;

    cleanup:
    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return res;
}
static int findMinesID(void)
{
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

    sqlite3_stmt *stmt = NULL;
    int gap_id = 1;
    bool finalize = false;

    if (sqlite3_prepare_v2(db, findMinestIDSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }
    finalize = true;

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        gap_id = sqlite3_column_int(stmt, 0);
    }
    else if (rc == SQLITE_DONE)
    {
        gap_id = 1;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    cleanup:

    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return gap_id;
}
static bool insertMetaData(const char * innerName, int ID)
{
    static const char * insertMetaData = "UPDATE ContentPath SET InnerName = ? WHERE ID = ?;";

    bool finalize = false;
    int res = false;
    sqlite3_stmt * stmt = NULL;

    if (sqlite3_prepare_v2(db, insertMetaData, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }
    finalize = true;

    res |= sqlite3_bind_text(stmt, 1, innerName, -1, SQLITE_STATIC);
    res |= sqlite3_bind_int(stmt, 2, ID);

    if (res)
    {
        SDL_Log("Failed to bind: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }

    cleanup:
    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return res;
}
// insert to minest ID row in ContentPath
static bool insertNode(const char *name, int parentID, Sint64 timeStamp, int type, Uint32 fileType, Uint64 fileSize)
{
    char innerName[16] = {0};
    unsigned char UUID[37];
    int res = false;

    // id = findSameDeletedRow(name, type, fileType, innerName);
    res = createNewUUID(UUID);

    res = insertNodeInsertSQL_ID(name, parentID, timeStamp, type, fileType, );
    contentID = getID(name);
    if (res > 0 && id > 0) 
    {
        res = insertMetaData(innerName, contentID);
        deleteRow3(id);
    }

    return res < 0 ? false : true;
}
// get ParentID in ContentPath by ID
static int getParentID(const int ID)
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

// get row TYPE(folder / file) in ContentPath by ID
SDL_PathType getPathType(const int id)
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
char * getName(const int ID)
{
    const char * SQL = "SELECT RelativePath FROM ContentPath WHERE ID = ?";
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
        name = (char *)G_malloc(strlen((const char*)temp) + 1);
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
static void updateModifyTime(Sint64 timeStamp, const int ID)
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
static void updateParentID(int parentID, int ID)
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
static FileType SDLCALL fileTypeJudge(const char * fname)
{
    char * temp;
    char * suffix = SDL_strrchr(fname, '.');

    temp = suffix;
    while (temp)
    {
        *temp = (char)SDL_tolower(*temp);
        temp++;
        if (*temp == '\0') break;
    }

    // SDL_Log("file: %s, suffix: %s", fname, suffix);

    if (suffix)
    {
        return findFileType(suffix);
    }

    return Folder;
}
// get absolutely path by joint relative path 
char * getPathByID(const int ID)
{
    int IDs[15];
    IDs[14] = ID;
    int parentID = getParentID(ID);
    IDs[13] = parentID;
    int i = 13;
    while (parentID != 1)
    {
        parentID = getParentID(parentID);
        IDs[--i] = parentID;
    }
    char *path = (char *)G_calloc(255, 1);
    i++;
    char * temp = getName(IDs[i]);
    SDL_strlcat(path, temp, 255);
    // SDL_Log(path);
    G_free(temp);
    i++;
    for (;i < 15;i++)
    {
        char * tempName = getName(IDs[i]);
        SDL_strlcat(path, SDL_strchr(tempName, SEPRATOR_C), 255);

        // SDL_strlcat(path, temp, 255);
        // SDL_Log(path);
        G_free(tempName);
    }
    // char * temp = getName(db, IDs[i]);
    // SDL_strlcat(path, temp, 255);
    // // SDL_snprintf(path, 255, "%s ", temp);
    // SDL_Log(path);
    // G_free(temp);

    return path;
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

        FileType fileType = Folder;

        if (info.type != SDL_PATHTYPE_DIRECTORY) fileType = fileTypeJudge(fname);

        // SDL_Log(temp_A_Path);
        // SDL_Log(temp_A_Path + ((DB_Path*)userdata)->R_Begin);
        // SDL_Log("type: %d, size: %llu, dirname:%s, fname:%s, begin: %u", info.type, info.size, dirname, fname, ((DB_Path*)userdata)->R_Begin);

        insertNode(temp_A_Path + ((DB_Path*)userdata)->R_Begin, getID(((DB_Path*)userdata)->A_path + ((DB_Path*)userdata)->LenGetId),
                     info.modify_time, (int)info.type, fileType);
        if (info.type == SDL_PATHTYPE_DIRECTORY)
        {
            DB_Path pack = {0};
            pack.LenGetId = ((DB_Path*)userdata)->R_Begin;
            pack.R_Begin = pack.LenGetId + (Uint16)SDL_strlen(dirname + ((DB_Path*)userdata)->R_Begin);
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
    if (fname)
    {
        SDL_PathInfo info = {0};
        char temp_A_Path[255];
        SDL_strlcpy(temp_A_Path, ((DB_Path*)userdata)->A_path, 255);

        SDL_strlcat(temp_A_Path, SEPRATOR, 255);

        SDL_strlcat(temp_A_Path, fname, 255);
        SDL_GetPathInfo(temp_A_Path, &info);

        FileType fileType = Folder;

        if (info.type != SDL_PATHTYPE_DIRECTORY) fileType = fileTypeJudge(fname);
        // SDL_Log(temp_A_Path);
        // SDL_Log(temp_A_Path + ((DB_Path*)userdata)->R_Begin);
        // SDL_Log("type: %d, size: %llu, dirname:%s, fname:%s, begin: %u", info.type, info.size, dirname, fname, ((DB_Path*)userdata)->R_Begin);

        const char * temp_R_Path = temp_A_Path + ((DB_Path*)userdata)->R_Begin;
        int ID = getID(temp_R_Path);
        int ParentID = getID(((DB_Path*)userdata)->A_path + ((DB_Path*)userdata)->LenGetId);

        if (ID < 0)
        {
            insertNode(temp_R_Path, ParentID,
                     info.modify_time, (int)info.type, fileType);
            existInDatabase[getID(temp_R_Path)] = true;
        }
        else if (info.modify_time > getModifyTime(ID))
        {
            updateModifyTime(info.modify_time, ID);
            if (getParentID(ID) != ParentID)
            {
                updateParentID(ParentID, ID);
            }
            existInDatabase[ID] = true;
        }
        else
        {
            if (getParentID(ID) != ParentID)
            {
                updateParentID(ParentID, ID);
            }
            existInDatabase[ID] = true;
        }

        if (info.type == SDL_PATHTYPE_DIRECTORY)
        {
            DB_Path pack = {0};
            pack.LenGetId = ((DB_Path*)userdata)->R_Begin;
            pack.R_Begin = pack.LenGetId + (Uint16)SDL_strlen(dirname + ((DB_Path*)userdata)->R_Begin);
            pack.A_path = temp_A_Path;
            SDL_EnumerateDirectory(temp_A_Path, updateFolderDatabase, &pack);
        }
    }
    else
    {
        // SDL_Log("End");
        return SDL_ENUM_SUCCESS;
    }

    return SDL_ENUM_CONTINUE;
}
bool updateDatabase(DB_Path * pPack)
{
    char * ContentPath = pPack->A_path;
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(ContentPath, &info);

    // UUID * uuid;
    // UuidCreate(UUID *Uuid)

    existInDatabase[0] = true;

    int i;
    int ID = getID(ContentPath + pPack->R_Begin);

    int rowsCount = 0;
    if (ID == 1)
    {
        if (info.modify_time > getModifyTime(ID))
        {
            updateModifyTime(info.modify_time, ID);
            // SDL_Log("update");
        }
        existInDatabase[ID] = true;

        // SDL_Log("count: %d", rowsCount);
        SDL_EnumerateDirectory(ContentPath, updateFolderDatabase, pPack);

        rowsCount = getRowsCount();
        rowsCount *= 2;
        // SDL_Log("count: %d", rowsCount);
        for (i = 0;i < MAX_ROW;i++)
        {
            if (!existInDatabase[i])
            {
                insertNode3(i);
                deleteRow(i);
            }
            if (i > rowsCount) break;
        }
        
    }
    else
    {
        insertNode(ContentPath + pPack->R_Begin, 0, info.modify_time, (int)info.type, (Uint32)info.type);

        SDL_EnumerateDirectory(ContentPath, createFolderDatabase, pPack);
    }

    return true;
}

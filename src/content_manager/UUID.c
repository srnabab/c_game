#include "content_manager/content_manager.h"
#include <string.h>
#include <windows.h>
#include <rpcdce.h>

int createNewUUID(unsigned char * uuidStr)
{
    RPC_STATUS status = 0;
    UUID uuid[1];
    RPC_CSTR str = NULL;
    status = UuidCreate(uuid);
    if (status != RPC_S_OK) return -1;

    status = UuidToString(uuid, &str);
    if (status != RPC_S_OK) return -2;

    memcpy(uuidStr, str, 38);

    (void)RpcStringFree(str);

    return 0;
}

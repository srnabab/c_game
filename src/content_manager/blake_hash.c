#include "content_manager/content_manager.h"
#include "blake3.h"

void blake3HashContent(const unsigned char * content, const size_t len, Uint8 * output)
{
    if (content == NULL || len == 0 || output == NULL) return;

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);

    blake3_hasher_update(&hasher, content, len);

    blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);
}

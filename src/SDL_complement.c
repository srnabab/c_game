#include "SDL_complement.h"

size_t SDL_strcspn(const char *str1, const char *str2) 
{
    const char *p1, *p2;
    for (p1 = str1; *p1; ++p1) {
        for (p2 = str2; *p2; ++p2) {
            if (*p1 == *p2) {
                return p1 - str1;
            }
        }
    }
    return p1 - str1;  // 如果没有找到匹配字符，返回字符串长度
}
char *SDL_IOgets(char *__restrict_arr _Buf, int _MaxCount, SDL_IOStream *__restrict_arr _IoStream)
{
    if ((_Buf == NULL) || (_MaxCount <= 0) || (_IoStream == NULL))
    {
        return NULL;
    }

    int i = 0;
    while (i < _MaxCount - 1)
    {
        unsigned char c;
        if (!SDL_ReadU8(_IoStream, &c)) break;

        _Buf[i++] = c;

        if (c == '\n') break;
    }

    return (i > 0) ? _Buf : NULL;
}
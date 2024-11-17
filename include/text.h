#include "std_c.h"
#include <unistr.h>
#include "cglm/cglm.h"

#ifndef TEXT_H
#define TEXT_H

#define MAX_CHARACTERS 100
#define FOUR_POINT 4

bool initTextSystem(void);
void getTextUV(char * text, uint32_t * textLen);

#endif
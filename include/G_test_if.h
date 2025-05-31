#include "G_constants.h"
#include "G_log.h"

#ifndef G_TEST_IF_H
#define G_TEST_IF_H 1

#define printf print
#define test_if(condition) \
    do { \
        _Bool __original_cond_val = (condition); /* 评估原始条件，处理潜在副作用 */ \
        printf("DEBUG_IF: Forcing execution for condition '%s'. Original value was: %s\n", \
               #condition, __original_cond_val ? "true" : "false"); \
        /* 在这里可以添加其他你希望在强制触发时执行的调试代码，例如： */ \
        /* print_program_state(); */ \
        printf("line %d, %s, Forced if\n", __LINE__, __FILE__); \
        /* char debug_buf[100]; */ \
        /* sprintf(debug_buf, "Value of x: %d, y: %d", x, y); */ \
        /* puts(debug_buf); */ \
        BREAK_POINT\
    } while(0); \
    if (1) // 强制 if 条件为真
#undef printf

#endif // G_test_if.h
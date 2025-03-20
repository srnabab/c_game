#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_mutex.h"

#ifndef G_QUEUE_H
#define G_QUEUE_H 1

struct _G_Queue;

typedef bool (*QueueOp)(struct _G_Queue *, void * data);

struct _G_Queue
{
    void * data;
    int32_t capacity;
    int32_t head;
    int32_t tail;
    size_t dataSize;

    SDL_Mutex * mutex;
    QueueOp addTail;
    QueueOp getHead;
    QueueOp addHead;
    QueueOp getTail;
};
typedef struct _G_Queue G_Queue;

extern bool SDLCALL initQueue(G_Queue * queue, size_t dataSize, int32_t capacity, QueueOp addTail, QueueOp getHead, QueueOp addHead, QueueOp getTail);
extern bool SDLCALL G_QueueIsFull(G_Queue queue);
extern bool SDLCALL G_QueueIsEmpty(G_Queue queue);
extern bool SDLCALL G_QueueResize(G_Queue * queue, int newCapacity);
extern void SDLCALL G_deInitQueue(G_Queue * queue);

#endif // G_queue.h

#if QUEUE_TEST
#include "SDL3/SDL_test.h"
#include <stdio.h>

// Struct Test
typedef struct {
    Uint64 a;
    float b;
} MyStruct;

int queueTest(void)
{
    int res, passed, total, failed;
    passed = total = failed = 0;
    SDL_Log("-------------------------------------------------------------");
    SDL_Log("queue test\n");

    G_Queue queue = {};
    // test 1
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");

    for (Uint32 i = 0;i < 1024;i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 0;i < 1024;i++)
    {
        Uint32 b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetHandFunc Uint32 test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("queue addTail getHead Uint32 test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getHead Uint32 test failed");
        if (!failed) failed = passed + 1;
    }

    // Float Test
    // test 2
    total++;
    initQueue(&queue, sizeof(float), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");

    for (float i = 0.0f; i < 1024.0f; i += 1.0f)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull float test"); // Corrected: Use queue
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty float test"); // Corrected: Use queue

    for (float i = 0.0f; i < 1024.0f; i += 1.0f)
    {
        float b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetHandFunc float test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");
    G_deInitQueue(&queue); // Corrected: Use G_deInitQueue

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getHead float test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getHead float test failed");
        if (!failed) failed = passed + 1;
    }


    // Int Test
    // test 3
    total++;
    initQueue(&queue, sizeof(int), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");

    for (int i = 0; i < 1024; i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull int test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty int test");

    for (int i = 0; i < 1024; i++)
    {
        int b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetHandFunc int test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getHead int test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getHead int test failed");
        if (!failed) failed = passed + 1;
    }


    // test 4
    total++;
    initQueue(&queue, sizeof(MyStruct), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");

    for (int i = 0; i < 1024; i++)
    {
        MyStruct temp;
        temp.a = (Uint64)i;
        temp.b = (float)i * 2.0f;
        queue.addTail(&queue, &temp);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull struct test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty struct test");

    for (int i = 0; i < 1024; i++)
    {
        MyStruct b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b.a == (Uint64)i, "defaultGetHandFunc struct test (a)");
        SDLTest_AssertCheck(b.b == (float)i * 2.0f, "defaultGetHandFunc struct test (b)");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getHead struct test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getHead struct test failed");
        if (!failed) failed = passed + 1;
    }


    // test 5
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");

    for (Uint32 i = 0;i < 1024;i++)
    {
        queue.addHead(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 0;i < 1024;i++)
    {
        Uint32 b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetTailFunc Uint32 test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("queue addHead getTail Uint32 test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getTail Uint32 test failed");
        if (!failed) failed = passed + 1;
    }

    // Float Test
    // test 6
    total++;
    initQueue(&queue, sizeof(float), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");

    for (float i = 0.0f; i < 1024.0f; i += 1.0f)
    {
        queue.addHead(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull float test"); // Corrected: Use queue
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty float test"); // Corrected: Use queue

    for (float i = 0.0f; i < 1024.0f; i += 1.0f)
    {
        float b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetTailFunc float test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");
    G_deInitQueue(&queue); // Corrected: Use G_deInitQueue

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addHead getTail float test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getTail float test failed");
        if (!failed) failed = passed + 1;
    }


    // Int Test
    // test 7
    total++;
    initQueue(&queue, sizeof(int), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");

    for (int i = 0; i < 1024; i++)
    {
        queue.addHead(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull int test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty int test");

    for (int i = 0; i < 1024; i++)
    {
        int b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetTailFunc int test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addHead getTail int test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getTail int test failed");
        if (!failed) failed = passed + 1;
    }


    // test 8
    total++;
    initQueue(&queue, sizeof(MyStruct), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");

    for (int i = 0; i < 1024; i++)
    {
        MyStruct temp;
        temp.a = (Uint64)i;
        temp.b = (float)i * 2.0f;
        queue.addHead(&queue, &temp);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull struct test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty struct test");

    for (int i = 0; i < 1024; i++)
    {
        MyStruct b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b.a == (Uint64)i, "defaultGetTailFunc struct test (a)");
        SDLTest_AssertCheck(b.b == (float)i * 2.0f, "defaultGetTailFunc struct test (b)");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addHead getTail struct test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getTail struct test failed");
        if (!failed) failed = passed + 1;
    }

    // test 9
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");

    for (Uint32 i = 0;i < 1024;i++)
    {
        queue.addHead(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 1024;i > 0;i--)
    {
        Uint32 b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i - 1, "defaultgetHeadFunc Uint32 test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("queue addHead getHead Uint32 test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getHead Uint32 test failed");
        if (!failed) failed = passed + 1;
    }

    // Float Test
    // test 10
    total++;
    initQueue(&queue, sizeof(float), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");

    for (float i = 0.0f; i < 1024.0f; i += 1.0f)
    {
        queue.addHead(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull float test"); // Corrected: Use queue
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty float test"); // Corrected: Use queue

    for (float i = 1023.0f; i >= 0.0f; i -= 1.0f)
    {
        float b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultgetHeadFunc float test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");
    G_deInitQueue(&queue); // Corrected: Use G_deInitQueue

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addHead getHead float test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getHead float test failed");
        if (!failed) failed = passed + 1;
    }


    // Int Test
    // test 11
    total++;
    initQueue(&queue, sizeof(int), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");

    for (int i = 0; i < 1024; i++)
    {
        queue.addHead(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull int test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty int test");

    for (int i = 1023;i >= 0;i--)
    {
        int b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultgetHeadFunc int test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addHead getHead int test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getHead int test failed");
        if (!failed) failed = passed + 1;
    }


    // test 12 
    total++;
    initQueue(&queue, sizeof(MyStruct), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");

    for (int i = 0; i < 1024; i++)
    {
        MyStruct temp;
        temp.a = (Uint64)i;
        temp.b = (float)i * 2.0f;
        queue.addHead(&queue, &temp);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull struct test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty struct test");

    for (int i = 1023;i >= 0;i--)
    {
        MyStruct b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b.a == (Uint64)i, "defaultgetHeadFunc struct test (a)");
        SDLTest_AssertCheck(b.b == (float)i * 2.0f, "defaultgetHeadFunc struct test (b)");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addHead getHead struct test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addHead getHead struct test failed");
        if (!failed) failed = passed + 1;
    }

    // test 13
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");

    for (Uint32 i = 0;i < 1024;i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 1024;i > 0;i--)
    {
        Uint32 b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b == i - 1, "defaultgetTailFunc Uint32 test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("queue addTail getTail Uint32 test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getTail Uint32 test failed");
        if (!failed) failed = passed + 1;
    }

    // Float Test
    // test 15
    total++;
    initQueue(&queue, sizeof(float), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");

    for (float i = 0.0f; i < 1024.0f; i += 1.0f)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull float test"); // Corrected: Use queue
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty float test"); // Corrected: Use queue

    for (float i = 1023.0f; i >= 0.0f; i -= 1.0f)
    {
        float b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultgetTailFunc float test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");
    G_deInitQueue(&queue); // Corrected: Use G_deInitQueue

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getTail float test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getTail float test failed");
        if (!failed) failed = passed + 1;
    }


    // Int Test
    // test 16
    total++;
    initQueue(&queue, sizeof(int), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");

    for (int i = 0; i < 1024; i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull int test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty int test");

    for (int i = 1023;i >= 0;i--)
    {
        int b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultgetTailFunc int test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getTail int test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getTail int test failed");
        if (!failed) failed = passed + 1;
    }


    // test 17 
    total++;
    initQueue(&queue, sizeof(MyStruct), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");

    for (int i = 0; i < 1024; i++)
    {
        MyStruct temp;
        temp.a = (Uint64)i;
        temp.b = (float)i * 2.0f;
        queue.addTail(&queue, &temp);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull struct test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty struct test");

    for (int i = 1023;i >= 0;i--)
    {
        MyStruct b;
        queue.getTail(&queue, &b);

        SDLTest_AssertCheck(b.a == (Uint64)i, "defaultgetTailFunc struct test (a)");
        SDLTest_AssertCheck(b.b == (float)i * 2.0f, "defaultgetTailFunc struct test (b)");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getTail struct test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getTail struct test failed");
        if (!failed) failed = passed + 1;
    }
    
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);
    
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");
    
    for (Uint32 i = 0;i < 512;i++)
    {
        queue.addHead(&queue, &i);
        i++;
        queue.addTail(&queue, &i);
        i--;
    }
    
    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");
    
    for (Uint32 i = 512;i > 0;i--)
    {
        Uint32 b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "defaultGetHandFunc Uint32 test");
    }
    for (Uint32 i = 0;i < 512;i++)
    {
        Uint32 b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "defaultGetHandFunc Uint32 test");
    }
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty Uint32 test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull Uint32 test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getTail struct test passed");
        passed++;
    }
    else 
    {
        SDL_Log("queue addTail getTail struct test failed");
        if (!failed) failed = passed + 1;
    }    
    
    // test 18
    // Float Test
    total++;
    initQueue(&queue, sizeof(float), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");

    for (float i = 0.0f; i < 512.0f; i++)
    {
        queue.addHead(&queue, &i);
        i += 1.0f;
        queue.addTail(&queue, &i);
        i -= 1.0f;
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull float test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty float test");

    for (float i = 512.0f; i > 0.0f; i--)
    {
        float b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "defaultGetHandFunc float test");
    }
    for (float i = 0.0f; i < 512.0f; i++)
    {
        float b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "defaultGetHandFunc float test");
    }
    
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty float test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull float test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getTail float test passed");
        passed++;
    }
    else
    {
        SDL_Log("queue addTail getTail float test failed");
        if (!failed) failed = passed + 1;
    }

    // test 19
    // Int Test
    total++;
    initQueue(&queue, sizeof(int), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");

    for (int i = 0; i < 512; i++)
    {
        queue.addHead(&queue, &i);
        i++;
        queue.addTail(&queue, &i);
        i--;
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull int test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty int test");

    for (int i = 512; i > 0; i--)
    {
        int b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "defaultGetHandFunc int test");
    }
    for (int i = 0; i < 512; i++)
    {
        int b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "defaultGetHandFunc int test");
    }
    
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty int test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull int test");
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getTail int test passed");
        passed++;
    }
    else
    {
        SDL_Log("queue addTail getTail int test failed");
        if (!failed) failed = passed + 1;
    }

    // test 20
    total++;
    initQueue(&queue, sizeof(MyStruct), 1024, NULL, NULL, NULL, NULL);

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");

    for (int i = 0; i < 512; i++)
    {
        MyStruct ts1 = { (Uint64)i, (float)i };
        queue.addHead(&queue, &ts1);
        MyStruct ts2 = { (Uint64)i + 1, (float)i + 1.0f };
        queue.addTail(&queue, &ts2);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull struct test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty struct test");
    
    for (int i = 511; i >= 0; i--)
    {
        MyStruct b;
        queue.getTail(&queue, &b);
      SDLTest_AssertCheck(b.a == (Uint64)(i+1) && b.b == (float)(i + 1.0f), "defaultGetHandFunc struct test");
    }

     for (int i = 0; i < 512; i++)
    {
        MyStruct b;
        queue.getTail(&queue, &b);
       SDLTest_AssertCheck(b.a == (Uint64)i && b.b == (float)i, "defaultGetHandFunc struct test");
    }

    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    G_deInitQueue(&queue);


    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue addTail getTail struct test passed");
        passed++;
    }
    else
    {
        SDL_Log("queue addTail getTail struct test failed");
        if (!failed) failed = passed + 1;
    }

    SDLTest_LogAssertSummary();
    if (res == TEST_RESULT_PASSED) SDL_Log("\nqueue addTail getHead test passed(%d / %d)", passed, total);
    else SDL_Log("\nqueue addTail getHead test failed(%d / %d), first failed: %d", passed, total, failed);

    SDL_Log("-------------------------------------------------------------\n");

    return res;
}
int queueReiszeTest(void)
{
    int res, passed, total, failed;
    passed = total = failed = 0;
    SDL_Log("-------------------------------------------------------------");
    SDL_Log("queue test\n");

    G_Queue queue = {};
    // test 1 tail < head 0-tail len >= head-max len full
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    for (Uint32 i = 0;i < 512;i++)
    {
        queue.addTail(&queue, &i);
        i++;
        queue.addHead(&queue, &i);
        i--;
    }

    // for (Uint32 i = 0;i < 1024;i++)
    // {
    //     printf("i: %5u ", ((Uint32*)queue.data)[i]);
    // }
    // puts("\n");

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull struct test");

    G_QueueResize(&queue, 2048);
    // for (Uint32 i = 0;i < 1024;i++)
    // {
    //     printf("i: %5u ", ((Uint32*)queue.data)[i]);
    // }
    // puts("\n");

    for (Uint32 i = 512;i > 0;i--)
    {
        Uint32 b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i - 1, "queue resize Uint32 test");
    }
    for (Uint32 i = 1;i <= 512;i++)
    {
        Uint32 b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "queue resize Uint32 test");
    }
    
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    

    for (Uint32 i = 0;i < 2048;i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 0;i < 2048;i++)
    {
        Uint32 b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetHandFunc Uint32 test");
    }
    
    G_deInitQueue(&queue);
    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue resize tail < head 0-tail len >= head-max len full passed");
        passed++;
    }
    else
    {
        SDL_Log("queue resize tail < head 0-tail len >= head-max len full failed");
        if (!failed) failed = passed + 1;
    }

    // test 2 tail < head 0-tail len < head - max len full
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    for (Uint32 i = 0;i < 200;i++)
    {
        queue.addTail(&queue, &i);
    }

    for (Uint32 i = 0;i < 824;i++)
    {
        queue.addHead(&queue, &i);
    }
    // for (Uint32 i = 0;i < 1024;i++)
    // {
    //     printf("i: %5u ", ((Uint32*)queue.data)[i]);
    // }
    // puts("\n");

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull struct test");

    G_QueueResize(&queue, 2048);
    // for (Uint32 i = 0;i < 1024;i++)
    // {
    //     printf("i: %5u ", ((Uint32*)queue.data)[i]);
    // }
    // puts("\n");

    for (Uint32 i = 200;i > 0;i--)
    {
        Uint32 b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i - 1, "queue resize Uint32 test");
    }
    for (Uint32 i = 0;i < 824;i++)
    {
        Uint32 b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == i, "queue resize Uint32 test");
    }
    
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    

    for (Uint32 i = 0;i < 2048;i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 0;i < 2048;i++)
    {
        Uint32 b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetHandFunc Uint32 test");
    }
    
    G_deInitQueue(&queue);


    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue resize tail < head 0-tail len < head - max len full passed");
        passed++;
    }
    else
    {
        SDL_Log("queue resize tail < head 0-tail len < head - max len full test failed");
        if (!failed) failed = passed + 1;
    }
    
    // test 3 tail > head not full
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    for (Uint32 i = 0;i < 800;i++)
    {
        queue.addTail(&queue, &i);
    }
    {
        Uint32 c;
        queue.getHead(&queue, &c);
    }

    // for (Uint32 i = 1;i < 800;i++)
    // {
        // printf("i:%u : %5u ", i, ((Uint32*)queue.data)[i]);
    // }
    // puts("\n");

    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");

    G_QueueResize(&queue, 2048);

    // for (Uint32 i = 0;i < 799;i++)
    // {
        // printf("i:%u: %5u ", i, ((Uint32*)queue.data)[i]);
    // }
    // puts("\n");

    for (Uint32 i = 1;i < 800;i++)
    {
        Uint32 b;
        queue.getHead(&queue, &b);
        SDLTest_AssertCheck(b == i, "queue resize Uint32 test");
    }
    
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    

    for (Uint32 i = 0;i < 2048;i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 0;i < 2048;i++)
    {
        Uint32 b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetHandFunc Uint32 test");
    }
    
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue resize tail > head no full test passed");
        passed++;
    }
    else
    {
        SDL_Log("queue resize tail > head no full test failed");
        if (!failed) failed = passed + 1;
    }

    // test 4 single
    total++;
    initQueue(&queue, sizeof(Uint32), 1024, NULL, NULL, NULL, NULL);

    for (Uint32 i = 0;i < 800;i++)
    {
        queue.addTail(&queue, &i);
    }
    for (Uint32 i = 0;i < 799;i++)
    {
        Uint32 c;
        queue.getHead(&queue, &c);
    }

    // {
    //     printf("i: %5u ", ((Uint32*)queue.data)[799]);
    // }
    // puts("\n");

    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");

    G_QueueResize(&queue, 2048);

    // {
    //     printf("i: %5u ", ((Uint32*)queue.data)[0]);
    // }
    // puts("\n");

    {
        Uint32 b;
        queue.getTail(&queue, &b);
        SDLTest_AssertCheck(b == 799, "queue resize Uint32 test");
    }
    
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == true, "G_QueueIsEmpty struct test");
    SDLTest_AssertCheck(G_QueueIsFull(queue) == false, "G_QueueIsFull struct test");
    

    for (Uint32 i = 0;i < 2048;i++)
    {
        queue.addTail(&queue, &i);
    }

    SDLTest_AssertCheck(G_QueueIsFull(queue) == true, "G_QueueIsFull Uint32 test");
    SDLTest_AssertCheck(G_QueueIsEmpty(queue) == false, "G_QueueIsEmpty Uint32 test");

    for (Uint32 i = 0;i < 2048;i++)
    {
        Uint32 b;
        queue.getHead(&queue, &b);

        SDLTest_AssertCheck(b == i, "defaultGetHandFunc Uint32 test");
    }
    
    G_deInitQueue(&queue);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED)
    {
        SDL_Log("queue resize single test passed");
        passed++;
    }
    else
    {
        SDL_Log("queue resize single test failed");
        if (!failed) failed = passed + 1;
    }
    SDLTest_LogAssertSummary();
    if (res == TEST_RESULT_PASSED) SDL_Log("\nqueue addTail getHead test passed(%d / %d)", passed, total);
    else SDL_Log("\nqueue addTail getHead test failed(%d / %d), first failed: %d", passed, total, failed);

    SDL_Log("-------------------------------------------------------------\n");

    return  res;
}
#endif
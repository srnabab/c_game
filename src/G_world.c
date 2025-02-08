#include "box2d/box2d.h"

#include "vk_code_h/vk_move.h"

#include "G_world.h"
#include "G_struct.h"
#include "G_stack.h"

static b2WorldDef worldDef = {};
static b2WorldId worldId = {};

/*middle, left, right*/
static b2BodyDef groundBodyDef[4];
static b2BodyId groundId[4];
static b2Polygon groundBox[4];
static b2ShapeDef groundShapeDef[4];
static b2ShapeId groundShapeId[4];

static SDL_Thread * worldThread;
static SDL_Semaphore * worldSemaphore;
ThreadSem world;

EmptyStack ballStack;

extern float physicalCoffectX;
extern float physicalCoffectY;
extern VK_ALL allInOne;

static void * box2d_SDL_Alloc(unsigned int size, int alignment)
{
    return SDL_aligned_alloc(alignment, size);
}
static void box2d_SDL_Free(void * memory)
{
    SDL_aligned_free(memory);
}
void initWorld(void)
{
    b2SetAllocator(box2d_SDL_Alloc, box2d_SDL_Free);

    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -100.0f * SCALE_FACTOR};
    worldDef.restitutionThreshold = 0.5f;

    worldId = b2CreateWorld(&worldDef);
    
    groundBodyDef[0] = b2DefaultBodyDef();
    groundBodyDef[0].type = b2_staticBody;
    groundBodyDef[0].position = (b2Vec2){0.0f, -300.0f * SCALE_FACTOR};

    groundId[0] = b2CreateBody(worldId, &groundBodyDef[0]);

    groundBox[0] = b2MakeBox(2000.0f * SCALE_FACTOR, 10.0f * SCALE_FACTOR);

    groundShapeDef[0] = b2DefaultShapeDef();
    groundShapeId[0] = b2CreatePolygonShape(groundId[0], &groundShapeDef[0], &groundBox[0]);

    groundBodyDef[1] = b2DefaultBodyDef();
    groundBodyDef[1].type = b2_staticBody;
    groundBodyDef[1].position = (b2Vec2){-300.0f * SCALE_FACTOR, -150.0f * SCALE_FACTOR};

    groundId[1] = b2CreateBody(worldId, &groundBodyDef[1]);

    groundBox[1] = b2MakeBox(10.0f * SCALE_FACTOR, 240.0f * SCALE_FACTOR);

    groundShapeDef[1] = b2DefaultShapeDef();
    groundShapeId[1] = b2CreatePolygonShape(groundId[1], &groundShapeDef[1], &groundBox[1]);

    groundBodyDef[2] = b2DefaultBodyDef();
    groundBodyDef[2].type = b2_staticBody;
    groundBodyDef[2].position = (b2Vec2){300.0f * SCALE_FACTOR, -150.0f * SCALE_FACTOR};

    groundId[2] = b2CreateBody(worldId, &groundBodyDef[2]);

    groundBox[2] = b2MakeBox(10.0f * SCALE_FACTOR, 240.0f * SCALE_FACTOR);

    groundShapeDef[2] = b2DefaultShapeDef();
    groundShapeId[2] = b2CreatePolygonShape(groundId[2], &groundShapeDef[2], &groundBox[2]);

    /*groundBodyDef[3] = b2DefaultBodyDef();
    groundBodyDef[3].type = b2_staticBody;
    groundBodyDef[3].position = (b2Vec2){0.0f, 80.0f};

    groundId[3] = b2CreateBody(worldId, &groundBodyDef[3]);

    groundBox[3] = b2MakeBox(2000.0f, 10.0f);

    groundShapeDef[3] = b2DefaultShapeDef();
    groundShapeId[3] = b2CreatePolygonShape(groundId[3], &groundShapeDef[3], &groundBox[3]);*/
    initStack(&ballStack, i32, NULL, NULL, NULL);

    worldSemaphore = SDL_CreateSemaphore(0);
    worldThread = SDL_CreateThread(stepWorld, "physical", NULL);

    world.thread = worldThread;
    world.semaphore = worldSemaphore;
}
static b2BodyDef * bodyDefs = NULL;
static b2BodyId * bodyIds = NULL;

static b2Circle * dynamicBoxs = NULL;

static b2ShapeDef * shapeDefs = NULL;

static b2ShapeId * shapeIds = NULL;

static uint32_t boxCount = 1;

void createCircle(float x, float y)
{
    boxCount++;
    bodyDefs = (b2BodyDef *)SDL_realloc(bodyDefs, boxCount * sizeof(b2BodyDef));
    int index = boxCount - 1;
    bodyDefs[index] = b2DefaultBodyDef();
    bodyDefs[index].type = b2_dynamicBody;
    bodyDefs[index].position = (b2Vec2){x * SCALE_FACTOR, y * SCALE_FACTOR};

    bodyIds = (b2BodyId *)SDL_realloc(bodyIds, boxCount * sizeof(b2BodyId));
    bodyIds[index] = b2CreateBody(worldId, &bodyDefs[index]);

    dynamicBoxs = (b2Circle *)SDL_realloc(dynamicBoxs, boxCount * sizeof(b2Circle));
    dynamicBoxs[index].center = (b2Vec2){0.0f, 0.0f};
    dynamicBoxs[index].radius = 7.7f * SCALE_FACTOR;

    shapeDefs = (b2ShapeDef *)SDL_realloc(shapeDefs, boxCount * sizeof(b2ShapeDef));
    shapeDefs[index] = b2DefaultShapeDef();
    shapeDefs[index].density = 1.0f;
    shapeDefs[index].friction = 0.3f;

    shapeIds = (b2ShapeId *)SDL_realloc(shapeIds, boxCount * sizeof(b2ShapeId));
    shapeIds[index] = b2CreateCircleShape(bodyIds[index], &shapeDefs[index], &dynamicBoxs[index]);
}
static bool stepDone = true;
void updateCircle(void)
{
    if (stepDone)
    {
        // float averagePhysicalCoffect = (physicalCoffectX + physicalCoffectY) / 2.0f;
        while (ballStack.top != -1)
        {
            int32_t temp = *(int32_t*)(ballStack.popFn(&ballStack));
            createCircle(temp + 8, 288);
        }
        for (uint32_t i = 1;i < boxCount;i++)
        {
            b2Vec2 position = b2Body_GetPosition(bodyIds[i]);
            updatePosition((position.x * SCALE_FACTOR_INV - 24.0f) * physicalCoffectY, (position.y * SCALE_FACTOR_INV - 24.0f) * physicalCoffectY, allInOne.ppVertices_Pos, i);
        }
        stepDone = false;
        SDL_SignalSemaphore(worldSemaphore);
    }
}
extern bool game_is_running;
int stepWorld(void * arg)
{
    while (game_is_running)
    {
        SDL_WaitSemaphore(worldSemaphore);
        // if (boxCount > 2)
        // {
        //     b2Body_SetTransform(bodyIds[1], (b2Vec2){-700.0f * SCALE_FACTOR, -300.0f * SCALE_FACTOR}, b2Rot_identity);
        // }
        b2World_Step(worldId, TIME_STEP, SUB_STEP_COUNT);
        stepDone = true;
    }
    return 0;
}
uint32_t getBoxCount(void)
{
    return boxCount;
}
void cleanWorld(void)
{
    SDL_SignalSemaphore(worldSemaphore);
    deInitStack(&ballStack);
    SDL_WaitThread(worldThread, NULL);
    SDL_DestroySemaphore(worldSemaphore);
    b2DestroyWorld(worldId);
}
void destroyFloor(void)
{
    b2DestroyShape(groundShapeId[3]);
    b2DestroyBody(groundId[3]);
}
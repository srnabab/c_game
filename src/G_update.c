#include "G_game.h"
#include "G_log.h"
#include "G_struct.h"
#include "G_entity.h"
#include "G_text.h"
#include "G_world.h"
#include "G_timer.h"
#include "G_map.h"
#include "G_text.h"
#include "G_scene.h"
#include "G_music.h"
#include "G_custom_math.h"
#include "G_TileMap/G_TileSet.h"

#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_move.h"

#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_timer.h"
#include "SDL3_mixer/SDL_mixer.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;
extern bool update_done;
extern bool game_is_running;

extern bool keys[SDL_SCANCODE_COUNT];
extern bool preKeys[SDL_SCANCODE_COUNT];
extern bool repeatKeys[SDL_SCANCODE_COUNT];

extern G_Stack ballStack;
extern vec2 UVs[MAX_CHARACTERS][FOUR_POINT];

static G_Entity mPoint = {};
static G_Entity camera = {};

extern Scene preScene;
extern Scene scene;

extern float mouse_x;
extern float mouse_y;

extern bool resolutionChanged; 
extern bool resolutionChanged2; 

extern uint8_t leftButtonClickedTimes;
extern bool leftButtonEnabled;
extern Uint32 ballCount;

static Uint32 textLine = 0;
static bool textDisplay = false;

static void processKeys(void)
{
    if (keys[SDL_SCANCODE_ESCAPE])
    {
        Mix_HaltMusic();
        SDL_SignalSemaphore(allSync.updateSemaphore);
        SDL_SignalSemaphore(allSync.renderSemaphore);
        SDL_SignalSemaphore(allSync.signalSemaphore);
        SDL_SignalSemaphore(allSync.signalSemaphore);
        SDL_SignalSemaphore(allSync.signalSemaphore);
        game_is_running = false;
    }

    if (!keys[SDL_SCANCODE_T] && preKeys[SDL_SCANCODE_T])
    {
        preKeys[SDL_SCANCODE_T] = false;

        textLine++;
        textDisplay = true;
        print("textline: %u", textLine);
    }

    if (keys[SDL_SCANCODE_W])
    {
        mPoint.direction[0] = true;
    }
    else
    {
        mPoint.direction[0] = false;
    }

    if (keys[SDL_SCANCODE_S])
    {
        mPoint.direction[1] = true;
    }
    else
    {
        mPoint.direction[1] = false;
    }

    if (keys[SDL_SCANCODE_A])
    {
        mPoint.direction[2] = true;
    }
    else
    {
        mPoint.direction[2] = false;
    }

    if (keys[SDL_SCANCODE_D])
    {
        mPoint.direction[3] = true;
    }
    else
    {
        mPoint.direction[3] = false;
    }

    if (keys[SDL_SCANCODE_UP])
    {
        camera.direction[0] = true;
    }
    else
    {
        camera.direction[0] = false;
    }

    if (keys[SDL_SCANCODE_DOWN])
    {
        camera.direction[1] = true;
    }
    else
    {
        camera.direction[1] = false;
    }

    if (keys[SDL_SCANCODE_LEFT])
    {
        camera.direction[2] = true;
    }
    else
    {
        camera.direction[2] = false;
    }

    if (keys[SDL_SCANCODE_RIGHT])
    {
        camera.direction[3] = true;
    }
    else
    {
        camera.direction[3] = false;
    }
}

// Update function with a fixed time step
int update(void * arg)
{
    Uint32 update_frame = 0;
    UniformBufferObject * pGraphicUbo = allInOne.pGraphicUbo;
    UniformBufferObject * pGraphic3DUbo = allInOne.pGraphic3DUbo;
    UniformBufferObject * pUIUbo = allInOne.pUIUbo;
    float * pCamera_X = allInOne.pCamera_X;
    float * pCamera_Y = allInOne.pCamera_Y;

    initEntity(&mPoint, 0.0f, 0.0f, 87.0f);
    initEntity(&camera, 0.0f, 0.0f, 1.6f);
    
    bool playedMusic = false;

    Uint64 frequency = SDL_GetPerformanceFrequency();;
    Uint64 delta_time_ns = 0;
    float delta_time = 0.0f;
    Uint64 totalTimeNs = 0;
    float totalTime = 0.0f;
    
    bool recovreyPause = false;
    bool sceneCleaned = false;

    G_Point_Int tileCenter = {0, 0};
    ShapeConstants shapePushConstants = {};
    shapePushConstants.pos[0] = 0.0f;
    shapePushConstants.pos[1] = 0.0f;
    shapePushConstants.scale[0] = 0.053333333f;
    shapePushConstants.scale[1] = 0.053333333f;



    Uint32 currentFrame;

    Uint32 vertexStart = 0;
    Uint32 vertexEnd = allInOne.vertices2DCount;

    Uint32 rowCount = 0;
    Uint32 colCount = 0;

    int firstBottom_X = 0;
    int firstBottom_Y = 0;

    int baseX = 0;
    int baseY = 0;

    int32_t groupID = -1;

    textureVertexInit(-32, -32, 64, 64, 0.2f, &allInOne.vertices2DCount, allInOne.pVertices2D, getTexture(TEXTURE_LOADING));
    
    // tileMapVertexInit(&allInOne.vertices2DCount, allInOne.pVertices2D);
    addModelMatrix(0, 0, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(100, 0, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(0, 100, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);
    addModelMatrix(100, 100, 8, allInOne.pStaticModelPool, TEXTURE_MODEL);

    setMapBottom(allInOne.extent2D.width, allInOne.extent2D.height, 0, 0, &rowCount, &colCount, &firstBottom_X, &firstBottom_Y, &baseX, &baseY, &groupID);
    memcpy(allInOne.pTimeMapTexCoordBufferMapped[0], allInOne.pTileMapUVs, sizeof(vec2) * VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP * (allInOne.bottomImageDrawStack.top + 1));
    memcpy(allInOne.pTimeMapTexCoordBufferMapped[1], allInOne.pTileMapUVs, sizeof(vec2) * VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP * (allInOne.bottomImageDrawStack.top + 1));
    // addModelMatrix(0, 100 / HEIGHT_FACTOR, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM);
    // addModelMatrix(-800, 100 / HEIGHT_FACTOR, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM);
    // addModelMatrix(0, 900 / HEIGHT_FACTOR, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM);

    glm_mat4_identity(allInOne.pTilemapUbo->model);
    glm_lookat((vec3){0.0f, 0.0f, 100.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, allInOne.pTilemapUbo->view);
    glm_ortho_vulkan(-800.0f / 800.0f, 800.0f / 800.0f, -800.0f / 800.0f, 800.0f / 800.0f, -0.001f, -100.0f, allInOne.pTilemapUbo->proj);
    memcpy(allInOne.ppTilemapUniformBufferMapped[0], allInOne.pTilemapUbo, sizeof(UniformBufferObject));
    memcpy(allInOne.ppTilemapUniformBufferMapped[1], allInOne.pTilemapUbo, sizeof(UniformBufferObject));

    SDL_Delay(300);
    
    Uint64 last_frame_time = SDL_GetPerformanceCounter();
    SDL_SignalSemaphore(allSync.updateSemaphore);
    SDL_SignalSemaphore(allSync.renderSemaphore);

    print("update init\n");
    while (game_is_running)
    {
        SDL_WaitSemaphore(allSync.updateSemaphore);
        if (preScene == Pause_Scene && recovreyPause)
        {
            while(SDL_TryWaitSemaphore(allSync.vertexSemaphore));

            last_frame_time = SDL_GetPerformanceCounter();
            recovreyPause = false;
        }

        Uint64 tempTime = SDL_GetPerformanceCounter();
        delta_time_ns = ((tempTime - last_frame_time) * 1000000000ULL) / frequency;
        last_frame_time = tempTime;
        totalTimeNs += delta_time_ns;
        delta_time = delta_time_ns / ((float)S_TO_NS);
        totalTime = totalTimeNs / ((float)S_TO_NS);

        currentFrame = allInOne.currentFrame;

        processKeys();
        
        // particle 
        allInOne.pComputeUbo->deltaTime = delta_time;

        memcpy(allInOne.ppComputeUniformBufferMapped[currentFrame], allInOne.pComputeUbo, sizeof(ComputeUniformBufferObject));
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        float aspect2 = 1.0f  * ((float)allInOne.extent2D.height / 600.0f);
        float aspect = ((float)allInOne.extent2D.width / allInOne.extent2D.height) * aspect2;

        float x, y, z;
        float factor_x = LIGHT_HEIGHT / (allInOne.extent2D.width / 2);
        float factor_y = LIGHT_HEIGHT / (allInOne.extent2D.height / 2);
        
        // shadow map
        SDL_LockMutex(allSync.inputMutex);
        x = mouse_x - (allInOne.extent2D.width / 2);
        y = -mouse_y + (allInOne.extent2D.height / 2);
        // y = 0;
        SDL_UnlockMutex(allSync.inputMutex);
        x *= factor_x;
        y *= factor_y;
        z = SDL_sqrtf(SDL_powf(LIGHT_HEIGHT, 2) - SDL_powf(x, 2) - SDL_powf(y, 2));

        // print("x: %f, y: %f, z: %f", x, y, z);

        mat4 lightProj;
        glm_ortho_vulkan(-(SHADOW_SIZE / 600.0f) / 2.0f, (SHADOW_SIZE / 600.0f) / 2.0f, -(SHADOW_SIZE / 800.0f) / 2.0f, (SHADOW_SIZE / 800.0f) / 2.0f, -0.001f, -100.0f, lightProj);

        glm_lookat((vec3){x, y, z}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, allInOne.pLightSpaceUbo->lightSpace);
        glm_mul(lightProj, allInOne.pLightSpaceUbo->lightSpace, allInOne.pLightSpaceUbo->lightSpace);


        memcpy(allInOne.ppLightSpaceUniformBufferMapped[currentFrame], allInOne.pLightSpaceUbo , sizeof(LightSpace));
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        // 3d object
        glm_mat4_identity(pGraphic3DUbo->model);
        glm_lookat((vec3){*pCamera_X * aspect, 0.0f + *pCamera_Y * aspect2, 10.0f}, (vec3){*pCamera_X * aspect, *pCamera_Y * aspect2, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pGraphic3DUbo->view);
        glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, -0.001f, -100.0f, pGraphic3DUbo->proj);
        // glm_perspective(glm_rad(45.0f), aspect, 0.1f, 100.0f, pGraphic3DUbo->proj);
        // pGraphic3DUbo->proj[1][1] *= -1;

        glm_vec3_copy((vec3){-x, -y, -z}, allInOne.pSunubo->lightDirection);
        glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, allInOne.pSunubo->lightColor);
        allInOne.pSunubo->lightIntensity = 1.1f;

        glm_mat4_copy(allInOne.pLightSpaceUbo->lightSpace, allInOne.pSunubo->lightSpace);

        memcpy(allInOne.ppSunUniformBufferMapped[currentFrame], allInOne.pSunubo, sizeof(DirectionLight));
        memcpy(allInOne.ppGraphic3DUniformBufferMapped[currentFrame], pGraphic3DUbo, sizeof(UniformBufferObject));
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        // SSGI
        glm_mat4_copy(allInOne.pGraphic3DUbo->proj, allInOne.pSSGIubo->projectionMatrix);
        glm_mat4_copy(allInOne.pGraphic3DUbo->proj, allInOne.pSSGIubo->inverseProjectionMatrix);
        glm_mat4_inv(allInOne.pSSGIubo->inverseProjectionMatrix, allInOne.pSSGIubo->inverseProjectionMatrix);

        allInOne.pSSGIubo->cameraPosition[0] = *pCamera_X;
        allInOne.pSSGIubo->cameraPosition[1] = 0.0f + *pCamera_Y;
        allInOne.pSSGIubo->cameraPosition[2] = 10.0f;

        allInOne.pSSGIubo->rayStepSize = 0.05f;
        allInOne.pSSGIubo->maxRaySteps = 64;
        allInOne.pSSGIubo->ssgiStrength = 0.1f;

        memcpy(allInOne.ppSSGIUniformBufferMapped[currentFrame], allInOne.pSSGIubo, sizeof(SSGIUniformBufferObject));
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        // UI object
        glm_mat4_identity(pGraphicUbo->model);
        glm_lookat((vec3){*pCamera_X, *pCamera_Y, 100.0f}, (vec3){*pCamera_X, *pCamera_Y, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pGraphicUbo->view);
        glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, -0.001f, -100.0f, pGraphicUbo->proj);
        // glm_ortho(-aspect, aspect, -1.0f, 1.0f, 0.001f, 100.0f, pGraphicUbo->proj);
        // pGraphicUbo->proj[1][1] *= -1;

        glm_mat4_identity(pUIUbo->model);
        glm_lookat((vec3){0.0f, 0.0f, 100.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pUIUbo->view);
        glm_ortho_vulkan(-aspect, aspect, -aspect2, aspect2, -0.001f, -100.0f, pUIUbo->proj);

        memcpy(allInOne.pShapeConstants, &shapePushConstants, sizeof(ShapeConstants));
        memcpy(allInOne.ppGraphicUniformBufferMapped[currentFrame], pGraphicUbo, sizeof(UniformBufferObject));
        memcpy(allInOne.ppUIUniformBufferMapped[currentFrame], pUIUbo, sizeof(UniformBufferObject));
        SDL_SignalSemaphore(allSync.vertexSemaphore);

        if (scene == Pause_Scene)
        {
            recovreyPause = true;
        }
        else
        {
            static int id_click = 0;
            if (leftButtonClickedTimes)
            {
                leftButtonEnabled = false;
                if (intervalIsDone(f32_ms_to_ns(58.8), &id_click, 1))
                {
                    SDL_LockMutex(allSync.updateMutex);

                    leftButtonClickedTimes = 0;
                    leftButtonEnabled = true;
                    id_click = 0;

                    SDL_UnlockMutex(allSync.updateMutex);
                }
            }

            if (resolutionChanged2)
            {
                rowCount = colCount = 0;
                setMapBottom(allInOne.extent2D.width, allInOne.extent2D.height, *pCamera_X * (allInOne.extent2D.width / 2), *pCamera_Y * (allInOne.extent2D.height / 2), &rowCount, &colCount, &firstBottom_X, &firstBottom_Y, &baseX, &baseY, &groupID);
                resolutionChanged2 = false;
            }

            EntityMove(&camera, delta_time);
            if (camera.direction[0] || camera.direction[1] || camera.direction[2] || camera.direction[3])
            {
                *pCamera_X = camera.position[0];
                *pCamera_Y = camera.position[1];

                setMapBottom(allInOne.extent2D.width, allInOne.extent2D.height, *pCamera_X * (allInOne.extent2D.width / 2), *pCamera_Y * (allInOne.extent2D.height / 2), &rowCount, &colCount, &firstBottom_X, &firstBottom_Y, &baseX, &baseY, &groupID);
                SDL_LockMutex(allSync.updateMutex);
                memcpy(allInOne.pTimeMapTexCoordBufferMapped[currentFrame], allInOne.pTileMapUVs, sizeof(vec2) * VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP * (allInOne.bottomImageDrawStack.top + 1));
                SDL_UnlockMutex(allSync.updateMutex);
            }

            if (scene == First_Scene)
            {
                if (!Mix_PlayingMusic() && !playedMusic)
                {
                    playMusic("test");
                    Mix_VolumeMusic(0);
                    playedMusic = true;
                }

                accumlateTime(delta_time_ns);
                // print("TotalTime: %fms", totalTime);

                if (textDisplay)
                {
                    Uint32 textLen = 0;
                    if (textLine == 2) getTextUV("一二三", &textLen);
                    else if (textLine == 1) getTextUV("哈哈哈哈哈哈哈哈哈", &textLen);

                    if (textLine < 3)
                    {
                        getTexture(TEXTURE_FONT)->refCount = 0;
                        for (Uint32 i = 0;i < textLen;i++)
                        {
                            textureVertexInit_SetUV(-300.0 + (float)i * FONT_SIZE, -100.0, FONT_SIZE, FONT_SIZE, 0.1f, &allInOne.vertices2DCount, allInOne.pVertices2D, UVs[i], getTexture(TEXTURE_FONT));
                        }
                    }
                    textDisplay = false;
                }

                // static int id_test = 0;
                // int test_a = -1;
                // addTimerFunc(u32_s_to_ns(1), &id_test, 10, test, &test_a);

                EntityMove(&mPoint, delta_time);
                tileCenter = locatePoint(&mPoint, rowCount, colCount, firstBottom_X, firstBottom_Y, groupID);
                shapePushConstants.pos[0] = (float)tileCenter.x / (allInOne.extent2D.width / 2);
                shapePushConstants.pos[1] = (float)tileCenter.y / (allInOne.extent2D.height / 2);
                shapePushConstants.scale[0] = (float)18 / (allInOne.extent2D.width / 2);
                shapePushConstants.scale[1] = (float)18 / (allInOne.extent2D.height / 2);

                // print("mPoint: (%d, %d)", (int32_t)mPoint.position.x, (int32_t)mPoint.position.y);
                // SDL_LockMutex(sdl_mutex_2);
                // if (pictureMove[0])
                // {
                //     *allInOne.pPictureY += 200 * delta_time;
                //     //print("y: %f, enabled: %d, delta time: %lf, last_frame_time: %lu ----%s", *allInOne.pPictureY, pictureMove[0], delta_time, last_frame_time, timeNow);
                // }
                // if (pictureMove[1])
                // {
                //     *allInOne.pPictureY -= 200 * delta_time;
                // }
                // if (pictureMove[2])
                // {
                //     *allInOne.pPictureX -= 200 * delta_time;
                // }
                // if (pictureMove[3])
                // {
                //     *allInOne.pPictureX += 200 * delta_time;
                // }
                // if (scale)
                // {
                //     glm_scale_self(allInOne.ppVertices2D, 2.0f, 1);
                // }
                // if (pictureMove[0] | pictureMove[1] | pictureMove[2] | pictureMove[3] | scale)
                // {
                //     updatePosition(*allInOne.pPictureX, *allInOne.pPictureY, allInOne.pExtent2D, allInOne.ppVertices2D, 1);
                //     scale = false;
                // }
                // SDL_UnlockMutex(sdl_mutex_2);

                // size_t bufferSize = sizeof(Vertex332) * count;

                // Uint32 indiceCount = *allInOne.pIndicesCount;
                // size_t bufferSize2 = sizeof(uint16_t) * indiceCount;

                // if (ballAdd)
                // {
                //     //allInOne.pVertices2D = (Vertex332 *)realloc(allInOne.pVertices2D, count * sizeof(Vertex332));
                //     int x = SDL_rand(250);
                //     if (SDL_rand(2))
                //     {
                //         x *= -1;
                //     }
                //     // float averagePhysicalCoffect = (physicalCoffectX + physicalCoffectY) / 2.0f;
                //     textureVertexInit(x * physicalCoffectX, 280 * physicalCoffectY, 16 * physicalCoffectY, 16 * physicalCoffectY, 0.9, allInOne.vertices2DCount, allInOne.pVertices2D, getTexture(TEXTURE_CIRCLE));

                //     ballStack.pushFn(&ballStack, &x);
                //     //print("indices count: %u\n", indiceCount);
                //     //*allInOne.ppIndices = (uint16_t *)realloc(*allInOne.ppIndices, indiceCount * sizeof(uint16_t));

                //     ballAdd = false;
                // }
            
                static int id_timeStep = 0;
                while (intervalIsDone(f32_s_to_ns(TIME_STEP), &id_timeStep, -1))
                {
                    updateCircle(); 
                    // accumulator -= timeStep;
                }
            }

            vertexEnd = allInOne.vertices2DCount;
            //print("time: %.2f\n", time);
            SDL_LockMutex(allSync.updateMutex);

            memcpy(allInOne.pVertexBuffer2DMemMapped[currentFrame], allInOne.pVertices2D, vertexEnd * sizeof(Vertex332));// update vertex buffer
            memcpy(allInOne.pVertexBuffer3DMemMapped[currentFrame], allInOne.pVertices3D, 30000 * sizeof(Vertex332));
            memcpy(allInOne.pIndexBuffer3DMemMapped[currentFrame], allInOne.pIndices3D, 45000 * sizeof(Uint32));
            // SDL_SignalSemaphore(allSync.vertexSemaphore);

            allInOne.pPushConstants->rotation = totalTime * glm_rad(580.0f);
            
            SDL_UnlockMutex(allSync.updateMutex);

            //print("test: %lf", testNum);
            //print("delta time:%f", delta_time);
            update_frame++;
            
            update_done = true;
        }

        SDL_SignalSemaphore(allSync.signalSemaphore);
    }
    return 0;
}
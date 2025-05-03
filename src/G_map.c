#include "G_constants.h"
#include "G_staticModel.h"
#include "G_map.h"
#include "G_threadPool.h"
#include "G_log.h"
#include "G_struct.h"

#include "SDL3/SDL_timer.h"

#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;

#define LEFT_COL_ADD 1
#define RIGHT_COL_ADD 2
#define DOWN_ROW_ADD 4 
#define UP_ROW_ADD 8
#define LEFT_COL_DEL 16
#define RIGHT_COL_DEL 32
#define UP_ROW_DEL 64
#define DOWN_ROW_DEL 128

static void * setMapBottomModel(int * array)
{
    int firstBottom_X = array[0];
    int firstBottom_Y = array[1];
    Uint32 rowCount = array[2];
    Uint32 columnCount = array[3];
    Uint32 rowCopyCount = array[4];
    Uint32 columnCopyCount = array[5];

    Uint32 i, j;
    int tempBottomX = 0;
    int tempBottomY = 0;
    bool res = true;

    // print("wait semaphore");
    SDL_WaitSemaphore(allSync.bottomSemaphore);
    // print("done wait");

    for (i = 0;i < rowCount;i++)
    {
        tempBottomX = firstBottom_X;
        tempBottomY = firstBottom_Y - i * BOTTOM_HEIGHT;
        for (j = 0;j < columnCount;j++)
        {
            res = setModelMatrixByIndex(tempBottomX, tempBottomY, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM, i * columnCount + j);
            if (res == false)
            {
                addModelMatrix(tempBottomX, tempBottomY, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM); 
            }
            tempBottomX += BOTTOM_LENGTH;
        }
    }

    if (rowCopyCount * columnCopyCount > rowCount * columnCount)
    {
        Uint32 deleteCount = rowCopyCount * columnCopyCount - rowCount * columnCount;
        Uint32 initBottomCount = rowCopyCount * columnCopyCount;
        for (i = 1;i <= deleteCount;i++)
        {
            deleteModelMatrixByIndex(allInOne.pStaticModelPool, TEXTURE_BOTTOM, initBottomCount - i);
        }
    }

    return NULL;
}
static void setMapBottomModelExecuteFunc(void * data)
{
    G_Task * task = (G_Task*)data;
    setMapBottomModel(task->arg);
}
void setMapBottom(Uint32 width, Uint32 height, int centerX, int centerY, Uint32 * pRowCount, Uint32 * pColumnCount, int * pFirstBottom_X, int * pFirstBottom_Y, int * pBaseX, int * pBaseY, int32_t * pFirstBottomID)
{
    int i, j;

    Uint32 rowCount = *pRowCount;
    Uint32 columnCount = *pColumnCount;

    Uint32 rowCopyCount = rowCount;
    Uint32 columnCopyCount = columnCount;

    Uint32 drawIndex = 0;
    // Uint32 bottomCount = 0;
    Uint32 flag = 0;

    int firstBottom_X = 0;
    int firstBottom_Y = 0;

    int widthRange_L = 0;
    int widthRange_R = 0;
    int heightRange_T = 0;
    int heightRange_B = 0;

    int baseX = *pBaseX;
    int baseY = *pBaseY;

    int leftEdge = 0;
    int rightEdge = 0;
    int topEdge = 0;
    int bottomEdge = 0;

    int leftAddEdge = 0;
    int rightAddEdge = 0;
    int topAddEdge = 0;
    int bottomAddEdge = 0;

    bool upRowAdd = false;
    bool downRowAdd = false;
    bool leftColAdd = false;
    bool rightColAdd = false;

    bool upRowDel = false;
    bool downRowDel = false;
    bool leftColDel = false;
    bool rightColDel = false;

    int32_t firstBottomID = *pFirstBottomID;
    Map_Group * firstMapGroup = NULL;

    if (firstBottomID == -1)
    {
        firstMapGroup = getFirstMapGroup(TEXTURE_TILE_SET, MAIN_TILE_MAP);
        firstBottomID = firstMapGroup->groupID;
    }
    else
    {
        firstMapGroup = getMapGroup(TEXTURE_TILE_SET, MAIN_TILE_MAP, firstBottomID);
    }
    
    if (rowCount == 0 || columnCount == 0)
    {
        if (height % BOTTOM_LENGTH == 0)
        {
            rowCount = height / BOTTOM_LENGTH;
        }
        else
        {
            rowCount = height / BOTTOM_LENGTH + 1;
        }
        if (width % BOTTOM_LENGTH == 0)
        {
            columnCount = width / BOTTOM_LENGTH;
        }
        else
        {
            columnCount = width / BOTTOM_LENGTH + 1;
        }

        rowCount += 2;
        columnCount += 2;

        rowCopyCount = rowCount;
        columnCopyCount = columnCount;

        DrawHere tempInitDraw;
        Map_Group * tempInitMapGroup = firstMapGroup;
        Map_Group * tempLeftMapGroup = firstMapGroup;
        for (i = 0;i < rowCount;i++)
        {
            for (j = 0;j < columnCount;j++)
            {
                tempInitDraw.group = tempInitMapGroup;
                tempInitDraw.BottomID = i * columnCount + j;
                
                allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempInitDraw);
                setTilemapUVs(tempInitMapGroup, allInOne.pTileMapUVs, drawIndex);
                drawIndex++;

                tempInitMapGroup = tempInitMapGroup->right;
            }
            tempInitMapGroup = tempLeftMapGroup->down;
            tempLeftMapGroup = tempLeftMapGroup->down;
        }
        // bottomCount = rowCount * columnCount;
    }

    bool rowIsOdd = rowCount % 2;
    bool columnIsOdd = columnCount % 2;
    int halfRowCount = rowCount / 2;
    int halfColumnCount = columnCount / 2;

    if (rowIsOdd) firstBottom_Y = baseY + halfRowCount * BOTTOM_HEIGHT;
    else firstBottom_Y = baseY + halfRowCount * BOTTOM_HEIGHT - BOTTOM_HEIGHT/ 2;

    if (columnIsOdd) firstBottom_X = baseX - halfColumnCount * BOTTOM_LENGTH;
    else firstBottom_X = baseX - halfColumnCount * BOTTOM_LENGTH + BOTTOM_LENGTH / 2;

    leftEdge = firstBottom_X + BOTTOM_LENGTH + BOTTOM_LENGTH / 2;
    rightEdge = firstBottom_X + ((columnCount - 1) * BOTTOM_LENGTH) - BOTTOM_LENGTH - BOTTOM_LENGTH / 2;
    topEdge = firstBottom_Y - BOTTOM_HEIGHT - BOTTOM_HEIGHT / 2;
    bottomEdge = firstBottom_Y - ((rowCount - 1) * BOTTOM_HEIGHT) + BOTTOM_HEIGHT + BOTTOM_HEIGHT / 2;

    leftAddEdge = firstBottom_X + BOTTOM_LENGTH / 2;
    rightAddEdge = firstBottom_X + ((columnCount - 1) * BOTTOM_LENGTH) - BOTTOM_LENGTH / 2;
    topAddEdge = firstBottom_Y - BOTTOM_HEIGHT / 2;
    bottomAddEdge = firstBottom_Y - ((rowCount - 1) * BOTTOM_HEIGHT) + BOTTOM_HEIGHT / 2;
    
    widthRange_L = centerX - width / 2;
    widthRange_R = centerX + width / 2;
    heightRange_T = centerY + height / 2;
    heightRange_B = centerY - height / 2;

    // minus
    if (leftEdge < widthRange_L)
    {
        firstBottom_X += BOTTOM_LENGTH;
        baseX += BOTTOM_LENGTH / 2;
        columnCount--;

        if (firstMapGroup->right == NULL)
        {
        }
        else
        {
            firstMapGroup = firstMapGroup->right;
            firstBottomID = firstMapGroup->groupID;
            leftColDel = true;
            flag |= LEFT_COL_DEL;
        }
        // print("1");
    }
    if (rightEdge > widthRange_R)
    {
        baseX -= BOTTOM_LENGTH / 2;
        columnCount--;
        rightColDel = true;
        flag |= RIGHT_COL_DEL;
        // print("2");
    }
    if (topEdge > heightRange_T)
    {
        firstBottom_Y -= BOTTOM_HEIGHT;
        baseY -= BOTTOM_HEIGHT / 2;
        rowCount--;

        if (firstMapGroup->down == NULL)
        {
        }
        else
        {
            firstMapGroup = firstMapGroup->down;
            firstBottomID = firstMapGroup->groupID;
            upRowDel = true;
            flag |= UP_ROW_DEL;
        }
        // print("3");
    }
    if (bottomEdge < heightRange_B) 
    {
        baseY += BOTTOM_HEIGHT / 2;
        rowCount--;
        downRowDel = true;
        flag |= DOWN_ROW_DEL;
        // print("4");
    }

    // add
    if (leftAddEdge > widthRange_L)
    {
        firstBottom_X -= BOTTOM_LENGTH;
        baseX -= BOTTOM_LENGTH / 2;
        columnCount++;

        if (firstMapGroup->left == NULL)
        {
            firstBottom_X += BOTTOM_LENGTH;
            baseX += BOTTOM_LENGTH / 2;
            columnCount--;
        }
        else
        {
            firstMapGroup = firstMapGroup->left;
            firstBottomID = firstMapGroup->groupID;
            leftColAdd = true;
            flag |= LEFT_COL_ADD;
        }
        // print("5");
    }
    if (rightAddEdge < widthRange_R)
    {
        baseX += BOTTOM_LENGTH / 2;
        columnCount++;
        if (mapGroupToRight(firstMapGroup, columnCount - 1) == NULL)
        {
            baseX -= BOTTOM_LENGTH / 2;
            columnCount--;
        }
        else
        {
            rightColAdd = true;
            flag |= RIGHT_COL_ADD;
        }
        // print("6");
    }
    if (topAddEdge < heightRange_T)
    {
        firstBottom_Y += BOTTOM_HEIGHT;
        baseY += BOTTOM_HEIGHT / 2;
        rowCount++;

        if (firstMapGroup->up == NULL)
        {
            firstBottom_Y -= BOTTOM_HEIGHT;
            baseY -= BOTTOM_HEIGHT / 2;
            rowCount--;
        }
        else
        {
            firstMapGroup = firstMapGroup->up;
            firstBottomID = firstMapGroup->groupID;
            upRowAdd = true;
            flag |= UP_ROW_ADD;
        }
        // print("7");
    }
    if (bottomAddEdge > heightRange_B) 
    {
        baseY -= BOTTOM_HEIGHT / 2;
        rowCount++;

        if (mapGroupToDown(firstMapGroup, rowCount - 1) == NULL)
        {
            baseY += BOTTOM_HEIGHT / 2;
            rowCount--;
        }
        else
        {
            downRowAdd = true;
            flag |= DOWN_ROW_ADD;
        }
        // print("8");
    }


    *pFirstBottom_X = firstBottom_X;
    *pFirstBottom_Y = firstBottom_Y;

    *pBaseX = baseX;
    *pBaseY = baseY;

    *pRowCount = rowCount;
    *pColumnCount = columnCount;

    *pFirstBottomID = firstBottomID;

    if ((leftColAdd || leftColDel || rightColAdd || rightColDel) && (upRowAdd || upRowDel || downRowAdd || downRowDel))
    {
        print("left col add: %d, left col del :%d, right col add: %d, right col del: %d", leftColAdd, leftColDel, rightColAdd, rightColDel);
        print("top row add: %d, top row del: %d, bottom row add:%d, bottom row del: %d\n", upRowAdd, upRowDel, downRowAdd, downRowDel);
    }

    while (StackIsEmpty(allInOne.bottomImageMoveStack) == false)
    {
        SDL_Delay(1);
    }

    if (flag == LEFT_COL_ADD)
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount - 1;
        for (i = 0;i < rowCount;i++)
        {
            for (j = 0;j < initColumnCount;j++)
            {
                temp.from = i * initColumnCount + j; 
                temp.to = temp.from + i + 1;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }

        DrawHere tempDraw;
        Map_Group * tempMapGroup = firstMapGroup;
        for (i = 0;i < rowCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);
            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            
            tempMapGroup = tempMapGroup->down;
        }

        leftColAdd = false;
        print("flag: %u", flag);
    }
    else if (flag == LEFT_COL_DEL)
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount + 1;
        for (i = rowCount - 1;i > -1;i--)
        {
            for (j = columnCount;j > 0;j--)
            {
                temp.from = i * initColumnCount + j; 
                temp.to = temp.from - i - 1;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }

        leftColDel = false;
        print("flag: %u", flag);
    }
    else if (flag == RIGHT_COL_ADD)
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount - 1;
        for (i = 1;i < rowCount;i++)
        {
            for (j = 0;j < initColumnCount;j++)
            {
                temp.from = i * initColumnCount + j;
                temp.to = temp.from + i;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }

        DrawHere tempDraw;
        Map_Group * tempMapGroup = mapGroupToRight(firstMapGroup, columnCount - 1);
        for (i = 0;i < rowCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount + initColumnCount;
            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }

        rightColAdd = false;
        print("flag: %u", flag);
    }
    else if (flag == RIGHT_COL_DEL)
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount + 1;
        for (i = rowCount - 1;i > 0;i--)
        {
            for (j = columnCount - 1;j > -1;j--)
            {
                temp.from = i * initColumnCount + j; 
                temp.to = temp.from - i;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }

        rightColDel = false;
        print("flag: %u", flag);
    }
    else if (flag == UP_ROW_ADD)
    {
        FromTo temp;
        Uint32 initRowCount = rowCount - 1;
        for (i = 0;i < initRowCount;i++)
        {
            for (j = 0;j < columnCount;j++)
            {
                temp.from = i * columnCount + j;
                temp.to = temp.from + columnCount;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }

        DrawHere tempDraw;
        Map_Group * tempMapGroup = firstMapGroup;
        for (i = 0;i < columnCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->right;
        }

        upRowAdd = false;
        print("flag: %u", flag);
    }
    else if (flag == UP_ROW_DEL)
    {
        FromTo temp;
        Uint32 initRowCount = rowCount + 1;
        for (i = initRowCount;i > 0;i--)
        {
            for (j = columnCount;j > -1;j--)
            {
                temp.from = i * columnCount + j;
                temp.to = temp.from - columnCount;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }

        upRowDel = false;
        print("flag: %u", flag);
    }
    else if (flag == DOWN_ROW_ADD)
    {
        Uint32 initRowCount = rowCount - 1;

        DrawHere tempDraw;
        Map_Group * tempMapGroup = mapGroupToDown(firstMapGroup, initRowCount);
        for (i = 0;i < columnCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = initRowCount * columnCount + i;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->right;
        }

        downRowAdd = false;
        print("flag: %u", flag);
    }
    else if (flag == DOWN_ROW_DEL)
    {
        ;
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | UP_ROW_ADD))
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount - 1;
        for (i = 0;i < rowCount;i++)
        {
            for (j = 0;j < initColumnCount;j++)
            {
                temp.from = i * initColumnCount + j; 
                temp.to = temp.from + i + columnCount + 1;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = firstMapGroup;
        for (i = 0;i < columnCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->right;
        }
        tempMapGroup = firstMapGroup->down;
        for (i = 1;i < rowCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | DOWN_ROW_ADD))
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount - 1;
        for (i = 0;i < rowCount;i++)
        {
            for (j = 0;j < initColumnCount;j++)
            {
                temp.from = i * initColumnCount + j; 
                temp.to = temp.from + i + 1;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = mapGroupToDown(firstMapGroup, rowCount - 1);
        for (i = 0;i < columnCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->right;
        }
        tempMapGroup = firstMapGroup;
        for (i = 0;i < rowCount - 1;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (RIGHT_COL_ADD | UP_ROW_ADD))
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount - 1;
        for (i = 0;i < rowCount;i++)
        {
            for (j = 0;j < initColumnCount;j++)
            {
                temp.from = i * initColumnCount + j; 
                temp.to = temp.from + columnCount + i;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = firstMapGroup;
        for (i = 0;i < columnCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->right;
        }
        tempMapGroup = mapGroupToRight(firstMapGroup, columnCount - 1);
        for (i = 0;i < rowCount - 1;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (RIGHT_COL_ADD | DOWN_ROW_ADD))
    {
        FromTo temp;
        Uint32 initColumnCount = columnCount - 1;
        for (i = 1;i < rowCount;i++)
        {
            for (j = 0;j < initColumnCount;j++)
            {
                temp.from = i * initColumnCount + j; 
                temp.to = temp.from + i;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = mapGroupToDown(firstMapGroup, rowCount - 1);
        for (i = 0;i < columnCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->right;
        }
        tempMapGroup = mapGroupToRight(firstMapGroup, columnCount - 1);
        for (i = 0;i < rowCount - 1;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | RIGHT_COL_DEL))
    {
        FromTo temp;
        for (i = 0;i < rowCount;i++)
        {
            for (j = 0;j < columnCount - 1;j++)
            {
                temp.from = i * columnCount + j; 
                temp.to = temp.from + 1;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = firstMapGroup;
        for (i = 0;i < rowCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | UP_ROW_DEL))
    {
        FromTo temp;
        Uint32 initRowCount = rowCount + 1;
        Uint32 initColCount = columnCount - 1;
        for (i = initRowCount - 1;i > 0;i--)
        {
            for (j = initColCount - 1;j > -1;j--)
            {
                temp.from = i * initColCount + j; 
                temp.to = temp.from - initRowCount + i;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = firstMapGroup;
        for (i = 0;i < rowCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | DOWN_ROW_DEL))
    {
        FromTo temp;
        Uint32 initRowCount = rowCount + 1;
        Uint32 initColCount = columnCount - 1;
        for (i = 0;i < initRowCount - 1;i++)
        {
            for (j = 0;j < initColCount;j++)
            {
                temp.from = i * initColCount + j; 
                temp.to = temp.from + 1 + i;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = firstMapGroup;
        for (i = 0;i < rowCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | RIGHT_COL_ADD))
    {
        FromTo temp;
        for (i = 0;i < rowCount;i++)
        {
            for (j = columnCount - 1;j > -1;j--)
            {
                temp.from = i * columnCount + j;
                temp.to = temp.from - 1;
                allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
            }
        }
        
        DrawHere tempDraw;
        Map_Group * tempMapGroup = mapGroupToRight(firstMapGroup, columnCount - 1);
        for (i = 0;i < rowCount;i++)
        {
            tempDraw.group = tempMapGroup;
            tempDraw.BottomID = i * columnCount + columnCount - 1;

            allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

            setTilemapUVs(tempMapGroup, allInOne.pTileMapUVs, drawIndex);
            drawIndex++;

            tempMapGroup = tempMapGroup->down;
        }
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_DEL | RIGHT_COL_ADD))
    {

        print("flag: %u", flag);
    }
    else if (flag == (DOWN_ROW_DEL | RIGHT_COL_ADD))
    {

        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_ADD | DOWN_ROW_DEL))
    {

        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_ADD | LEFT_COL_DEL))
    {

        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_ADD | RIGHT_COL_DEL))
    {

        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_DEL | DOWN_ROW_ADD))
    {

        print("flag: %u", flag);
    }
    else if (flag == (RIGHT_COL_DEL | DOWN_ROW_ADD))
    {

        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | DOWN_ROW_ADD))
    {

        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | UP_ROW_DEL))
    {

        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | DOWN_ROW_DEL))
    {

        print("flag: %u", flag);
    }
    else if (flag == (RIGHT_COL_DEL | UP_ROW_DEL))
    {

        print("flag: %u", flag);
    }
    else if (flag == (RIGHT_COL_DEL | DOWN_ROW_DEL))
    {

        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | RIGHT_COL_DEL | UP_ROW_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | RIGHT_COL_DEL | DOWN_ROW_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | RIGHT_COL_DEL | UP_ROW_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | RIGHT_COL_DEL | DOWN_ROW_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | RIGHT_COL_ADD | UP_ROW_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | RIGHT_COL_ADD | DOWN_ROW_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | RIGHT_COL_ADD | UP_ROW_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | RIGHT_COL_ADD | DOWN_ROW_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_ADD | DOWN_ROW_DEL | LEFT_COL_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_ADD | DOWN_ROW_DEL | RIGHT_COL_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_ADD | DOWN_ROW_DEL | LEFT_COL_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_ADD | DOWN_ROW_DEL | RIGHT_COL_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_DEL | DOWN_ROW_ADD | LEFT_COL_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_DEL | DOWN_ROW_ADD | RIGHT_COL_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_DEL | DOWN_ROW_ADD | LEFT_COL_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (UP_ROW_DEL | DOWN_ROW_ADD | RIGHT_COL_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | RIGHT_COL_DEL | UP_ROW_ADD | DOWN_ROW_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_ADD | RIGHT_COL_DEL | UP_ROW_DEL | DOWN_ROW_ADD))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | RIGHT_COL_ADD | UP_ROW_ADD | DOWN_ROW_DEL))
    {
    
        print("flag: %u", flag);
    }
    else if (flag == (LEFT_COL_DEL | RIGHT_COL_ADD | UP_ROW_DEL | DOWN_ROW_ADD))
    {
    
        print("flag: %u", flag);
    }

    static int array[6];
    array[0] = firstBottom_X;
    array[1] = firstBottom_Y;
    array[2] = rowCount;
    array[3] = columnCount;
    array[4] = rowCopyCount;
    array[5] = columnCopyCount;
    static int * taskIndex = NULL;
    if (StackIsEmpty(allInOne.bottomImageMoveStack) == false)
    {
        G_WaitTask(allInOne.pThreadPool, taskIndex);
        G_Task task = {0};
        task.func = setMapBottomModel;
        task.executeFunc = setMapBottomModelExecuteFunc;
        task.arg = array;
        taskIndex = G_AddTask(allInOne.pThreadPool, 1, 1, &task);
    }
    else
    {
        SDL_SignalSemaphore(allSync.bottomSemaphore);
        // print("signal bottom");
        setMapBottomModel(array);
    }

    // print("Resolution %dx%d\n", width, height);
    // print("Row count: %d, Column count: %d\n", rowCount, columnCount);
    // print("First bottom X: %d, First bottom Y: %d\n", firstBottom_X, firstBottom_Y);
    // print("Width range L: %d, Width range R: %d\n", widthRange_L, widthRange_R);
    // print("Left edge: %d, Right edge: %d\n", leftEdge, rightEdge);
    // print("Left add edge: %d, Right add edge: %d\n", leftAddEdge, rightAddEdge);
    // print("Height range T: %d, Height range B: %d\n", heightRange_T, heightRange_B);
    // print("Top edge: %d, Bottom edge: %d\n", topEdge, bottomEdge);
    // print("Top add edge: %d, Bottom add edge: %d\n", topAddEdge, bottomAddEdge);
    // print("Base X: %d, Base Y: %d\n", baseX, baseY);
    // print("Center X: %d, Center Y: %d\n", centerX, centerY);
}
static bool findInUint32Array(Uint32 * array, Uint32 arraySize, Uint32 num)
{
    for (Uint32 i = 0;i < arraySize;i++)
    {
        if (array[i] == num) return true;
    }

    return false;
}
bool moveBottomImage(Uint32 currentFrame)
{
    if (StackIsEmpty(allInOne.bottomImageMoveStack) == true) return false;

    int32_t i;
    FromTo tempFromTo[48];
    VkCommandBuffer transferCommandBuffer = allInOne.pTransferCommandBuffer[0];
    VkCommandBuffer graphicCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    VkFence transferFence = allInOne.pTransferInFlightFence[0];
    VkFence graphicFence = allInOne.pGraphicInFlightFence[currentFrame];
    
    VkSemaphore transferSemaphore = allInOne.pTransferSemaphore[0];

    SDL_LockMutex(allSync.renderMutex);
    Uint32 arrayCap = (allInOne.bottomImageMoveStack.top + 1) * 2;
    while (StackIsEmpty(allInOne.bottomImageMoveStack) == false)
    {
        allInOne.bottomImageMoveStack.popFn(&allInOne.bottomImageMoveStack, &tempFromTo[i]);
        i++;
    }
    SDL_UnlockMutex(allSync.renderMutex);

    G_Texture_P * imageArray = getTexture(TEXTURE_MAP_ARRAY);

    Uint32 imageMemoryBarrierCount = 0;
    Uint32 * notShaderReadOnly = (Uint32*)SDL_malloc(arrayCap * sizeof(Uint32));
    if (notShaderReadOnly == NULL)
    {
        return false;
    }
    memset(notShaderReadOnly, UINT32_MAX, arrayCap * sizeof(Uint32));

    Uint32 graphicFamilyIndice = allInOne.queueFamilyIndices.graphicsFamily.familyIndice;
    Uint32 transferFamilyIndice = allInOne.queueFamilyIndices.transferFamily.familyIndice;

    VkImageMemoryBarrier imageMemoryBarrierGraphicRelease[48];
    VkImageMemoryBarrier imageMemoryBarrierGraphicGet[48];

    VkImageMemoryBarrier imageMemoryBarrierTransferGet[48];
    VkImageMemoryBarrier imageMemoryBarrierTransferRelease[48];

    for (i = 0;i < arrayCap / 2;i++)
    {
        if (findInUint32Array(notShaderReadOnly, arrayCap, tempFromTo[i].from) == false)
        {
            // print("image %u layout: %u", tempFromTo.from, imageArray->layouts[tempFromTo.from]);

            setTextureImageMemoryBarrier(NULL, VK_ACCESS_SHADER_READ_BIT, 0, VK_IMAGE_LAYOUT_UNDEFINED, graphicFamilyIndice, transferFamilyIndice, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, tempFromTo[i].from\
                , 1, imageMemoryBarrierGraphicRelease + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo.from, imageArray->layouts[tempFromTo.from]);

            setTextureImageMemoryBarrier(NULL, 0, VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, graphicFamilyIndice, transferFamilyIndice\
                , VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, tempFromTo[i].from, 1, imageMemoryBarrierTransferGet + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo.from, imageArray->layouts[tempFromTo.from]);

            notShaderReadOnly[imageMemoryBarrierCount] = tempFromTo[i].from;
            imageMemoryBarrierCount++;
        }

        if (findInUint32Array(notShaderReadOnly, arrayCap, tempFromTo[i].to) == false)
        {
            // print("image %u layout: %u", tempFromTo.to, imageArray->layouts[tempFromTo.to]);

            setTextureImageMemoryBarrier(NULL, VK_ACCESS_SHADER_READ_BIT, 0, VK_IMAGE_LAYOUT_UNDEFINED, graphicFamilyIndice, transferFamilyIndice, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, tempFromTo[i].to, 1\
                , imageMemoryBarrierGraphicRelease + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo.to, imageArray->layouts[tempFromTo.to]);

            setTextureImageMemoryBarrier(NULL, 0, VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED, graphicFamilyIndice, transferFamilyIndice, VK_IMAGE_ASPECT_COLOR_BIT\
                , 0, 1, tempFromTo[i].to, 1, imageMemoryBarrierTransferGet + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo.to, imageArray->layouts[tempFromTo.to]);

            notShaderReadOnly[imageMemoryBarrierCount] = tempFromTo[i].to;
            imageMemoryBarrierCount++;
        }
    }

    vkWaitForFences(allInOne.device, 1, &graphicFence, VK_TRUE, UINT64_MAX);
    vkResetFences(allInOne.device, 1, &graphicFence);
    vkWaitForFences(allInOne.device, 1, &transferFence, VK_TRUE, UINT64_MAX);
    vkResetFences(allInOne.device, 1, &transferFence);

    vkResetCommandBuffer(graphicCommandBuffer, 0);
    beginCommandBuffer(graphicCommandBuffer);
    vkCmdPipelineBarrier(graphicCommandBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, imageMemoryBarrierCount, imageMemoryBarrierGraphicRelease);
    vkEndCommandBuffer(graphicCommandBuffer);

    VkSubmitInfo graphicSubmitInfo = {};
    setSubmitInfo(NULL, 0, NULL, NULL, 1, &graphicCommandBuffer, 1, &transferSemaphore, &graphicSubmitInfo);
    vkQueueSubmit(getGraphic2dQueue(), 1, &graphicSubmitInfo, transferFence);

    vkWaitForFences(allInOne.device, 1, &transferFence, VK_TRUE, UINT64_MAX);
    vkResetFences(allInOne.device, 1, &transferFence);

    vkResetCommandBuffer(transferCommandBuffer, 0);
    beginCommandBuffer(transferCommandBuffer);
    vkCmdPipelineBarrier(transferCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, imageMemoryBarrierCount, imageMemoryBarrierTransferGet);
    vkEndCommandBuffer(transferCommandBuffer);

    VkSubmitInfo transferSubmitInfo1 = {};
    VkPipelineStageFlags waitStage1[] = {VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
    setSubmitInfo(NULL, 1, &transferSemaphore, waitStage1, 1, &transferCommandBuffer, 1, &transferSemaphore, &transferSubmitInfo1);
    vkQueueSubmit(getTransferQueue(), 1, &transferSubmitInfo1, transferFence);

    vkWaitForFences(allInOne.device, 1, &transferFence, VK_TRUE, UINT64_MAX);
    vkResetFences(allInOne.device, 1, &transferFence);

    vkResetCommandBuffer(transferCommandBuffer, 0);
    beginCommandBuffer(transferCommandBuffer);
    for (i = 0;i < arrayCap / 2;i++)
    {
        VkImageMemoryBarrier tempImageMemoryBarrier = {};
        setTextureImageMemoryBarrier(NULL, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, VK_IMAGE_ASPECT_COLOR_BIT\
            , 0, 1, tempFromTo[i].to, 1, &tempImageMemoryBarrier, imageArray);
        
        vkCmdPipelineBarrier(transferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &tempImageMemoryBarrier);

        VkImageCopy region = {};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.mipLevel = 0;
        region.srcSubresource.baseArrayLayer = tempFromTo[i].from;
        region.srcSubresource.layerCount = 1;
        region.srcOffset.x = 0;
        region.srcOffset.y = 0;
        region.srcOffset.z = 0;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.baseArrayLayer = tempFromTo[i].to;
        region.dstSubresource.layerCount = 1;
        region.dstOffset.x = 0;
        region.dstOffset.y = 0;
        region.dstOffset.z = 0;
        region.extent.width = BOTTOM_WIDTH;
        region.extent.height = BOTTOM_HEIGHT;
        region.extent.depth = 1; 

        vkCmdCopyImage(transferCommandBuffer, imageArray->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageArray->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    }

    imageMemoryBarrierCount = 0;
    memset(notShaderReadOnly, UINT32_MAX, arrayCap * sizeof(Uint32));
    for (i = 0;i < arrayCap / 2;i++)
    {
        if (findInUint32Array(notShaderReadOnly, arrayCap, tempFromTo[i].from) == false)
        {
            // print("image %u layout: %u", tempFromTo[i].from, imageArray->layouts[tempFromTo[i].from]);

            setTextureImageMemoryBarrier(NULL, VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_UNDEFINED, transferFamilyIndice, graphicFamilyIndice, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, tempFromTo[i].from\
                , 1, imageMemoryBarrierTransferRelease + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo[i].from, imageArray->layouts[tempFromTo[i].from]);

            setTextureImageMemoryBarrier(NULL, 0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transferFamilyIndice, graphicFamilyIndice\
                , VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, tempFromTo[i].from, 1, imageMemoryBarrierGraphicGet + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo[i].from, imageArray->layouts[tempFromTo[i].from]);

            notShaderReadOnly[imageMemoryBarrierCount] = tempFromTo[i].from;
            imageMemoryBarrierCount++;
        }

        if (findInUint32Array(notShaderReadOnly, arrayCap, tempFromTo[i].to) == false)
        {
            // print("image %u layout: %u", tempFromTo[i].to, imageArray->layouts[tempFromTo[i].to]);

            setTextureImageMemoryBarrier(NULL, VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_UNDEFINED, transferFamilyIndice, graphicFamilyIndice, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, tempFromTo[i].to\
                , 1, imageMemoryBarrierTransferRelease + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo[i].to, imageArray->layouts[tempFromTo[i].to]);

            setTextureImageMemoryBarrier(NULL, 0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transferFamilyIndice, graphicFamilyIndice\
                , VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, tempFromTo[i].to, 1, imageMemoryBarrierGraphicGet + imageMemoryBarrierCount, imageArray);

            // print("image %u layout: %u", tempFromTo[i].to, imageArray->layouts[tempFromTo[i].to]);

            notShaderReadOnly[imageMemoryBarrierCount] = tempFromTo[i].to;
            imageMemoryBarrierCount++;
        }
    }

    vkCmdPipelineBarrier(transferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, imageMemoryBarrierCount, imageMemoryBarrierTransferRelease);
   
    vkEndCommandBuffer(transferCommandBuffer);

    VkSubmitInfo transferSubmitInfo2 = {};
    VkPipelineStageFlags waitStage[] = {VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

    setSubmitInfo(NULL, 1, &transferSemaphore, waitStage, 1, &transferCommandBuffer, 1, &transferSemaphore, &transferSubmitInfo2);
    vkQueueSubmit(getTransferQueue(), 1, &transferSubmitInfo2, transferFence);

    vkResetCommandBuffer(graphicCommandBuffer, 0);
    beginCommandBuffer(graphicCommandBuffer);
    vkCmdPipelineBarrier(graphicCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, imageMemoryBarrierCount, imageMemoryBarrierGraphicGet);
    vkEndCommandBuffer(graphicCommandBuffer);

    VkSubmitInfo graphicSubmitInfo2 = {};
    setSubmitInfo(NULL, 1, &transferSemaphore, waitStage, 1, &graphicCommandBuffer, 0, NULL, &graphicSubmitInfo2);
    vkQueueSubmit(getGraphic2dQueue(), 1, &graphicSubmitInfo2, graphicFence); 

    // print("signal bottom");
    SDL_SignalSemaphore(allSync.bottomSemaphore);

    return true;
}
static G_Point_Int locatePointInGroup(int32_t x, int32_t y, Map_Group * group)
{
    const int32_t maxRow = 49;
    const int32_t maxCol = 49;

    int32_t row, col;
    // row = SDL_min((BOTTOM_WIDTH - x) / TILE_WIDTH, maxRow);
    row = SDL_min(x / TILE_WIDTH, maxRow);
    col = SDL_min((BOTTOM_HEIGHT - y) / TILE_HEIGHT, maxCol);
    // col = SDL_min(y / TILE_HEIGHT, maxCol);

    int32_t tileCenterX = row * TILE_WIDTH + TILE_WIDTH / 2;
    int32_t tileCenterY = BOTTOM_HEIGHT - (col * TILE_HEIGHT + TILE_HEIGHT / 2);

    // print("index: %u(%d, %d)", group->indices[row][col], x, y);
    return (G_Point_Int){tileCenterX, tileCenterY};
}
void locatePoint(G_Entity * entity, Uint32 groupRowCount, Uint32 groupColCount, int32_t firstBottom_X, int32_t firstBottom_Y, Uint32 groupID)
{
    int32_t topLine = firstBottom_Y + BOTTOM_HEIGHT / 2;
    int32_t leftLine = firstBottom_X - BOTTOM_WIDTH / 2;
    
    // if (entity->position[0] < firstBottom_X) return;
    // if (entity->position[1] > firstBottom_Y) return;
    Uint32 row, col;

    int32_t pointX = (Uint32)entity->position[0];
    int32_t pointY = (Uint32)entity->position[1];

    int32_t offsetLengthX = pointX - leftLine;
    int32_t offsetLengthY = topLine - pointY;

    if (offsetLengthX < 0 || offsetLengthY < 0) return;

    row = offsetLengthX / BOTTOM_WIDTH;
    col = offsetLengthY / BOTTOM_HEIGHT;

    if (row > groupRowCount) return;
    if (col > groupColCount) return;

    Map_Group * firstMapGroup = getMapGroup(TEXTURE_TILE_SET, MAIN_TILE_MAP, groupID);

    Map_Group * group = mapGroupToDown(firstMapGroup, row);
    group = mapGroupToRight(group, col);

    int32_t distanceX = -(leftLine + row * BOTTOM_WIDTH);
    int32_t distanceY = 800 - (topLine - col * BOTTOM_HEIGHT);

    int32_t clampPointX = pointX + distanceX;
    int32_t clampPointY = pointY + distanceY;

    G_Point_Int tileCenter = locatePointInGroup(clampPointX, clampPointY, group);

    print("center (%d, %d)", tileCenter.x - distanceX, tileCenter.y - distanceY);
}
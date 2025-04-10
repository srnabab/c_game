#include "G_constants.h"
#include "G_map.h"
#include "G_staticModel.h"
#include "G_log.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

static int calculateFirstBottomX(int baseX, int columnCount)
{
    int halfColumnCount = columnCount / 2;
    bool columnIsOdd = columnCount % 2;

    if (columnIsOdd) return baseX + halfColumnCount * BOTTOM_LENGTH;
    else return baseX + halfColumnCount * BOTTOM_LENGTH - BOTTOM_LENGTH / 2;
}
void setMapBottom(Uint32 width, Uint32 height, int centerX, int centerY, Uint32 * pRowCount, Uint32 * pColumnCount, int * pFirstBottom_X, int * pFirstBottom_Y, int * pBaseX, int * pBaseY)
{
    Uint32 rowCount = *pRowCount;
    Uint32 columnCount = *pColumnCount;
    Uint32 bottomCount = 0;

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
        bottomCount = rowCount * columnCount;
    }

    bool rowIsOdd = rowCount % 2;
    bool columnIsOdd = columnCount % 2;
    int halfRowCount = rowCount / 2;
    int halfColumnCount = columnCount / 2;

    if (rowIsOdd) firstBottom_Y = baseY - halfRowCount * BOTTOM_HEIGHT;
    else firstBottom_Y = baseY - halfRowCount * BOTTOM_HEIGHT + BOTTOM_HEIGHT/ 2;

    if (columnIsOdd) firstBottom_X = baseX + halfColumnCount * BOTTOM_LENGTH;
    else firstBottom_X = baseX + halfColumnCount * BOTTOM_LENGTH - BOTTOM_LENGTH / 2;

    leftEdge = firstBottom_X - BOTTOM_LENGTH - BOTTOM_LENGTH / 2;
    rightEdge = firstBottom_X - ((columnCount - 1) * BOTTOM_LENGTH) + BOTTOM_LENGTH + BOTTOM_LENGTH / 2;
    topEdge = firstBottom_Y + BOTTOM_HEIGHT + BOTTOM_HEIGHT / 2;
    bottomEdge = firstBottom_Y + ((rowCount - 1) * BOTTOM_HEIGHT) - BOTTOM_HEIGHT - BOTTOM_HEIGHT / 2;

    leftAddEdge = firstBottom_X - BOTTOM_LENGTH / 2;
    rightAddEdge = firstBottom_X - ((columnCount - 1) * BOTTOM_LENGTH) + BOTTOM_LENGTH / 2;
    topAddEdge = firstBottom_Y + BOTTOM_HEIGHT / 2;
    bottomAddEdge = firstBottom_Y + ((rowCount - 1) * BOTTOM_HEIGHT) - BOTTOM_HEIGHT / 2;
    
    widthRange_L = centerX + width / 2;
    widthRange_R = centerX - width / 2;
    heightRange_T = centerY - height / 2;
    heightRange_B = centerY + height / 2;

    if (leftEdge > widthRange_L)
    {
        firstBottom_X -= BOTTOM_LENGTH;
        baseX -= BOTTOM_LENGTH;
        columnCount--;
        // print("del left col");
    }
    if (rightEdge < widthRange_R)
    {
        columnCount--;
        // print("del right col");
    }
    if (topEdge < heightRange_T)
    {
        firstBottom_Y += BOTTOM_HEIGHT;
        baseY += BOTTOM_HEIGHT;
        rowCount--;
    }
    if (bottomEdge > heightRange_B) 
    {
        rowCount--;
    }

    if (leftAddEdge < widthRange_L)
    {
        firstBottom_X += BOTTOM_LENGTH;
        baseX += BOTTOM_LENGTH;
        columnCount++;
        // print("add left col");
    }
    if (rightAddEdge > widthRange_R)
    {
        columnCount++;
        // print("add right col");
    }
    if (topAddEdge > heightRange_T)
    {
        firstBottom_Y -= BOTTOM_HEIGHT;
        baseY -= BOTTOM_HEIGHT;
        rowCount++;
    }
    if (bottomAddEdge < heightRange_B) 
    {
        rowCount++;
    }


    *pFirstBottom_X = firstBottom_X;
    *pFirstBottom_Y = firstBottom_Y;

    *pBaseX = baseX;
    *pBaseY = baseY;

    int i, j;
    int tempBottomX = 0;
    int tempBottomY = 0;
    bool res = true;
    for (i = 0;i < rowCount;i++)
    {
        tempBottomX = firstBottom_X;
        tempBottomY = firstBottom_Y + i * BOTTOM_HEIGHT;
        for (j = 0;j < columnCount;j++)
        {
            res = setModelMatrixByIndex(tempBottomX, tempBottomY, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM, i * columnCount + j);
            if (res == false)
            {
                addModelMatrix(tempBottomX, tempBottomY, -1, allInOne.pStaticModelPool, TEXTURE_BOTTOM); 
            }
            tempBottomX -= BOTTOM_LENGTH;
        }
    }

    // print("Resolution %dx%d\n", width, height);
    // print("Row count: %d, Column count: %d\n", rowCount, columnCount);
    // printf("Bottom count: %d\n", bottomCount);
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
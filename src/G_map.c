#include "G_constants.h"
#include "G_staticModel.h"
#include "G_map.h"
#include "G_log.h"

#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

void setMapBottom(Uint32 width, Uint32 height, int centerX, int centerY, Uint32 * pRowCount, Uint32 * pColumnCount, int * pFirstBottom_X, int * pFirstBottom_Y, int * pBaseX, int * pBaseY, int32_t * pFirstBottomID)
{
    int i, j;

    Uint32 rowCount = *pRowCount;
    Uint32 columnCount = *pColumnCount;

    Uint32 rowCopyCount = rowCount;
    Uint32 columnCopyCount = columnCount;

    // Uint32 bottomCount = 0;

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
        }
        // print("1");
    }
    if (rightEdge > widthRange_R)
    {
        baseX -= BOTTOM_LENGTH / 2;
        columnCount--;
        rightColDel = true;
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
        }
        // print("3");
    }
    if (bottomEdge < heightRange_B) 
    {
        baseY += BOTTOM_HEIGHT / 2;
        rowCount--;
        downRowDel = true;
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

    int tempBottomX = 0;
    int tempBottomY = 0;
    bool res = true;
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

    // if (leftColAdd)
    // {
    //     FromTo temp;
    //     Uint32 initColumnCount = columnCount - 1;
    //     for (i = 0;i < rowCount;i++)
    //     {
    //         for (j = 0;j < initColumnCount;j++)
    //         {
    //             temp.from = i * initColumnCount + j; 
    //             temp.to = temp.from + i + 1;
    //             allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
    //         }
    //     }

    //     DrawHere tempDraw;
    //     Map_Group * tempMapGroup = firstMapGroup;
    //     for (i = 0;i < rowCount;i++)
    //     {
    //         tempDraw.group = tempMapGroup;
    //         tempDraw.BottomID = i * columnCount;

    //         allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);
            
    //         tempMapGroup = firstMapGroup->down;
    //     }

    //     leftColAdd = false;
    // }

    // if (rightColAdd)
    // {
    //     FromTo temp;
    //     Uint32 initColumnCount = columnCount - 1;
    //     for (i = 1;i < rowCount;i++)
    //     {
    //         for (j = 0;j < initColumnCount;j++)
    //         {
    //             temp.from = i * initColumnCount + j;
    //             temp.to = temp.from + i;
    //             allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
    //         }
    //     }

    //     DrawHere tempDraw;
    //     Map_Group * tempMapGroup = firstMapGroup;
    //     for (i = 0;i < rowCount;i++)
    //     {
    //         tempDraw.group = tempMapGroup;
    //         tempDraw.BottomID = i * columnCount + initColumnCount;
    //         allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

    //         tempMapGroup = firstMapGroup->down;
    //     }

    //     rightColAdd = false;
    // }

    // if (leftColDel)
    // {
    //     FromTo temp;
    //     Uint32 initColumnCount = columnCount + 1;
    //     for (i = rowCount - 1;i > -1;i--)
    //     {
    //         for (j = columnCount;j > 0;j--)
    //         {
    //             temp.from = i * initColumnCount + j; 
    //             temp.to = temp.from - i - 1;
    //             allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
    //         }
    //     }

    //     leftColDel = false;
    // }

    // if (rightColDel)
    // {
    //     FromTo temp;
    //     Uint32 initColumnCount = columnCount + 1;
    //     for (i = rowCount - 1;i > 0;i--)
    //     {
    //         for (j = columnCount - 1;j > -1;j--)
    //         {
    //             temp.from = i * initColumnCount + j; 
    //             temp.to = temp.from - i;
    //             allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
    //         }
    //     }

    //     rightColDel = false;
    // }

    // if (upRowAdd)
    // {
    //     FromTo temp;
    //     Uint32 initRowCount = rowCount - 1;
    //     for (i = 0;i < initRowCount;i++)
    //     {
    //         for (j = 0;j < columnCount;j++)
    //         {
    //             temp.from = i * columnCount + j;
    //             temp.to = temp.from + columnCount;
    //             allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
    //         }
    //     }

    //     DrawHere tempDraw;
    //     Map_Group * tempMapGroup = firstMapGroup;
    //     for (i = 0;i < columnCount;i++)
    //     {
    //         tempDraw.group = tempMapGroup;
    //         tempDraw.BottomID = i;
    //         allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

    //         tempMapGroup = tempMapGroup->right;
    //     }

    //     upRowAdd = false;
    // }

    // if (upRowDel)
    // {
    //     FromTo temp;
    //     Uint32 initRowCount = rowCount + 1;
    //     for (i = initRowCount;i > 0;i--)
    //     {
    //         for (j = columnCount;j > -1;j--)
    //         {
    //             temp.from = i * columnCount + j;
    //             temp.to = temp.from - columnCount;
    //             allInOne.bottomImageMoveStack.pushFn(&allInOne.bottomImageMoveStack, &temp);
    //         }
    //     }

    //     upRowAdd = false;
    // }

    // if (downRowAdd)
    // {
    //     Uint32 initRowCount = rowCount - 1;

    //     DrawHere tempDraw;
    //     Map_Group * tempMapGroup = firstMapGroup;
    //     for (i = 0;i < columnCount;i++)
    //     {
    //         tempDraw.group = tempMapGroup;
    //         tempDraw.BottomID = initRowCount * columnCount + i;
    //         allInOne.bottomImageDrawStack.pushFn(&allInOne.bottomImageDrawStack, &tempDraw);

    //         tempMapGroup = tempMapGroup->right;
    //     }
    // }

    // if (downRowDel)
    // {
    //     ;
    // }

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
    if ((leftColAdd || leftColDel || rightColAdd || rightColDel) && (upRowAdd || upRowDel || downRowAdd || downRowDel))
    {
        print("left col add: %d, left col del :%d, right col add: %d, right col del: %d", leftColAdd, leftColDel, rightColAdd, rightColDel);
        print("top row add: %d, top row del: %d, bottom row add:%d, bottom row del: %d\n", upRowAdd, upRowDel, downRowAdd, downRowDel);
    }
}
static bool findInUint32Array(Uint32 * array, Uint32 arraySize, Uint32 num)
{
    for (Uint32 i = 0;i < arraySize;i++)
    {
        if (array[i] == num) return true;
    }

    return false;
}
void moveBottomImage(Uint32 currentFrame)
{
    FromTo tempFromTo = {};
    VkCommandBuffer commandBuffer = (*allInOne.ppTransferCommandBuffer)[currentFrame];
    G_Texture_P * imageArray = getTexture(TEXTURE_MAP_ARRAY);
    Uint32 arrayCap = allInOne.bottomImageMoveStack.top + 1;
    Uint32 offset = 0;
    Uint32 * notShaderReadOnly = (Uint32*)SDL_malloc(arrayCap * sizeof(Uint32));

    beginCommandBuffer(commandBuffer);
    while (StackIsEmpty(allInOne.bottomImageMoveStack) == false)
    {
        allInOne.bottomImageMoveStack.popFn(&allInOne.bottomImageMoveStack, &tempFromTo);
        transitionImageLayout(commandBuffer, imageArray->image, imageArray->format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, tempFromTo.from, 1);
        notShaderReadOnly[offset] = tempFromTo.from;
        offset++;

        if (findInUint32Array(notShaderReadOnly, arrayCap, tempFromTo.to))
        {
            transitionImageLayout(commandBuffer, imageArray->image, imageArray->format, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, tempFromTo.to, 1);
        }
        else
        {
            transitionImageLayout(commandBuffer, imageArray->image, imageArray->format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, tempFromTo.to, 1);
            notShaderReadOnly[offset] = tempFromTo.from;
            offset++;
        }

        VkImageCopy region = {};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.mipLevel = 0;
        region.srcSubresource.baseArrayLayer = tempFromTo.from;
        region.srcSubresource.layerCount = 1;
        region.srcOffset.x = 0;
        region.srcOffset.y = 0;
        region.srcOffset.z = 0;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.baseArrayLayer = tempFromTo.to;
        region.dstSubresource.layerCount = 1;
        region.dstOffset.x = 0;
        region.dstOffset.y = 0;
        region.dstOffset.z = 0;
        region.extent.width = BOTTOM_WIDTH;
        region.extent.width = BOTTOM_HEIGHT;
        region.extent.depth = 1; 

        vkCmdCopyImage((*allInOne.ppTransferCommandBuffer)[currentFrame], imageArray->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageArray->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL\
            , 1, &region);

        transitionImageLayout(commandBuffer, imageArray->image, imageArray->format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, tempFromTo.to, 1);
    }
    VkSubmitInfo submitInfo = {};

    setSubmitInfo(NULL, 0, NULL, NULL, 1, &commandBuffer, 0, NULL, &submitInfo);
    vkQueueSubmit(getTransferQueue(), 1, &submitInfo, NULL);
    
    vkEndCommandBuffer(commandBuffer);
}
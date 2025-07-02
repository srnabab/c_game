#include "content_manager/content_manager.h"

int main(int argc, char * argv[])
{
    initMemoryRecord();
    generatePath(argc, argv);
    printResidueMemory();

    return 0;
}

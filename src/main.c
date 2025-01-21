#include "G_game.h"

int main(int argc, char* argv[]) 
{
    setup(argc, argv);

    while (true) 
    {
        if (process_input())
        {
            destroy();
        }
    }    

    return 0;
}
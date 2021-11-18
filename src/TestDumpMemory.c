#include "DumpMemory.h"

#include <stdbool.h>

#if !defined(Host)

#include "bsl_Uart.h"

#endif
/*-------------------------------------------------------------------
* main
-------------------------------------------------------------------*/
int main(void)
{
    #if !defined(Host)
    bsl_Uart_Init();
    #endif

    bool run = true;
    static uint8_t mem[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31
    };

    PutS("Usage: type 'm'(memory) and 'q' to quit");
    while (run)
    {
        PutN();
        PutS("$ ");
        char cmd = GetC();
        // PutN();
        switch (cmd) {
        case 'm':
            #if !defined(Host)
            DumpMemory((uint8_t*) 0, 32);
            #else
            DumpMemory(mem, 32);
            #endif
            break;
        case 'q':
            PutS("bye!");
            run = false;
            break;
        case '\n':
            break;
        default: 
            PutS("Invalid command");
            break;
        }
    }

    return 0;
}

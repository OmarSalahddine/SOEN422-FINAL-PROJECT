#include "DumpMemory.h"
#if !defined(Host)
#include "bsl_Uart.h"
#include <stdbool.h>
#endif
/*-------------------------------------------------------------------
* main
-------------------------------------------------------------------*/
int main(void)
{
    bsl_Uart_Init();
    bool run = true;
    static uint8_t mem[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31};
    PutS("Usage: type 'm'(memory) and 'q' to quit");
    while (run)
    {
        PutN();
        PutS("$");
        char cmd = GetC();
        PutN();
        switch (cmd)
        {
        case 'm':
            DumpMemory((uint8_t *)mem, 32);
            break;
        case 'q':
         PutS("bye!");
            run = false;
            break;

        default: 
        PutS("Invalid command");
        break;
        }
    }

    return 0;
}

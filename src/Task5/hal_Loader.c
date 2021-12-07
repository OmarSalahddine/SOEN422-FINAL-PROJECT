#include "hal_Loader.h"
#include <stdbool.h>

Cmd cmd;
Status status;

void hal_Init() {}

uint8_t hal_Loader(uint8_t* mem)
{
    // small buffer to receive one byte at a time max 11
    // use bsl_Uart to handle the bytes.
    while (true)
    {
        //reveiving packet data
        mem = (uint8_t)bsl_Uart_RxChar();
        //inputing bytes one by one into the buffer
        for (uint8_t index = 1; index < mem[0]; index++)
         {             
            bsl_Uart_TxChar((char)mem[index]);
         }
    }
}

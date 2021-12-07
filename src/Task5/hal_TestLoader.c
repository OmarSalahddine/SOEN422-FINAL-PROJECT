// hal_TestLoader.c - Test Loader implementation for HAL using bsl_Uart.c

#include "COutForAUnit.h"
#include "bsl_Uart.h"
#include "hal_Loader.h"
#include <stdbool.h>

#define Target "(ATMega328P)"
#define VMName "Small Edison Virtual Machine "
#define AppSuffix ""
#define AppName "se"
#define Version " v1.0 "
#define Copyright "Copyright (c) 2021  Michel de Champlain"

static void DisplayBanner(){
    PutS(VMName);
    PutS(AppSuffix);
    PutS(Version);
    PutS(Target);
    PutN();
    PutS(Copyright);
    PutN();
}

#define MemMax 256

static uint8_t mem[MemMax];

int main()
{
    bsl_Uart_Init();
    hal_Init();

    while (true)
    {
        uint8_t status = Success;
        if ((status = hal_Loader(mem)) == Success)
        {
            DisplayBanner();
            VM_Init();
            VM_execute(mem);

            // Send an Ack to tell the Host that program's execution is done.
            PutC((char)Ack);
            PutC((char)0);
        }
        else
        {
            PutS("Loader error: ");
            PutX4(status);
            PutN();
            return -1;
        }
    }
    return 0;
}

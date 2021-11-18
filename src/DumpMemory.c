#include "COutForAUnit.h"
#include "DumpMemory.h"

#if !defined(Host)
#include "bsl_Uart.h"
#endif

void DumpMemory(uint8_t *from, uint16_t nBytes)
{
    int i = 0;
    while (i < nBytes)
    {
        PutX4(from);
        from++;
        i++;
        if (i != nBytes)
            PutN();
    }
}


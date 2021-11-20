#include "COutForAUnit.h"
#include "DumpMemory.h"

#if !defined(Host)
#include "bsl_Uart.h"
#endif

void DumpMemory(uint8_t* from, uint16_t nBytes)
{
    for(uint8_t i = 0; i < nBytes; i++)
    {
        if(i%16==0){
          if(i!=0){
          PutN();
          }
          PutX16(from);
        } else {
          PutS(" ");
          PutX8(*from);
        }
        from++;
    }
}

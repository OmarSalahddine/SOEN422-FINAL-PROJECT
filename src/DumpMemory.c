#include "COutForAUnit.h"
#include "bsl_Uart.h"
#include "DumpMemory.h"

void DumpMemory(uint8_t *from, uint16_t nBytes){
int i = 0;
while(i < nBytes){
PutX16(*from);
*from++;
i++;
if(i != nBytes)
PutN();
}

}


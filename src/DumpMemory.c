#include "COutForAUnit.h"
#include "bsl_Usart.h"
#include "DumpMemory.h"

void DumpMemory(uint8_t* from, uint16_t nBytes){
int i = 0;
while(i < nBytes){
PutX16(from);
from++;
i++;
PutS(" ");
}
PutN();
}


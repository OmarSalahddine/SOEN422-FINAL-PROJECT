#include "hal_Loader.h"
#include "bsl_Uart.h"
#include "COutForAUnit.h"

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define BufferMax 11

static uint8_t byteArray[0];

uint8_t hal_Loader(uint8_t* mem)
{
   uint8_t index = 0;
   char received = bsl_Uart_RxChar();

   uint8_t size = received; 
   
   while(index < size){;
   PutC((char) Ack);
   index++;
   }

}

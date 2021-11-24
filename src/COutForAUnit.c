/* COutForAUnit_task1_todo.c - COut for AUnit
 */

#include <string.h>
#include <stdbool.h>
#include "COutForAUnit.h"

#if !defined(Host)
#include "bsl_Uart.h"
#endif

#define nBuffers 3
#define BufferMax 40

#if !defined(Host)
#define BaudRate9600 ((uint16_t)(103))
#endif

// buffer[0] is the test name.
// buffer[1] is the expected output.
// buffer[2] is the current output.
static char buffer[nBuffers][BufferMax];

static uint8_t n = 0;
static uint8_t bufferNum = 0; // based on the newline detected.
static bool overflow = false;
static uint16_t size = 0;

bool Equals(void)
{
    if (overflow) {return false;}

    for(uint16_t i = 0; i < size; i++){
     if(buffer[1][i] != buffer[2][i]){
      return false;
     }
    }

    return true;
}

void ResetBuffer(void)
{
    for(int i = 0; i < nBuffers; i++){
     for(int j = 0; j < BufferMax; j++){
      buffer[i][j] = (char) 0;
     }
    }

    size = 0;
    bufferNum = 0;
    n = 0;
    overflow = false;
}

static void putBuffer(char c)
{
if(c == '\n'){
 bufferNum++;
 n = 0;
}

uint16_t bufferN = bufferNum%3;

buffer[bufferN][n] = c;

if(bufferN == 2){
 size++;
} 

if(n < BufferMax){n++;}
else {overflow = true;}
}

// AUnit's putchar to store output characters in AUnit's buffers.
static void __putchar(char c)
{
    #if !defined(Host)
    bsl_Uart_TxChar(c);
    putBuffer(c);
    #else
    printf("%c", c);
    #endif
}

#define getchar() bsl_Uart_RxChar()

void PutC(char c)
{ 
    __putchar(c);
}

void PutS(const char *s)
{
    while (*s) PutC(*s++);
}

void PutN(void) { PutC('\n'); }

char GetC(void) {
   #if !defined(Host) 
   return getchar(); 
   #else
   char getchar;
   scanf("%c", &getchar);
   return getchar;
   #endif 
}

/*---------------------------------------------------------------------------
 * PutX4 - PutHexNibble - print a nibble as an hex digit character.
 *-------------------------------------------------------------------------*/
void PutX4(uint8_t n)
{
    n &= 0xF;
    PutC(n>=10 ? (n-10)+'A' : n+'0');
}
/*---------------------------------------------------------------------------
 * PutX8 - PutHexByte - print a byte (uint8_t) as two hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX8(uint8_t b)
{
    PutX4(b >>  4);
    PutX4(b);
}
/*---------------------------------------------------------------------------
 * PutX16 - PutHexWord - print a word (uint16_t) as four hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX16(uint16_t w)
{
    PutX8(w >> 8);
    PutX8(w);
}

/* COutForAUnit_task1_todo.c - COut for AUnit
 */

#include <string.h>
#include "COutForAUnit.h"
#include "bsl_Uart.h"

#define nBuffers 3
#define BufferMax 40
#define BaudRate9600 ((uint16_t)(103))
static bool initialized = false;

// buffer[0] is the test name.
// buffer[1] is the expected output.
// buffer[2] is the current output.
static char buffer[nBuffers][BufferMax];

static uint8_t n = 0;
static uint8_t bufferNum = 0; // based on the newline detected.
static bool overflow = false;
static uint16_t size = 0;

void initialize(void){
initialized = true;
}

bool Equals(void)
{
    if (overflow) {return false;}


    for(uint16_t i = 0; i < size; i++){
    if(buffer[1][i] != buffer[2][i]){return false;}
    }

    return true;
}

void printBuffer(void){
for(int i = 0; i < 3; i++){
 for(int j = 0; j < (sizeof(buffer[i])/sizeof(buffer[i][0])); j++){
    bsl_Uart_TxChar(buffer[i][j]);
  }
 }
}

void ResetBuffer(void)
{
for(int i = 0; i < 3; i++){
 for(int j = 0; j < (sizeof(buffer[i])/sizeof(buffer[i][0])); j++){
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
    bsl_Uart_TxChar(c);
    if(initialized == true){
    putBuffer(c);
    }
}

#define getchar() bsl_Uart_RxChar()

void PutC(char c)
{
    __putchar(c);
}

void PutS(const char *s)
{
    while (*s)
        PutC(*s++);
}
void PutN(void) { PutC('\n'); }

char GetC(void) { return getchar(); }

/*---------------------------------------------------------------------------
 * decToHexa - Converting from decimal to Hexadecimal.
 *-------------------------------------------------------------------------*/
char * decToHexa(uint16_t n)
{
    // char array to store hexadecimal number
    static char hexaDeciNum[100];
 
    // counter for hexadecimal number array
    uint16_t i = 0;
    while (n != 0) {
        // temporary variable to store remainder
        uint16_t temp = 0;
 
        // storing remainder in temp variable.
        temp = n % 16;
 
        // check if temp < 10
        if (temp < 10) {
            hexaDeciNum[i] = temp + 48;
            i++;
        }
        else {
            hexaDeciNum[i] = temp + 55;
            i++;
        }
 
        n = n / 16;
   }
   
   return hexaDeciNum;
}

/*---------------------------------------------------------------------------
 * PutX4 - PutHexNibble - print a nibble as an hex digit character.
 *-------------------------------------------------------------------------*/
void PutX4(uint8_t n)
{
    PutS(decToHexa(n));
}
/*---------------------------------------------------------------------------
 * PutX8 - PutHexByte - print a byte (uint8_t) as two hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX8(uint8_t b)
{
    PutS(decToHexa(b));
}
/*---------------------------------------------------------------------------
 * PutX16 - PutHexWord - print a word (uint16_t) as four hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX16(uint16_t w)
{
    PutS(decToHexa(w));
}

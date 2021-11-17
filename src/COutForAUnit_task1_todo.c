/* COutForAUnit_task1_todo.c - COut for AUnit
*/

#include <string.h>
#include "COutForAUnit.h"
#include "bsl_Usart.h"

#define nBuffers  3
#define BufferMax 40

// buffer[0] is the test name.
// buffer[1] is the expected output.
// buffer[2] is the current output.
static char buffer[nBuffers][BufferMax];

static uint8_t  n = 0;
static uint8_t  bufferNum = 0; // based on the newline detected.
static bool     overflow = false;

bool Equals(void) {
    if (overflow) return false;

    return false;
}

void ResetBuffer(void) {
    // Your code...
}

static void putBuffer(char c) {
    // Your code...
}

// AUnit's putchar to store output characters in AUnit's buffers.
static void __putchar(char c) {
    bsl_Uart_TxChar(c);
    putBuffer(c);
}

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

#define getchar()  bsl_Uart_RxChar()

void PutC(char c)        { __putchar(c); }
void PutS(const char* s) { while (*s) PutC(*s++); }
void PutN(void)          { PutC('\n'); }

char GetC(void)          { return getchar(); }

/*---------------------------------------------------------------------------
 * PutX4 - PutHexNibble - print a nibble as an hex digit character.
 *-------------------------------------------------------------------------*/
void PutX4(uint8_t n) {
    PutS(decToHexa(n));
}
/*---------------------------------------------------------------------------
 * PutX8 - PutHexByte - print a byte (uint8_t) as two hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX8(uint8_t b) {
    PutS(decToHexa(b));
}
/*---------------------------------------------------------------------------
 * PutX16 - PutHexWord - print a word (uint16_t) as four hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX16(uint16_t w) {
    PutS(decToHexa(w));
}

/* COutForAUnit_task1_todo.c - COut for AUnit
*/

#include <string.h>
#include "COutForAUnit.h"
#include "bsl_Uart.h"

#define nBuffers  3
#define BufferMax 40
#define BaudRate9600 ((uint16_t)(103))
static bool initialized;

// buffer[0] is the test name.
// buffer[1] is the expected output.
// buffer[2] is the current output.
static char buffer[nBuffers][BufferMax];

static uint8_t  n = 0;
static uint8_t  bufferNum = 0; // based on the newline detected.
static bool     overflow = false;

void _PutS(const char* s) { while (*s) bsl_Uart_TxChar(*s++); }

bool Equals(void) {
    if (overflow) return false;

    // Comparing expected and current outputs.
    // Your code...

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

#define getchar()  bsl_Uart_RxChar()

void PutC(char c)        { __putchar(c); }
void PutS(const char* s) { while (*s) PutC(*s++); }
void PutN(void)          { PutC('\n'); }

char GetC(void)          { return getchar(); }

/*---------------------------------------------------------------------------
 * decToHexa - Converting from decimal to Hexadecimal.
 *-------------------------------------------------------------------------*/
uint16_t decToHexa(uint16_t n)
{
    // char array to store hexadecimal number
    char hexaDeciNum[100];
 
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
 
    // printing hexadecimal number array in reverse order
    for (uint16_t j = i - 1; j >= 0; j--)
        printf("%c", hexaDeciNum[j]);
}

/*---------------------------------------------------------------------------
 * PutX4 - PutHexNibble - print a nibble as an hex digit character.
 *-------------------------------------------------------------------------*/
void PutX4(uint8_t n) {
    UDR0 = DecToHex(n);
}
/*---------------------------------------------------------------------------
 * PutX8 - PutHexByte - print a byte (uint8_t) as two hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX8(uint8_t b) {
    UDR0 = DecToHex(n);
}
/*---------------------------------------------------------------------------
 * PutX16 - PutHexWord - print a word (uint16_t) as four hex digit characters.
 *-------------------------------------------------------------------------*/
void PutX16(uint16_t w) {
    UDR0 = DecToHex(n);
}

char bsl_Uart_RxChar(void) {

  while(!(UCSR0A & (1 << RXC0))){}

    // return data
    return (char) UDR0;
}

char bsl_Uart_TxChar(char c) {

  while(!(UCSR0A & (1 << UDRE0))){}

    // return data
    UDR0 = c;
}

void bsl_Uart_Init(void) {
	if (!initialized) { // To make sure it will be done only once.

	    // Set baud rate
	    UBRR0 = BaudRate9600;
	    
	    // Turn on the receiver for UART
	    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}}
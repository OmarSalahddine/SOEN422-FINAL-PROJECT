// bsl_UsartByPolling.c
#include "bsl_Uart.h" // Share the same interface (polling vs interrupts).
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include<stdio.h>
#define BaudRate9600 ((uint16_t)(103))
static bool initialized = false;
//------------------------------------------------------------------------
// Receive a character from UART.
//------------------------------------------------------------------------
char bsl_Uart_RxChar() {
/* Wait for data to be received */
while ( !(UCSR0A & (1<<RXC0)) );
/* Get and return received data from buffer */
return UDR0;
}

void bsl_Uart_TxChar(char c) {
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) );
/* Put data into buffer, sends the data */
UDR0 = c;
}

//------------------------------------------------------------------------
// Initialize UART for polling.
//------------------------------------------------------------------------
void bsl_Uart_Init() {
if (!initialized) { // To make sure it will be done only once.
/*Set baud rate */
UBRR0H = (unsigned char)(BaudRate9600>>8);
UBRR0L = (unsigned char)BaudRate9600;
/* Enable read */
UCSR0B = (1<<RXEN0) | (1<<TXEN0);
/* Set frame format: 8 bit, no parity, 1 stop bit*/
UCSR0C = (3<<UCSZ00);
initialized = true;
}
}

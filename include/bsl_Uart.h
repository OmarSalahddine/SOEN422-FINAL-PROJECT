#ifndef __bsl_Uart_h
#define __bsl_Uart_h
#include <avr/io.h>

void bsl_Uart_Init(void);
void bsl_Uart_TxChar(char c);
char bsl_Uart_RxChar(void);
#endif

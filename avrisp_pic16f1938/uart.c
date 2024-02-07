#include "uart.h"

void uart_init(void)
{
    //x = (FOSC/Baud Rate * 64) - 1
    //uint32_t brg = (F_CPU /(64 * BAUD)) - 1;
    
    TRISC |= (1 << 7) | (1 << 6); //RC7, RC6 Input
    TXSTA = (1 << 5) | (1 << 2); //TXEN, BRGH   
    RCSTA = (1 << 7) | (1 << 4); //SPEN, CREN
    SPBRG =  10;// 57600 ??? 16MHz 
    RCIE = 1;
}

void uart_sendChar(uint8_t ch)
{
    while(!TRMT);
    TXREG = ch;
}

void uart_sendString(uint8_t *str)
{
    int i = 0;
    while(str[i] != '\0') uart_sendChar(str[i++]);
}


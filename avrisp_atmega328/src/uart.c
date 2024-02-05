/*
 * uart.c
 *
 *  Created on: 3 февр. 2024 г.
 *      Author: andrej
 */

#include "uart.h"

void uart_init(void)
{
	uint16_t divider = CPU_CLOCK/(16 * BAUDE) - 1;

	UCSR0A = 0x00;
	//UCSR0B = 0x00;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	UCSR0C = ((1 << UCSZ00) | (1 << UCSZ01));

	UBRR0H = ((divider>>8) & 0xFF);
	UBRR0L = (divider & 0xFF);
}

void uart_sendChar(uint8_t tdat)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = tdat;
}

void uart_sendString(uint8_t *tdat)
{
	int chr = 0;
	while(tdat[chr]!=0)
	{
		uart_sendChar(tdat[chr]);
		chr++;
	}
}


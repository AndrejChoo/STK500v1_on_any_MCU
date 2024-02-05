/*
 * main.c
 *
 *  Created on: 3 февр. 2024 г.
 *      Author: andrej
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "spi.h"
#include "progger.h"


// USART Receiver buffer
#define RX_BUFFER_SIZE0 256
uint8_t exchange[RX_BUFFER_SIZE0];
volatile uint8_t rx_wr_index0 = 0, rx_rd_index0 = 0;

extern uint8_t error;
extern uint8_t pmode;


ISR(USART_RX_vect)
{
	uint8_t dat = UDR0;

	if(!(UCSR0A & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))))
	{
		exchange[rx_wr_index0] = dat;
		rx_wr_index0++;
	}
}



int main(void)
{
	setup();
	sei();

	while(1)
	{
		  // is pmode active?
		  if (pmode) LED_PMODE_HIGH
		  else LED_PMODE_LOW

		  // is there an error?
		  if (error) LED_ERR_HIGH
		  else LED_ERR_LOW

		  if(Serial_available()) avrisp();
	}
}

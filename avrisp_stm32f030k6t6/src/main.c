/*
 * main.c
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */

#include "stm32f0xx.h"
#include "rcc.h"
#include "progger.h"

#define RX_BUFFER_SIZE0 256
uint8_t exchange[RX_BUFFER_SIZE0];
volatile uint8_t rx_wr_index0 = 0, rx_rd_index0 = 0;

extern int error;
extern int pmode;


//Прерывание по приёму байта USART
void  USART1_IRQHandler(void)
{
	if (USART1->ISR & USART_ISR_RXNE)
	{
		exchange[rx_wr_index0++] = (USART1->RDR);
	}
}


int main(void)
{

	clk_init();
	setup();



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

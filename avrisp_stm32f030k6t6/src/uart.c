/*
 * uart.c
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */

#include "uart.h"

void uart_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER &= ~((GPIO_MODER_MODER2)|(GPIO_MODER_MODER3));
	GPIOA->MODER |= GPIO_MODER_MODER2_1|GPIO_MODER_MODER3_1;
	GPIOA->OTYPER |= GPIO_OTYPER_OT_2|GPIO_OTYPER_OT_3;
	GPIOA->OSPEEDR &= ~((GPIO_OSPEEDER_OSPEEDR2)|(GPIO_OSPEEDER_OSPEEDR3));
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_1|GPIO_OSPEEDER_OSPEEDR3_1;

	GPIOA->AFR[0] |= (1<<((2-0)*4));
	GPIOA->AFR[0] |= (1<<((3-0)*4));

	USART1->CR1 = 0x0000002C;
	USART1->CR2 = 0x00000000;
	USART1->BRR = 0x000001A1;
	USART1->CR1 |= USART_CR1_UE;

	NVIC_EnableIRQ(USART1_IRQn);
}

void uart_sendChar(uint8_t tdat)
{
	USART1->TDR = tdat;
	while(!(USART1->ISR & USART_ISR_TXE));
}

void uart_sendString(uint8_t *tdat)
{
	int i=0;
	while(tdat[i] != '\0') uart_sendChar(tdat[i++]);
}

/*
 * delay.c
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */
#include "delay.h"


void delay_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	TIM14->CR1 |= TIM_CR1_OPM;
}

void _delay_ms(int dms)
{
	TIM14->CNT = 0;
	TIM14->PSC = 48000 - 1;
	TIM14->ARR = dms;
	TIM14->CR1 |= TIM_CR1_CEN;
	while(!(TIM14->SR & TIM_SR_UIF));
	TIM14->SR &= ~TIM_SR_UIF;
}

void _delay_us(int dus)
{
	TIM14->CNT = 0;
	TIM14->PSC = 48 - 1;
	TIM14->ARR = dus;
	TIM14->CR1 |= TIM_CR1_CEN;
	while(!(TIM14->SR & TIM_SR_UIF));
	TIM14->SR &= ~TIM_SR_UIF;
}

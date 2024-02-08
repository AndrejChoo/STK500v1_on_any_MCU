/*
 * rcc.c
 *
 *  Created on: 8 ����. 2024 �.
 *      Author: Andrej
 */

#include "rcc.h"


void clk_init(void)
{
	//����������� ��������� PLL_MULL
	RCC->CFGR = 0x00000000;
	RCC->CFGR |= (10<<18);
	//��������� PLL
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	//����������� ������������ ���� APB1
	RCC->CFGR2 |= (4<<8);
	//����������� Flash Latency
	FLASH->ACR |= 1;
	//����������� �������� ������������ SYS_CLK
	RCC->CFGR |= 2; //PLL
	//��������� ������������ ���������
}

/*
 * rcc.c
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */

#include "rcc.h"


void clk_init(void)
{
	//Настраиваем множитель PLL_MULL
	RCC->CFGR = 0x00000000;
	RCC->CFGR |= (10<<18);
	//Запускаем PLL
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	//Настраиваем предделитель шины APB1
	RCC->CFGR2 |= (4<<8);
	//Настраиваем Flash Latency
	FLASH->ACR |= 1;
	//Настраиваем источник тактирования SYS_CLK
	RCC->CFGR |= 2; //PLL
	//Настройка тактирования завершена
}

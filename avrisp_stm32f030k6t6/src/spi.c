/*
 * spi.c
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */

#include "spi.h"

void spi_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHBENR |=  RCC_AHBENR_GPIOAEN;

	GPIOA->MODER &= ~((GPIO_MODER_MODER5)|(GPIO_MODER_MODER6)|(GPIO_MODER_MODER7));
	GPIOA->MODER |= GPIO_MODER_MODER5_1|GPIO_MODER_MODER6_1|GPIO_MODER_MODER7_1;
	GPIOA->OTYPER &= ~((GPIO_OTYPER_OT_5)|(GPIO_OTYPER_OT_6)|(GPIO_OTYPER_OT_7));
	GPIOA->OSPEEDR &= ~((GPIO_OSPEEDER_OSPEEDR5)|(GPIO_OSPEEDER_OSPEEDR6)|(GPIO_OSPEEDER_OSPEEDR7));
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_1|GPIO_OSPEEDER_OSPEEDR6_1|GPIO_OSPEEDER_OSPEEDR7_1;

	GPIOA->AFR[0] |= (0<<((5-0)*4));
	GPIOA->AFR[0] |= (0<<((6-0)*4));
	GPIOA->AFR[0] |= (0<<((7-0)*4));


	SPI1->CR1 |= SPI_CR1_MSTR |SPI_CR1_SSI | SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_BR; SPI1->CR1 &= ~SPI_CR1_BR_1;
	SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
	SPI1->CR2 &= ~SPI_CR2_DS_3;
	SPI1->CR2 |= SPI_CR2_FRXTH;
	SPI1->CR1 |= SPI_CR1_SPE;

}

void spi_deinit(void)
{
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

uint8_t SPI_transfer(uint8_t spdat)
{
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1_DR_8bit = spdat;
	while(!(SPI1->SR & SPI_SR_RXNE));
	return SPI1_DR_8bit;
}





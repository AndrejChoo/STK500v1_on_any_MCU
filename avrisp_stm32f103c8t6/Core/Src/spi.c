/*
 * spi.c
 *
 *  Created on: 4 февр. 2024 г.
 *      Author: Andrej
 */

#include "spi.h"

void spi_init(uint8_t speed)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	//MOSI, SCK
	GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_CNF5); //
	GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_CNF5_1; //PA7(MOSI), PA5(SCK), PA4(NSS) - AF, Output, PP
	GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_MODE5; // MOSI, SCK, NSS - 50MHz

	GPIOA->CRL &= ~GPIO_CRL_MODE6; //PA6(MISO) - Input
	GPIOA->CRL &= ~GPIO_CRL_CNF6_0;
	GPIOA->CRL |= GPIO_CRL_CNF6_1; //PA6(MISO) - Input floating

	if(speed > 7) speed = speed % 7;

	//SPI1->CR1 &= ~(7 << 3);
	SPI1->CR1 |= SPI_CR1_BR;

	SPI1->CR1 &= ~SPI_CR1_CPOL;             //Polarity cls signal CPOL = 0;
	SPI1->CR1 &= ~SPI_CR1_CPHA;             //Phase cls signal    CPHA = 0;
	SPI1->CR1 &= ~SPI_CR1_DFF;               //8 bit data
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;         //MSB will be first
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;  //Software slave management & Internal slave select
	SPI1->CR1 |= SPI_CR1_MSTR;              //Mode Master
	SPI1->CR1 |= SPI_CR1_SPE;                //Enable SPI2
}

void spi_deinit(void)
{
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

uint8_t SPI_transfer(uint8_t spdat)
{
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = spdat;
	while(!(SPI1->SR & SPI_SR_RXNE));
	return SPI1->DR;
}

/*
 * spi.c
 *
 *  Created on: 3 февр. 2024 г.
 *      Author: andrej
 */

#include "spi.h"


void spi_init(uint8_t speed, uint8_t s2x)
{
	if(speed > 3) speed = speed % 3;

	//PULL UP SS pin
	DDRB &= ~(1 << 2);
	PORTB |= (1 << 2);
	DDRB |= (1 << 3) | (1 << 5);
	DDRB &= ~((1 << 4) | (1 << 2));

	if(s2x) SPSR |= (1 << 0);
	else SPSR &= ~(1 << 0);

	SPCR = speed;
	SPCR |= (1 << MSTR) | (1 << SPE);

}

void spi_deinit(void)
{
	SPCR &= ~(1 << 6);
}

uint8_t SPI_transfer(uint8_t spdat)
{
	SPDR = spdat;
	while(!(SPSR & (1 << 7)));
	return(SPDR);
}

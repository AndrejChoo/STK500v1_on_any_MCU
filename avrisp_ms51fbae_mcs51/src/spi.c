#include "spi.h"
#include "MS51_16K.h"

void spi_init(void)
{
	P0M1&=0xFE; P0M2|=0x01;   //MOSI PUSH_PULL P00
	P0M1|=0x02; P0M2&=0xFD;   //MISO INPUT     P01
	P1M1&=0xFE; P1M2|=0x01;   //SCLK PUSH_PULL P10
    
    SPSR |= (1 << 3); //DISMODF SS - GPIO
	//SSOE - SPIEN - LSBFE - MSTR - CHPOL - CHPA - SPR1 - SPR0
    SFRS = 0x00;
	SPCR = (1 << 4);  
    SPCR |= (1 << 6); 
}

void spi_deinit(void)
{
    SPCR &= ~(1 << 6); 
}

void spi_setSpeed(char speed)
{
	SPCR &= ~0x03;
	SPCR |= (speed & 0x03);
}


char SPI_transfer(char dat)
{  
	SPSR &= ~(1 << 7);
	SPDR = dat;
	while(!(SPSR & (1 << 7)));
	return SPDR;    
}


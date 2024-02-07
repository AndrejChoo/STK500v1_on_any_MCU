#include "spi.h"

#define _XTAL_FREQ 20000000

void spi_init(void)
{
    TRISC5 = 0; // SDO -> Output
    TRISC4 = 1; // SDI -> Input
    TRISC3 = 0; // SCK -> Output
    
    //SSPSTAT = 0x00;
    //SSPCON1 = 0b00100010;  
}

uint8_t SPI_transfer(uint8_t dat)
{
    /*
    SSPBUF = dat;
    while(!(SSPSTAT & (1 << 0))); //While !BF
    SSPSTAT &= ~(1 << 0); //Clear BF
    return SSPBUF;
    */ 
    uint8_t rslt = 0;
    
    for (unsigned int i = 0; i < 8; i++) 
    {
        if(dat & (1 << (7 - i))) MOSI_HIGH
        else MOSI_LOW
        SCK_HIGH
        __delay_us(20);
        if(PORTC & (1 << 4)) rslt |= (1 << (7 - i));
        SCK_LOW // slow pulse
        __delay_us(20);
    }
    MOSI_LOW
    return rslt;
    
}

void spi_deinit(void)
{
    SSPCON1 = 0x02;
    /*
    TRISC5 = 1; // SDO -> Output
    TRISC4 = 1; // SDI -> Input
    TRISC3 = 1; // SCK -> Output
    */
}

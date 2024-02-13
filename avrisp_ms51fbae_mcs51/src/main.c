#include "MS51_16K.h"
#include "delay.h"
#include "usart.h"
#include "spi.h"
#include "sys.h"
#include "programmer.h"

#define RX_BUFFER_SIZE0 256
xdata volatile unsigned char exchange[RX_BUFFER_SIZE0];
xdata unsigned char rx_rd_index0 = 0;
xdata volatile unsigned char rx_wr_index0 = 0;

extern xdata int error;
extern xdata int pmode;


void uart0 (void) interrupt 4 using 1  
{
    if(RI == 1)
    { 
        exchange[rx_wr_index0++] = SBUF; 
        
        RI = 0;
    }
    if(TI == 1)
    {
        TI = 0;
    }
}

void main(void)
{   
    MODIFY_HIRC(HIRC_24);
    
    setup();
    delay_init();
    spi_init();
    

    EA = 1;
    
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
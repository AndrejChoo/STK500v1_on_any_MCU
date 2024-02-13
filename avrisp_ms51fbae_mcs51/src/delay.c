#include "MS51_16K.h"
#include "delay.h"



void delay_init(void)
{
   CKCON = 0x00;
}


void _delay_ms(unsigned int dl)
{
    while(dl)
    {
        TH0 = 0xC1;
        TL0 = 0x7F;
        TR0 = 1;
        while(!TF0);
        TR0 = 0;
        TF0 = 0;
        dl--;
    }
}

void _delay_us(unsigned int dl)
{
    while(dl)
    {
        TH0 = 0xFF;
        TL0 = 0xF5;
        TR0 = 1;
        while(!TF0);
        TR0 = 0;
        TF0 = 0;
        dl--;
    }
}
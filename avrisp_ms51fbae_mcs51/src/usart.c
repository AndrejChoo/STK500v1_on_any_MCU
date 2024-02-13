#include "usart.h"
#include "MS51_16K.h"

void usart_init(void)
{
	P0M1&=0xBF; P0M2&=0xBF;    //Setting UART pin as Quasi mode for transmit
	P0M1|=0x80; P0M2&=0x7F;   //Setting UART pin as input mode for recieve    
	
    SCON = 0x50; //UART0 Mode1,REN=1,TI=1
    PCON |= 0x80; //UART0 Double Rate Enable
    T3CON &= 0xF8; //(Prescale=1)
    T3CON |= 0x20; //UART0 baud rate clock source = Timer3
    RH3 = 0xFF;
    RL3 = 0xF3;   //115200 on 24MHz
    T3CON|= 0x08; //Trigger Timer3
    
    IE |= (1 << 4);
}


void uart_sendChar(char ch)
{
	TI = 0;
	SBUF = ch;
	while(TI == 0);
}


void uart_sendString(char *str)
{
	int i = 0;
	while(str[i] != '\0')
	{
		TI = 0;
		SBUF = str[i];
		while(TI == 0);
		i++;
	}
}

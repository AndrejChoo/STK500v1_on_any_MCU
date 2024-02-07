/*
 * File:   main.c
 * Author: Andrej
 *
 * Created on 5 ??????? 2024 ?., 21:39
 */
// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection (ECH, External Clock, High Power Mode (4-32 MHz): device clock supplied to CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable (All VCAP pin functionality is disabled)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


#include <xc.h>
#include "spi.h"
#include "uart.h"
#include "progger.h"

#define _XTAL_FREQ 20000000

// USART Receiver buffer
#define RX_BUFFER_SIZE0 256
uint8_t exchange[RX_BUFFER_SIZE0];
volatile uint8_t rx_wr_index0 = 0, rx_rd_index0 = 0;

extern int errors;
extern int pmode;


void __interrupt() ISR(void)
{
  if (RCIF == 1)
  {
    exchange[rx_wr_index0] = RCREG;         	
    rx_wr_index0++;
  }
}


void main(void) 
{
    setup();
    
    INTCON |= (1 << 7)|(1 << 6); //GIE, PEIE Enable
    
    while(1)
    {
        // is pmode active?
		if (pmode) LED_PMODE_HIGH
		else LED_PMODE_LOW

		// is there an error?
		if (errors) LED_ERR_HIGH
		else LED_ERR_LOW

		if(Serial_available()) avrisp();
    }
}

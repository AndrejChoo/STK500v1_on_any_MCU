/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/
#include "ch32v20x.h"
#include "uart.h"
#include "delay.h"
#include "progger.h"
#include "spi.h"

/* Global typedef */

/* Global define */
#define RX_BUFFER_SIZE0 256
/* Global Variable */

uint8_t exchange[RX_BUFFER_SIZE0];
volatile uint8_t rx_wr_index0 = 0, rx_rd_index0 = 0;

extern int error;
extern int pmode;
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        exchange[rx_wr_index0++] = USART1->DATAR;
    }
}


int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    uart_init();
    delay_init();
    spi_init();
    setup();

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

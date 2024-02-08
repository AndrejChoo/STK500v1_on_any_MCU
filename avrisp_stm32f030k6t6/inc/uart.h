/*
 * uart.h
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f0xx.h"


void uart_init(void);
void uart_sendChar(uint8_t tdat);
void uart_sendString(uint8_t *tdat);

#endif /* UART_H_ */

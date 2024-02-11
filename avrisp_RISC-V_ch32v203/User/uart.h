/*
 * uart.h
 *
 *  Created on: Feb 10, 2024
 *      Author: Andrej
 */

#ifndef USER_UART_H_
#define USER_UART_H_

#include "ch32v20x.h"



void uart_init(void);
void uart_sendChar(uint8_t tdat);
void uart_sendString(uint8_t *tdat);

#endif /* USER_UART_H_ */

/*
 * uart.h
 *
 *  Created on: 3 февр. 2024 г.
 *      Author: andrej
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <avr/io.h>

#define CPU_CLOCK		16000000UL
#define BAUDE			57600UL

void uart_init(void);
void uart_sendChar(uint8_t tdat);
void uart_sendString(uint8_t *tdat);

#endif /* INC_UART_H_ */

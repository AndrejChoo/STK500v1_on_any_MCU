/*
 * delay.h
 *
 *  Created on: Feb 10, 2024
 *      Author: Andrej
 */

#ifndef USER_DELAY_H_
#define USER_DELAY_H_

#include "ch32v20x.h"

void delay_init(void);
void _delay_us(uint32_t n);
void _delay_ms(uint32_t n);

#endif /* USER_DELAY_H_ */

/*
 * delay.h
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f0xx.h"

void delay_init(void);
void _delay_ms(int dms);
void _delay_us(int dus);

#endif /* DELAY_H_ */

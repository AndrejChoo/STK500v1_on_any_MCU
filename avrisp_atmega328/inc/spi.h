/*
 * spi.h
 *
 *  Created on: 3 февр. 2024 г.
 *      Author: andrej
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include <avr/io.h>

void spi_init(uint8_t speed, uint8_t s2x);
void spi_deinit(void);
uint8_t SPI_transfer(uint8_t spdat);

#endif /* INC_SPI_H_ */

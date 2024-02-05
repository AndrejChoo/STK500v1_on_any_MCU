/*
 * spi.h
 *
 *  Created on: 4 февр. 2024 г.
 *      Author: Andrej
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "stm32f1xx_hal.h"

void spi_init(uint8_t speed);
void spi_deinit(void);
uint8_t SPI_transfer(uint8_t spdat);

#endif /* INC_SPI_H_ */

/*
 * spi.h
 *
 *  Created on: 8 февр. 2024 г.
 *      Author: Andrej
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f0xx.h"

#define SPI1_DR_8bit          (*(__IO uint8_t *)((uint32_t)&(SPI1->DR)))


void spi_init(void);
void spi_deinit(void);
uint8_t SPI_transfer(uint8_t spdat);

#endif /* SPI_H_ */

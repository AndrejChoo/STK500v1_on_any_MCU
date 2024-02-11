/*
 * spi.h
 *
 *  Created on: Feb 10, 2024
 *      Author: Andrej
 */

#ifndef USER_SPI_H_
#define USER_SPI_H_

#include "ch32v20x.h"
#include "delay.h"

#define SPI_HARD

#define SCK_PERIOD      80

#define MOSI_PIN        GPIO_Pin_7
#define MISO_PIN        GPIO_Pin_6
#define SCK_PIN         GPIO_Pin_5

#define MOSI_LOW        GPIO_WriteBit(GPIOA, MOSI_PIN, 0);
#define MOSI_HIGH       GPIO_WriteBit(GPIOA, MOSI_PIN, 1);
#define SCK_LOW         GPIO_WriteBit(GPIOA, SCK_PIN, 0);
#define SCK_HIGH        GPIO_WriteBit(GPIOA, SCK_PIN, 1);

void spi_init(void);
void spi_deinit(void);
uint8_t SPI_transfer(uint8_t spdat);

#endif /* USER_SPI_H_ */

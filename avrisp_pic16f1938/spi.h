/* 
 * File:   spi.h
 * Author: Andrej
 *
 * Created on 28 ?????? 2023 ?., 21:17
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
   
#define MOSI_HIGH       PORTC |= (1 << 5);
#define MOSI_LOW        PORTC &= ~(1 << 5);
#define SCK_HIGH        PORTC |= (1 << 3);
#define SCK_LOW         PORTC &= ~(1 << 3);
    
void spi_init(void);
uint8_t SPI_transfer(uint8_t dat);
void spi_deinit(void);


#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */


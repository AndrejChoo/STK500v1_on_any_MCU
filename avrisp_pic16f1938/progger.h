/*
 * progger.h
 *
 *  Created on: 3 февр. 2024 г.
 *      Author: andrej
 */

#ifndef INC_PROGGER_H_
#define INC_PROGGER_H_

#include <xc.h>
#include "uart.h"
#include "spi.h"

#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

// STK Definitions
#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20 //ok it is a space...

//Pins definition
#define LED_HB			1
#define LED_ERR			2
#define LED_PMODE		3


#define RESET_PIN         1
#define LED_HB_PIN        2
#define LED_ERR_PIN       3
#define LED_PMODE_PIN     4

#define RESET_DDR			TRISC
#define LED_HB_DDR			TRISA
#define LED_ERR_DDR			TRISA
#define LED_PMODE_DDR		TRISA

#define RESET_PORT			PORTC
#define LED_HB_PORT			PORTA
#define LED_ERR_PORT		PORTA
#define LED_PMODE_PORT		PORTA

#define RESET_HIGH			RESET_PORT |= (1 << RESET_PIN);
#define LED_HB_HIGH			LED_HB_PORT |= (1 << LED_HB_PIN);
#define LED_ERR_HIGH         LED_ERR_PORT |= (1 << LED_ERR_PIN);
#define LED_PMODE_HIGH		LED_PMODE_PORT |= (1 << LED_PMODE_PIN);

#define RESET_LOW			RESET_PORT &= ~(1 << RESET_PIN);
#define LED_HB_LOW			LED_HB_PORT &= ~(1 << LED_HB_PIN);
#define LED_ERR_LOW		LED_ERR_PORT &= ~(1 << LED_ERR_PIN);
#define LED_PMODE_LOW		LED_PMODE_PORT &= ~(1 << LED_PMODE_PIN);


#define beget16(addr) (*addr * 256 + *(addr+1) )

typedef struct param {
  uint8_t devicecode;
  uint8_t revision;
  uint8_t progtype;
  uint8_t parmode;
  uint8_t polling;
  uint8_t selftimed;
  uint8_t lockbytes;
  uint8_t fusebytes;
  uint8_t flashpoll;
  uint16_t eeprompoll;
  uint16_t pagesize;
  uint16_t eepromsize;
  uint32_t flashsize;
}
parameter;

parameter param;

uint8_t Serial_available(void);
void setup(void);
void pulse(int pin, int times);

// Function definition
void avrisp(void);
void read_signature(void);
void read_page(void);
char eeprom_read_page(int length);
char flash_read_page(int length);
uint8_t flash_read(uint8_t hilo, unsigned int addr);
void program_page(void);
uint8_t write_eeprom_chunk(unsigned int start, unsigned int length);
uint8_t write_eeprom(unsigned int length);
uint8_t write_flash_pages(int length);
void write_flash(int length);
unsigned int current_page(void);
void commit(unsigned int addr);
void flash(uint8_t hilo, unsigned int addr, uint8_t data);
void universal(void);
void end_pmode(void);
void start_pmode(void);
void set_parameters(void);
void get_version(uint8_t c);
void breply(uint8_t b);
void empty_reply();
uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
void prog_lamp(int state);
void pulse(int pin, int times);
void fill(int n);
uint8_t getch(void);
void reset_target(uint8_t reset);
void heartbeat(void);
// End of Function definition

#endif /* INC_PROGGER_H_ */

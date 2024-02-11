/*
 * progger.c
 *
 *  Created on: Feb 10, 2024
 *      Author: Andrej
 */

#include "progger.h"

#define PROG_FLICKER    1
#define PTIME 30
#define EECHUNK (32)

int error = 0;
int pmode = 0;
// address for reading and writing, set by 'U' command
unsigned int here;

extern uint8_t exchange[];
extern volatile uint8_t rx_wr_index0, rx_rd_index0;

uint8_t buff[256]; // global block storage

static uint8_t rst_active_high;

parameter param;


void setup(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = LED_HB_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_HB_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_ERR_PIN;
    GPIO_Init(LED_ERR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_PMODE_PIN;
    GPIO_Init(LED_PMODE_PORT, &GPIO_InitStructure);

    pulse(LED_PMODE, 2);
    pulse(LED_ERR, 2);
    pulse(LED_HB, 2);

    spi_init();
}

void pulse(int pin, int tmes)
{
    /*
    do
    {
        if(pin == LED_PMODE)
            {
                LED_PMODE_HIGH
                _delay_ms(30);
                LED_PMODE_LOW
                _delay_ms(30);
            }
        if(pin == LED_ERR)
            {
                LED_ERR_HIGH
                _delay_ms(30);
                LED_ERR_LOW
                _delay_ms(30);
            }
        if(pin == LED_HB)
            {
                LED_HB_HIGH
                _delay_ms(30);
                LED_HB_LOW
                _delay_ms(30);
            }
    }
    while(tmes--);
    */
}

void reset_target(uint8_t reset)
{
    if((reset && rst_active_high) || (!reset && !rst_active_high)) RESET_HIGH
    else RESET_LOW
}

uint8_t Serial_available(void)
{
    if(rx_wr_index0 != rx_rd_index0) return 1;
    return 0;
}

uint8_t getch(void)
{
    uint8_t temp;
    while(!(Serial_available()));
    temp = exchange[rx_rd_index0];
    rx_rd_index0++;
    return(temp);
}

void fill(int n)
{
  for (int x = 0; x < n; x++) buff[x] = getch();
}

void prog_lamp(int state)
{
    if (PROG_FLICKER)
    {
        if(state) LED_PMODE_HIGH
        else LED_PMODE_LOW
    }
}

uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  SPI_transfer(a);
  SPI_transfer(b);
  SPI_transfer(c);
  return SPI_transfer(d);
}

void empty_reply(void)
{
  if (CRC_EOP == getch())
  {
      uart_sendChar(STK_INSYNC);
      uart_sendChar(STK_OK);
  }
  else
  {
    error++;
    uart_sendChar(STK_NOSYNC);
  }
}

void breply(uint8_t b)
{
  if (CRC_EOP == getch())
  {
      uart_sendChar(STK_INSYNC);
      uart_sendChar(b);
      uart_sendChar(STK_OK);
  }
  else
  {
      error++;
      uart_sendChar(STK_NOSYNC);
  }
}

void get_version(uint8_t c)
{
  switch (c) {
    case 0x80:
      breply(HWVER);
      break;
    case 0x81:
      breply(SWMAJ);
      break;
    case 0x82:
      breply(SWMIN);
      break;
    case 0x93:
      breply('S'); // serial programmer
      break;
    default:
      breply(0);
  }
}

void set_parameters() {
  // call this after reading parameter packet into buff[]
  param.devicecode = buff[0];
  param.revision   = buff[1];
  param.progtype   = buff[2];
  param.parmode    = buff[3];
  param.polling    = buff[4];
  param.selftimed  = buff[5];
  param.lockbytes  = buff[6];
  param.fusebytes  = buff[7];
  param.flashpoll  = buff[8];
  // ignore buff[9] (= buff[8])
  // following are 16 bits (big endian)
  param.eeprompoll = beget16(&buff[10]);
  param.pagesize   = beget16(&buff[12]);
  param.eepromsize = beget16(&buff[14]);

  // 32 bits flashsize (big endian)
  param.flashsize = buff[16] * 0x01000000
                    + buff[17] * 0x00010000
                    + buff[18] * 0x00000100
                    + buff[19];

  // AVR devices have active low reset, AT89Sx are active high
  rst_active_high = (param.devicecode >= 0xe0);
}

void start_pmode() {

  // Reset target before driving PIN_SCK or PIN_MOSI

  // SPI.begin() will configure SS as output, so SPI master mode is selected.
  // We have defined RESET as pin 10, which for many Arduinos is not the SS pin.
  // So we have to configure RESET as output here,
  // (reset_target() first sets the correct level)
  reset_target(1);

  spi_init();
  //Reset pin
      GPIO_InitTypeDef  GPIO_InitStructure = {0};

      GPIO_InitStructure.GPIO_Pin = RESET_PIN;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
      GPIO_Init(RESET_PORT, &GPIO_InitStructure);
  // See AVR datasheets, chapter "SERIAL_PRG Programming Algorithm":

  // Pulse RESET after PIN_SCK is low:
  //digitalWrite(PIN_SCK, LOW);
  _delay_ms(20); // discharge PIN_SCK, value arbitrarily chosen
  reset_target(0);
  // Pulse must be minimum 2 target CPU clock cycles so 100 usec is ok for CPU
  // speeds above 20 KHz
  _delay_us(100);
  reset_target(1);

  // Send the enable programming command:
  _delay_ms(50); // datasheet: must be > 20 msec
  spi_transaction(0xAC, 0x53, 0x00, 0x00);
  pmode = 1;
}

void end_pmode(void)
{
    spi_deinit();
    // We're about to take the target out of reset so configure SPI pins as input
    reset_target(1);
    //Reset deinit
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = RESET_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(RESET_PORT, &GPIO_InitStructure);

    pmode = 0;
}

void universal(void)
{
  uint8_t ch;

  fill(4);
  ch = spi_transaction(buff[0], buff[1], buff[2], buff[3]);
  breply(ch);
}

void flash(uint8_t hilo, unsigned int addr, uint8_t data)
{
  spi_transaction(0x40 + 8 * hilo,
                  addr >> 8 & 0xFF,
                  addr & 0xFF,
                  data);
}

void commit(unsigned int addr)
{
  if (PROG_FLICKER)
  {
    prog_lamp(0);
  }
  spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
  if (PROG_FLICKER)
  {
    for(int i=0; i<PTIME; i++) _delay_ms(1);
    prog_lamp(1);
  }
}

unsigned int current_page(void)
{
  if (param.pagesize == 32)
  {
    return here & 0xFFFFFFF0;
  }
  if (param.pagesize == 64)
  {
    return here & 0xFFFFFFE0;
  }
  if (param.pagesize == 128)
  {
    return here & 0xFFFFFFC0;
  }
  if (param.pagesize == 256)
  {
    return here & 0xFFFFFF80;
  }
  return here;
}

void write_flash(int length)
{
  fill(length);
  if (CRC_EOP == getch())
  {
      uart_sendChar(STK_INSYNC);
      uart_sendChar(write_flash_pages(length));
  }
  else
  {
      error++;
      uart_sendChar(STK_NOSYNC);
  }
}

uint8_t write_flash_pages(int length)
{
  int x = 0;
  unsigned int page = current_page();
  while (x < length)
  {
    if (page != current_page())
    {
      commit(page);
      page = current_page();
    }
    flash(0, here, buff[x++]);
    flash(1, here, buff[x++]);
    here++;
  }

  commit(page);

  return STK_OK;
}

uint8_t write_eeprom(unsigned int length)
{
  // here is a word address, get the byte address
  unsigned int start = here * 2;
  unsigned int remaining = length;
  if (length > param.eepromsize)
  {
    error++;
    return STK_FAILED;
  }
  while (remaining > EECHUNK)
  {
    write_eeprom_chunk(start, EECHUNK);
    start += EECHUNK;
    remaining -= EECHUNK;
  }
  write_eeprom_chunk(start, remaining);
  return STK_OK;
}

uint8_t write_eeprom_chunk(unsigned int start, unsigned int length)
{
  // this writes byte-by-byte, page writing may be faster (4 bytes at a time)
  fill(length);
  prog_lamp(0);
  for (unsigned int x = 0; x < length; x++)
  {
    unsigned int addr = start + x;
    spi_transaction(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, buff[x]);
    _delay_us(45);
  }
  prog_lamp(1);
  return STK_OK;
}

void program_page()
{
  char result = (char) STK_FAILED;
  unsigned int length = 256 * getch();
  length += getch();
  char memtype = getch();
  // flash memory @here, (length) bytes
  if (memtype == 'F')
  {
    write_flash(length);
    return;
  }
  if (memtype == 'E')
  {
    result = (char)write_eeprom(length);
    if (CRC_EOP == getch())
    {
        uart_sendChar(STK_INSYNC);
        uart_sendChar(result);
    }
    else
    {
      error++;
      uart_sendChar(STK_NOSYNC);
    }
    return;
  }
  uart_sendChar(STK_FAILED);
  return;
}

uint8_t flash_read(uint8_t hilo, unsigned int addr)
{
  return spi_transaction(0x20 + hilo * 8,
                         (addr >> 8) & 0xFF,
                         addr & 0xFF,
                         0);
}

char flash_read_page(int length)
{
  for (int x = 0; x < length; x += 2)
  {
    uint8_t low = flash_read(0, here);
    uart_sendChar(low);
    uint8_t high = flash_read(1, here);
    uart_sendChar(high);
    here++;
  }
  return STK_OK;
}

char eeprom_read_page(int length)
{
  // here again we have a word address
  int start = here * 2;
  for (int x = 0; x < length; x++)
  {
    int addr = start + x;
    uint8_t ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
    uart_sendChar(ee);
  }
  return STK_OK;
}

void read_page()
{
  char result = (char)STK_FAILED;
  int length = 256 * getch();
  length += getch();
  char memtype = getch();
  if (CRC_EOP != getch())
  {
    error++;
    uart_sendChar(STK_NOSYNC);
    return;
  }
  uart_sendChar(STK_INSYNC);
  if (memtype == 'F') result = flash_read_page(length);
  if (memtype == 'E') result = eeprom_read_page(length);
  uart_sendChar(result);
}

void read_signature(void)
{
  if (CRC_EOP != getch())
  {
    error++;
    uart_sendChar(STK_NOSYNC);
    return;
  }
  uart_sendChar(STK_INSYNC);
  uint8_t high = spi_transaction(0x30, 0x00, 0x00, 0x00);
  uart_sendChar(high);
  uint8_t middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  uart_sendChar(middle);
  uint8_t low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  uart_sendChar(low);
  uart_sendChar(STK_OK);
}



void avrisp(void)
{
  uint8_t ch = getch();
  switch (ch)
  {
    case '0': // signon
    {
      error = 0;
      empty_reply();
      break;
    }
    case '1':
    {
      if (getch() == CRC_EOP)
      {
          uart_sendChar(STK_INSYNC);
          uart_sendString("AVR ISP");
          uart_sendChar(STK_OK);
      }
      else
      {
        error++;
        uart_sendChar(STK_NOSYNC);
      }
      break;
    }
    case 'A':
    {
      get_version(getch());
      break;
    }
    case 'B':
    {
      fill(20);
      set_parameters();
      empty_reply();
      break;
    }
    case 'E': // extended parameters - ignore for now
    {
      fill(5);
      empty_reply();
      break;
    }
    case 'P':
    {
      if (!pmode) start_pmode();
      empty_reply();
      break;
    }
    case 'U': // set address (word)
    {
      here = getch();
      here += 256 * getch();
      empty_reply();
      break;
    }
    case 0x60: //STK_PROG_FLASH
    {
      getch(); // low addr
      getch(); // high addr
      empty_reply();
      break;
    }
    case 0x61: //STK_PROG_DATA
    {
      getch(); // data
      empty_reply();
      break;
    }
    case 0x64: //STK_PROG_PAGE
    {
      program_page();
      break;
    }
    case 0x74: //STK_READ_PAGE 't'
    {
      read_page();
      break;
    }
    case 'V': //0x56
    {
      universal();
      break;
    }
    case 'Q': //0x51
    {
      error = 0;
      end_pmode();
      empty_reply();
      break;
    }
    case 0x75: //STK_READ_SIGN 'u'
    {
      read_signature();
      break;
    }
    // expecting a command, not CRC_EOP
    // this is how we can get back in sync
    case CRC_EOP:
    {
      error++;
      uart_sendChar(STK_NOSYNC);
      break;
    }
    // anything else we will return STK_UNKNOWN
    default:
    {
      error++;
      if (CRC_EOP == getch()) uart_sendChar(STK_UNKNOWN);
      else uart_sendChar(STK_NOSYNC);
      break;
    }
  }
}


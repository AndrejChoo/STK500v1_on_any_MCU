#include "MS51_16K.h"
#include "usart.h"
#include "spi.h"
#include "programmer.h"
#include "delay.h"



#define PROG_FLICKER 	1
#define PTIME 30
#define EECHUNK (32)

typedef xdata struct param {
  unsigned char devicecode;
  unsigned char revision;
  unsigned char progtype;
  unsigned char parmode;
  unsigned char polling;
  unsigned char selftimed;
  unsigned char lockbytes;
  unsigned char fusebytes;
  unsigned char flashpoll;
  unsigned int eeprompoll;
  unsigned int pagesize;
  unsigned int eepromsize;
  unsigned long flashsize;
}
parameter;

parameter param;

xdata int error = 0;
xdata int pmode = 0;
// address for reading and writing, set by 'U' command
xdata unsigned int here;

extern xdata volatile unsigned char exchange[];
extern xdata unsigned char rx_rd_index0;
extern xdata volatile unsigned char rx_wr_index0;

xdata unsigned char buff[256]; // global block storage

static xdata unsigned char rst_active_high;


void setup(void)
{
	usart_init();
    
    spi_setSpeed(3);

	//P2M0 |= 0x1c; P2M1 &= ~0x1c; //
	pulse(LED_PMODE, 2);
	pulse(LED_ERR, 2);
	pulse(LED_HB, 2);

}

void pulse(int pin, int tmes)
{
	do
	{
		if(pin == LED_PMODE)
			{
				LED_PMODE_PORT |= (1 << LED_PMODE_PIN);
				_delay_ms(30);
				LED_PMODE_PORT &= ~(1 << LED_PMODE_PIN);
				_delay_ms(30);
			}
		if(pin == LED_ERR)
			{
				LED_ERR_PORT |= (1 << LED_ERR_PIN);
				_delay_ms(30);
				LED_ERR_PORT &= ~(1 << LED_ERR_PIN);
				_delay_ms(30);
			}
		if(pin == LED_HB)
			{
				LED_HB_PORT |= (1 << LED_HB_PIN);
				_delay_ms(30);
				LED_HB_PORT &= ~(1 << LED_HB_PIN);
				_delay_ms(30);
			}
	}
	while(tmes--);
}

void reset_target(unsigned char reset)
{
	if((reset && rst_active_high) || (!reset && !rst_active_high)) RESET_HIGH
	else RESET_LOW
}

unsigned char Serial_available(void)
{
	if(rx_wr_index0 != rx_rd_index0) return 1;
	return 0;
}

unsigned char getch(void)
{
	unsigned char temp;
	while(!(Serial_available()));
	temp = exchange[rx_rd_index0++];
	return(temp);
}

void fill(int n)
{
    int x;
    for (x = 0; x < n; x++) buff[x] = getch();
}

void prog_lamp(int state)
{
	if (PROG_FLICKER)
	{
		if(state) LED_PMODE_HIGH
		else LED_PMODE_LOW
	}
}

unsigned char spi_transaction(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
  SPI_transfer(a);
  SPI_transfer(b);
  SPI_transfer(c);
  return SPI_transfer(d);
}

void empty_reply(void)
{
    unsigned char rp = getch();
    if (rp == CRC_EOP)
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

void breply(unsigned char b)
{
    unsigned char rp = getch();
    if (rp == CRC_EOP)
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

void get_version(unsigned char c)
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
  param.flashsize = (buff[16] << 24) | (buff[17] << 16) | (buff[18] << 8) | buff[19];

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
      
    RESET_OUT

  spi_init();
  spi_setSpeed(3);
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

void end_pmode() {
	spi_deinit();
	// We're about to take the target out of reset so configure SPI pins as input
	reset_target(1);
	    
    RESET_IN
    
	pmode = 0;
    //Clear ring buffer
    //rx_wr_index0 = 0, rx_rd_index0 = 0;
}

void universal()
{
  unsigned char ch;

  fill(4);
  ch = spi_transaction(buff[0], buff[1], buff[2], buff[3]);
  breply(ch);
}

void flash(unsigned char hilo, unsigned int addr, unsigned char dat)
{
  spi_transaction((0x40 + 8 * hilo), ((addr >> 8) & 0xFF), (addr & 0xFF), dat);
}

void commit(unsigned int addr)
{
  int i;
    
  if (PROG_FLICKER)
  {
    prog_lamp(0);
  }
  spi_transaction(0x4C, ((addr >> 8) & 0xFF), (addr & 0xFF), 0);
  if (PROG_FLICKER)
  {
    for(i=0; i<PTIME; i++) _delay_ms(1);
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

unsigned char write_flash_pages(int length)
{
    int x = 0;
    unsigned int page; 
    
    page = current_page();
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

unsigned char write_eeprom(unsigned int length)
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

unsigned char write_eeprom_chunk(unsigned int start, unsigned int length)
{
  // this writes byte-by-byte, page writing may be faster (4 bytes at a time)
  unsigned int x;
  fill(length);
  prog_lamp(0);
  for (x = 0; x < length; x++)
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
   char result; 
   unsigned int length;
   char memtype;
    
  result = STK_FAILED;
  length = 256 * getch();
  length += getch();
  memtype = getch();
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

unsigned char flash_read(unsigned char hilo, unsigned int addr)
{
  return spi_transaction((0x20 + hilo * 8), ((addr >> 8) & 0xFF), (addr & 0xFF), 0);
}

char flash_read_page(int length)
{
    int x;
    unsigned char low;
    unsigned char high;
    
    for (x = 0; x < length; x += 2)
    {
        low = flash_read(0, here);
        uart_sendChar(low);
        high = flash_read(1, here);
        uart_sendChar(high);
        here++;
    }
    return STK_OK;
}

char eeprom_read_page(int length)
{
    // here again we have a word address
    int x;
    int start;
    int addr;
    unsigned char ee;
    
    start = here * 2;
    for (x = 0; x < length; x++)
    {
        addr = start + x;
        ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
        uart_sendChar(ee);
    }
    return STK_OK;
}

void read_page()
{
    char result;
    int length;
    char memtype;
    char temp;
    
    result = STK_FAILED;
    length = getch() << 8;
    length += getch();
    memtype = getch();
    
    temp = getch();
    if (temp != CRC_EOP )
    {
        error++;
        uart_sendChar(STK_NOSYNC);
        return;
    }
    else
    {
        uart_sendChar(STK_INSYNC);
        if (memtype == 'F') result = flash_read_page(length);
        if (memtype == 'E') result = eeprom_read_page(length);
        uart_sendChar(result);
    }
}

void read_signature(void)
{
    unsigned char high;
    unsigned char middle;
    unsigned char low;
    
  if (CRC_EOP != getch())
  {
    error++;
    uart_sendChar(STK_NOSYNC);
    return;
  }
  uart_sendChar(STK_INSYNC);
  high = spi_transaction(0x30, 0x00, 0x00, 0x00);
  uart_sendChar(high);
  middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  uart_sendChar(middle);
  low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  uart_sendChar(low);
  uart_sendChar(STK_OK);
}



void avrisp(void)
{
  unsigned char ch;

  ch = getch();
    
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
      here += getch() << 8;
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




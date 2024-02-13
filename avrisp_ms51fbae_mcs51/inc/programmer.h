
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
#define LED_HB_PIN        5
#define LED_ERR_PIN       6
#define LED_PMODE_PIN     7


#define RESET_PORT			P1
#define LED_HB_PORT			P1
#define LED_ERR_PORT		P1
#define LED_PMODE_PORT		P1

#define RESET_HIGH			RESET_PORT |= (1 << RESET_PIN);
#define LED_HB_HIGH			LED_HB_PORT |= (1 << LED_HB_PIN);
#define LED_ERR_HIGH		LED_ERR_PORT |= (1 << LED_ERR_PIN);
#define LED_PMODE_HIGH		LED_PMODE_PORT |= (1 << LED_PMODE_PIN);

#define RESET_LOW			RESET_PORT &= ~(1 << RESET_PIN);
#define LED_HB_LOW			LED_HB_PORT &= ~(1 << LED_HB_PIN);
#define LED_ERR_LOW			LED_ERR_PORT &= ~(1 << LED_ERR_PIN);
#define LED_PMODE_LOW		LED_PMODE_PORT &= ~(1 << LED_PMODE_PIN);

#define RESET_OUT           {P1M1 &= ~(1 << RESET_PIN); P1M2 |= (1 << RESET_PIN);}
#define RESET_IN            {P1M1 |= (1 << RESET_PIN); P1M2 &= ~(1 << RESET_PIN);}


#define beget16(addr) (*addr * 256 + *(addr+1) )


unsigned char Serial_available(void);
void setup(void);
void pulse(int pin, int times);

// Function definition
void avrisp(void);
void read_signature(void);
void read_page(void);
char eeprom_read_page(int length);
char flash_read_page(int length);
unsigned char flash_read(unsigned char hilo, unsigned int addr);
void program_page(void);
unsigned char write_eeprom_chunk(unsigned int start, unsigned int length);
unsigned char write_eeprom(unsigned int length);
unsigned char write_flash_pages(int length);
void write_flash(int length);
unsigned int current_page(void);
void commit(unsigned int addr);
void flash(unsigned char hilo, unsigned int addr, unsigned char dat);
void universal(void);
void end_pmode(void);
void start_pmode(void);
void set_parameters(void);
void get_version(unsigned char c);
void breply(unsigned char b);
void empty_reply();
unsigned char spi_transaction(unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void prog_lamp(int state);
void pulse(int pin, int times);
void fill(int n);
unsigned char getch(void);
void reset_target(unsigned char reset);
void heartbeat(void);
// End of Function definition
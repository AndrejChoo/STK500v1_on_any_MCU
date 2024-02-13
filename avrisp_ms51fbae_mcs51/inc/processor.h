

void deassemble_frame(void);//Функция разбора пакетов
void send_errorFrame(void); //Функция отаправки сообщения "Ошибка пакета"
void send_successConn(void); //Функция отправки пакета "Программатор присутствует"
void send_frame(unsigned char *buff, unsigned char count); //Функция отправки одного кадра
void send_arr(unsigned char marker, unsigned char *buff, unsigned int count, unsigned int offset);


//I2C EEPROM
void read_eeprom(void);
void write_eeprom(void);

//SPI FLASH
void read_flash(void);
unsigned char read_sreg1(void);
void write_flash(void);
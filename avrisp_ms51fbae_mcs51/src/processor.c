#include "processor.h"
#include "usart.h"
#include "spi.h"
//#include "i2c.h"
#include "delay.h"
#include "MS51_16K.h"


extern uint8_t rx_counter;
extern xdata uint8_t exchange[];
extern xdata uint8_t main_buff[];
extern uint8_t read_bsy; //Флаг готовности хоста 0 - готов, 1 - не готов

unsigned long f_size;
uint16_t f_psize;
uint32_t ee_offset;
uint16_t block;
char f_type;


//Отправка ответа об ошибке
void send_errorFrame(void)
{

}

void send_successConn(void)
{
	exchange[0] = 0xFE;
	exchange[1] = 0x01;
	exchange[2] = 0xCC;
	exchange[3] = 0x00;
	send_frame(exchange, 4);
}

void send_frame(uint8_t *buff, uint8_t count)
{
    int i;
	for(i = 0; i < count; i++) USARTSend_char(buff[i]);
}

//
void send_arr(uint8_t marker, uint8_t *buff, uint16_t count, uint16_t offset)
{
    int p;
	USARTSend_char(0xFE); //Start
	USARTSend_char(count);
	USARTSend_char(marker);
	for(p = 0; p < count; p++) USARTSend_char(buff[p + offset]);
}

void deassemble_frame(void)
{
	switch(exchange[2])
	{
		case 0xCC:
		{
			send_successConn();
			break;
		}
/*
		//Пакеты EEPROM
		case 0xE0://Чтение EEPROM
		{
			f_size = exchange[4] | (exchange[3]<<8);
			read_eeprom();
			break;
		}

		case 0xE1://Подготовка к записи EEPROM
		{
			f_size = exchange[4] | (exchange[3]<<8);
			ee_offset = 0;

			exchange[0] = 0xFE;
			exchange[1] = 0x01;
			exchange[2] = 0xE2;
			exchange[3] = 0x00;
			send_frame(exchange, 4);
			break;
		}

		case 0xE2://Пакет для записи в EEPROM
		{
			write_eeprom();
			break;
		}
*/
		//*************************SPI FLASH************************
		case 0xF0:
		{
			uint8_t temp_reg1 = 0, temp_reg2 = 0, temp_reg3 = 0;

			FLCS_LOW
			spi1_read(0x9F, 0);
			temp_reg3 = spi1_read(0x00, 0);
			temp_reg1 = spi1_read(0x00, 0);
			temp_reg2 = spi1_read(0x00, 0);
			FLCS_HIGH

			exchange[0] = 0xFE;
			exchange[1] = 0x03;
			exchange[2] = 0xF0;
			exchange[3] = temp_reg1;
			exchange[4] = temp_reg2;
			exchange[5] = temp_reg3;
			send_frame(exchange, 6);

			break;
		}
		case 0xF1:
		{
			uint8_t temp_reg1 = 0, temp_reg2 = 0;

			FLCS_LOW
			spi1_read(0x05, 0);
			temp_reg1 = spi1_read(0x00, 0);
			FLCS_HIGH

			FLCS_LOW
			spi1_read(0x35, 0);
			temp_reg2 = spi1_read(0x00, 0);
			FLCS_HIGH

			exchange[0] = 0xFE;
			exchange[1] = 0x02;
			exchange[2] = 0xF1;
			exchange[3] = temp_reg1;
			exchange[4] = temp_reg2;
			send_frame(exchange, 5);

			break;
		}
		case 0xF2:
		{
            unsigned long tmp_add;
			//[32:24]
			f_size = (exchange[3] << 8);
			f_size = f_size << 8;
			f_size = f_size << 8;
			//[23:16]
			tmp_add = (exchange[4] << 8);
			tmp_add = tmp_add << 8;
			f_size |= tmp_add;
			//[15:0]
			f_size |= (exchange[5] << 8) | exchange[6];
			read_flash();
			break;
		}

		case 0xF3:
		{
			uint8_t hSreg = exchange[3];
			uint8_t lSreg = exchange[4];

			FLCS_LOW
			spi1_read(0x50, 0);
			FLCS_HIGH

			while(!(read_sreg1() & (1 << 1)));

			FLCS_LOW
			spi1_read(0x01, 0);
			spi1_read(lSreg, 0);
			spi1_read(hSreg, 0);
			FLCS_HIGH

			FLCS_LOW
			spi1_read(0x04, 0);
			FLCS_HIGH

			exchange[0] = 0xFE;
			exchange[1] = 0x01;
			exchange[2] = 0xF3;
			exchange[3] = 0x00;
			send_frame(exchange, 4);

			break;
		}
		case 0xF4:
		{
			FLCS_LOW
			spi1_read(0x06, 0); //Write enable
			FLCS_HIGH

			while(!(read_sreg1() & (1 << 1)));

			FLCS_LOW
			spi1_read(0xC7, 0); //Erase
			FLCS_HIGH

			while(!(read_sreg1() & (1 << 1)));

			exchange[0] = 0xFE;
			exchange[1] = 0x01;
			exchange[2] = 0xF4;
			exchange[3] = 0x00;
			send_frame(exchange, 4);

			break;
		}
		case 0xF5:
		{
			f_psize = (exchange[3] << 8) | exchange[4];
            f_type = exchange[5];
			block = 0;
			ee_offset = 0;

			exchange[0] = 0xFE;
			exchange[1] = 0x01;
			exchange[2] = 0xF6;
			exchange[3] = 0x00;
			send_frame(exchange, 4);
			break;
		}
		case 0xF6:
		{
			write_flash();
			break;
		}

		default:
		{
			break;
		}
	}
}

//**********************************I2C EEPROM*************************************
/*
void read_eeprom(void)
{
    int i;
	if(f_size == 128) //24c01, 24c02
	{
		eeprom_read_arr(0xA0, 1, 0, main_buff, f_size);
		for(i = 0; i < f_size; i+= 128) send_arr(0xE0, main_buff, 128, i);
	}
	else if(f_size == 256) //24c01, 24c02
	{
		eeprom_read_arr(0xA0, 1, 0, main_buff, f_size);
		for(i = 0; i < f_size; i+= 128)
			{
				send_arr(0xE0, main_buff, 128, i);
				while(exchange[2] != 0x3D);
				exchange[2] = 0x00;
			}
	}
	else if(f_size > 256 && f_size <= 2048) //24c04, 24c08, 24c16
	{
		for(i = 0; i < f_size; i+= 128)
		{
			eeprom_read_arr((0xA0 | ((i >> 7) & 0x0E)), 1, i & 0xFF, main_buff, 128);
			send_arr(0xE0, main_buff, 128, 0);
			while(exchange[2] != 0x3D);
			exchange[2] = 0x00;
		}
	}
	else
	{
		for(i = 0; i < f_size; i+= 128)
		{
			eeprom_read_arr(0xA0, 2, i, main_buff, 128);
			send_arr(0xE0, main_buff, 128, 0);
			while(exchange[2] != 0x3D);
			exchange[2] = 0x00;
		}
	}
}

void write_eeprom(void)
{
	uint16_t offs = (exchange[3] << 8) | exchange[4];
    int k;

	if(f_size <= 256) //24c01, 24c02 8-байтовая страница
	{
		for(k = 0; k < 32; k+= 8)
		{
			eeprom_write_arr(0xA0, k + offs, 1, exchange, 8, k + 5);
			delay_ms(10);
		}
	}
	else if(f_size > 256 && f_size <= 2048) // 16-байтовая страница
	{
		uint8_t dop_add;

		for(k = 0; k < 32; k+= 16)
		{
			dop_add = ((offs + k) >> 7) & 0x0E;
			eeprom_write_arr( (0xA0 | dop_add), ((k + offs) & 0xFF), 1, exchange, 16, k + 5);
			delay_ms(15);
		}
	}
	else
	{
		eeprom_write_arr(0xA0, offs, 2, exchange, 32, 5);
		delay_ms(25);
	}

	exchange[0] = 0xFE;
	exchange[1] = 0x01;
	exchange[2] = 0xE2;
	exchange[3] = 0x00;
	send_frame(exchange, 4);
}
*/
//*********************************SPI FLASH********************************
//Читаем SREG1
uint8_t read_sreg1(void)
{
	uint8_t tmp;
	FLCS_LOW
	spi1_read(0x05, 0);
	tmp = spi1_read(0x00, 0);
	FLCS_HIGH
	return tmp;
}

void read_flash(void)
{
    unsigned long i, k;
	FLCS_LOW
    if(f_size <= 0x1000000)
    {
        spi1_read(0x03, 0);
        spi1_read(0x00, 0);
        spi1_read(0x00, 0);
        spi1_read(0x00, 0);
    }
    else
    {
        spi1_read(0x13, 0);
        spi1_read(0x00, 0);
        spi1_read(0x00, 0);
        spi1_read(0x00, 0); 
        spi1_read(0x00, 0); 
    }
	for(i = 0; i < f_size; i += 128)
	{
		for(k = 0; k < 128; k++) main_buff[k] = spi1_read(0x03, 0);
		send_arr(0xF2, main_buff, 128, 0);

		while(exchange[2] != 0x3D);
		exchange[2] = 0x00;
	}
	FLCS_HIGH
}

void write_flash(void)
{
	uint32_t tmp;
    int i,k;

	for(i = 0; i < exchange[1]; i++) main_buff[i + block] = exchange[i + 3];
	block += exchange[1];
	ee_offset += exchange[1];
	if(block == f_psize)
	{
		tmp = ee_offset - f_psize; //Адрес блока для записи

		FLCS_LOW
		spi1_read(0x06, 0);
		FLCS_HIGH
		while(!(read_sreg1() & (1 << 1)));

		FLCS_LOW
		spi1_read(0x02, 0);
        if(f_type == 2) spi1_read((tmp >> 24), 0);
		spi1_read(((tmp >> 16) & 0xFF), 0);
		spi1_read(((tmp >> 8) & 0xFF), 0);
		spi1_read((tmp & 0xFF), 0);
		for(k = 0; k < f_psize; k++) spi1_read(main_buff[k], 0);
		FLCS_HIGH

		while((read_sreg1() & 0x01) == 0x01); //bit BSY

		block = 0;
	}
	exchange[0] = 0xFE;
	exchange[1] = 0x01;
	exchange[2] = 0xF6;
	exchange[3] = 0x00;
	send_frame(exchange, 4);
}

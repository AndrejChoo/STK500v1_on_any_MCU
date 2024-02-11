/*
 * spi.c
 *
 *  Created on: Feb 10, 2024
 *      Author: Andrej
 */

#include "spi.h"

void spi_init(void)
{
#ifdef SPI_HARD

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    //SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    //SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);

#else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = MOSI_PIN|SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    MOSI_LOW
    SCK_LOW
#endif
}

void spi_deinit(void)
{
#ifdef SPI_HARD
    SPI_Cmd(SPI1, DISABLE);
#else
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = MOSI_PIN|SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

uint8_t SPI_transfer(uint8_t spdat)
{
#ifdef SPI_HARD
    /*
    while(!(SPI1->STATR & SPI_STATR_TXE));
    SPI1->DATAR = spdat;
    while(!(SPI1->STATR & SPI_STATR_RXNE));
    return SPI1->DATAR;
    */
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, spdat);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);

#else
    uint8_t rslt = 0;

    for (unsigned int i = 0; i < 8; i++)
    {
        if(spdat & (1 << (7 - i))) MOSI_HIGH
        else MOSI_LOW
        _delay_us(SCK_PERIOD / 4);
        SCK_HIGH
        _delay_us(SCK_PERIOD);
        if(GPIOA->INDR & MISO_PIN) rslt |= (1 << (7 - i));
        SCK_LOW // slow pulse
        _delay_us(SCK_PERIOD);
    }
    MOSI_LOW
    return rslt;
#endif
}

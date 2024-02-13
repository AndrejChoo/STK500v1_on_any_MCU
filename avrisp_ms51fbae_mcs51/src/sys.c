/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2020 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Jan/21/2020
//***********************************************************************************************************
#include "MS51_16K.H"
#include "sys.h"

unsigned char data  TA_REG_TMP,BYTE_TMP,SFRS_TMP,BIT_TMP;

  /**
  * @brief This API configures modify system HIRC value
  * @param[in] u8FsysMode . Valid values are:
  *                       - \ref HIRC_24                 :Internal HIRC 24MHz .
  *                       - \ref HIRC_16                 :Internal HIRC 16MHz.
  * @note      None.
  * @exmaple : MODIFY_HIRC(HIRC_24);
  */
 void MODIFY_HIRC(unsigned char u8HIRCSEL)
{
    unsigned char data hircmap0,hircmap1, offset,judge;
    unsigned char trimvalue16bit;

    BIT_TMP=EA; EA=0; TA=0xAA; TA=0x55; CHPCON|=0x01; EA=BIT_TMP;
    SFRS = 0 ;
    switch (u8HIRCSEL)
    {
      case HIRC_24:
        IAPAL = 0x38;
      break;
      case HIRC_16:
        IAPAL = 0x30;
      break;
    }
    IAPAH = 0x00;
    IAPCN = READ_UID;
    BIT_TMP=EA;EA=0; TA=0xAA; TA=0x55; IAPTRG|=0x01; EA=BIT_TMP;
    hircmap0 = IAPFD;
    IAPAL++;
    BIT_TMP=EA;EA=0; TA=0xAA; TA=0x55; IAPTRG|=0x01; EA=BIT_TMP;
    hircmap1 = IAPFD;

    TA = 0xAA;
    TA = 0x55;
    RCTRIM0 = hircmap0;
    TA = 0xAA;
    TA = 0x55;
    RCTRIM1 = hircmap1;
    BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPTRG&=~0x01;EA=BIT_TMP;
}

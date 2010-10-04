/*------------------------------------------------------------------------------
* (c) Copyright, Augusta Technology, Inc., 2006-present.
* (c) Copyright, Augusta Technology USA, Inc., 2006-present.
*  
* This software, document, web pages, or material (the "Work") is copyrighted 
* by its respective copyright owners.  The Work may be confidential and 
* proprietary.  The Work may be further protected by one or more patents and 
* be protected as a part of a trade secret package.
*   
* No part of the Work may be copied, photocopied, reproduced, translated, or 
* reduced to any electronic medium or machine-readable form, in whole or in 
* part, without prior written consent of the copyright owner. Any other 
* reproduction in any form without the permission of the copyright owner is 
* prohibited.
*   
* All Work are protected by the copyright laws of all relevant jurisdictions, 
* including protection under the United States copyright laws, and may not be 
* reproduced, distributed, transmitted, displayed, published, or broadcast 
* without the prior written permission of the copyright owner.
* 
------------------------------------------------------------------------------*/

#ifndef	__ASM_ARCH_BOARD_MIRACLE_H
#define	__ASM_ARCH_BOARD_MIRACLE_H

#define GPIO_SD_DETC		0
#define GPIO_ADAPTOR_DETC	1

#define GPIO_KEYPAD_ROW2	2
#define GPIO_KEYPAD_ROW3	3
#define GPIO_KEYPAD_COL0	8
#define GPIO_KEYPAD_COL1	9
#define GPIO_KEYPAD_COL2	10

#define GPIO_ETH_RST		4 
#define GPIO_ETH_IRQ		23 
#define GPIO_ETH_SPEED		25
#define GPIO_ETH_PME		27
#define GPIO_ETH_FIFO		28
#define GPIO_ETH_LK		33

#define GPIO_PMU_PWREN1		5
#define GPIO_PMU_WAKEUP		12
#define GPI0_PMU_SCUSB		16
#define GPIO_PMU_PWREN2		19
#define GPIO_PMU_IRQ		31

#define GPIO_CAM_RESETn		6
#define GPIO_CAM_PDN		30

#define GPIO_WIFI_RESETn	7
#define GPIO_WIFI_PA_ON		18	/*Actually Wifi PA power pin*/
#define GPIO_WIFI_WAKE_HOST	22
#define GPIO_WIFI_VREG_EN	35

#define GPIO_CABLE_INS		11

#define GPIO_VCHG_DET		13
#define GPIO_VCHG_STAT		38

#define GPIO_GPS_RXEN		14

#define GPIO_HP_INS		15
#define GPIO_AUDIO_CODEC	45

#define GPIO_USB_OFFVBUSn	17
#define GPIO_USB_INS		24
#define GPIO_VBUS_5V_EN		29

#define GPIO_TV_I2CCLK		20
#define GPIO_TV_I2CDAT		21

#define GPIO_BT_WAKE_HOST	26
#define GPIO_BT_RESET		39

#define GPIO_GATE_DISB		32

#define GPIO_LCD_BL_DIM		40
#define GPIO_LCD_STBYB		41
#define GPIO_LCD_PWEN_AVDD	44

#define GPIO_FM_I2CCLK		42
#define GPIO_FM_I2CDAT		43

#define GPIO_BT_CLK_REQ		46
#define GPIO_WIFI_CLK_REQ	47

#endif

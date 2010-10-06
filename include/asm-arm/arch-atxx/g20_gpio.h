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

#ifndef	__ASM_ARCH_BOARD_CAYMAN20_H
#define	__ASM_ARCH_BOARD_CAYMAN20_H

#define GPIO_EEP_WP		1

#define GPIO_KEYPAD_ROW0	0
#define GPIO_KEYPAD_ROW2	2
#define GPIO_KEYPAD_COL0	8
#define GPIO_KEYPAD_COL1	9
#define GPIO_KEYPAD_COL2	10

#define GPIO_PMU_PWREN1		5
#define GPIO_PMU_PWREN2		19
#define GPIO_PMU_WAKEUP		12
#define GPI0_PMU_SCUSB		16
#define GPIO_PMU_IRQ		31
#define GPIO_USB_OFFVBUSn	17

#define GPIO_LCD_BL_EN		13
#define GPIO_CTP_EN		34

#define GPIO_GPS_RXEN		14

#define GPIO_GSM_DTR		25
#define GPIO_GSM_RI		28
#define GPIO_GSM_ACKON		29
#define GPIO_GSM_RESETn		32
#define GPIO_GSM_ON		33

#define GPIO_CAM_RESETn		6
#define GPIO_CAM_PDN		30
#define GPIO_CAM_PWN_S		38

#define GPIO_WIFI_RESETn	7

#define GPIO_CABLE_INS		11

#define GPIO_MIC_DET		46
#define GPIO_HP_INS		15

#define GPIO_USB_INS		24

#define	GPIO_HOME_KEY		27

#define GPIO_WIFI_PA_ON		18
#define GPIO_WIFI_WAKE_HOST	22
#define GPIO_WIFI_VREG_EN	35

#define GPIO_BT_WAKE_HOST	26
#define GPIO_BT_RESET		39

#define GPIO_DTV_EN		40
#define GPIO_ATV_PD		41

#define GPIO_FM_I2CCLK		42
#define GPIO_FM_I2CDAT		43

#define GPIO_TV_I2CCLK		20
#define GPIO_TV_I2CDAT		21

#define GPIO_FM_SWEN		23
#define GPIO_PROXIMITY_EN	24

#define GPIO_TP_IRQ		45
#define GPIO_TP_RESETn		47

#endif

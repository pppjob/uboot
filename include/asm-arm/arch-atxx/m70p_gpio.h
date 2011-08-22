/*
 * Copyright (c) 2009 Wind River Systems, Inc.
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _M70P_GPIO_H
#define _M70P_GPIO_H

#define GPIO_LCD_PWEN_AVDD       0
#define GPIO_MV_RSTN             1
#define GPIO_KEYPAD_ROW2         2
#define GPIO_KEYPAD_ROW3         3
#define GPIO_SPK_PWDN			 4
//#define GPIO_KEYPAD_ROW5       5
#define GPIO_CAM1_RST		     6
#define GPIO_WIFI_RST		     7
#define GPIO_KEYPAD_COL0         8
//#define GPIO_KEYPAD_COL1         9
//#define GPIO_KEYPAD_COL2        10
#define GPIO_CAM2_RST		    11
#define GPIO_CAM1_PWDN		    12
#define GPIO_CAM2_PWDN		    13
#define GPIO_GPS_RXEN           14
#define GPIO_BT_RST             15
#define GPIO_PMU_SCUSB          16
#define GPIO_USB_OFFVBUSn       17
#define GPIO_LED_PWM			18
#define GPIO_PMU_MODE	        19
#define GPIO_TV_I2CCLK          20
#define GPIO_TV_I2CDAT          21
//#define GPIO_MSD2_CLK           22
//#define GPIO_MSD2_CMD           23
//#define GPIO_MSD2_DATA0         24
//#define GPIO_MSD2_DATA1         25
//#define GPIO_MSD2_DATA2         26
//#define GPIO_MSD2_DATA3         27
#define GPIO_PCM_DATIN_ALT	    28
#define GPIO_PCM_DATOUT_ALT	    29
#define GPIO_PCM_SYNC_ALT	    30
#define GPIO_PCM_CLK_ALT	    31
#define GPIO_CABLE_INS			32
#define GPIO_BT_WAKE_HOST	    33
#define GPIO_PMU_IRQ            34
#define GPIO_WIFI_VREG_EN       35
#define GPIO_SD_DETC			36
#define GPIO_WIFI_PA_ON         37
#define GPIO_PMU_WAKEUP         38
#define GPIO_HP_INS				39
#define GPIO_USB_INS            40
#define GPIO_TP_RST				41
#define GPIO_CAM_FLASH_EN		42/*flash light*/
#define GPIO_HP_ANSWER		    43
#define GPIO_GRYO_IRQ     		44 /*gryo irq*/
#define GPIO_TP_IRQ				45
#define GPIO_BTCLKREQ			46
#define	GPIO_WIFICLKREQ			47

#endif /* _M70P_GPIO_H */

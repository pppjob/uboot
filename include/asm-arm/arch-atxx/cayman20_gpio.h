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
#ifndef _CAYMAN_GPIO_H
#define _CAYMAN_GPIO_H

#define GPIO_SD_DETC            0
#define GPIO_EEP_WP             1

#define GPIO_KEYPAD_ROW2        2
#define GPIO_KEYPAD_ROW3        3
#define GPIO_KEYPAD_COL0        8
#define GPIO_KEYPAD_COL1        9
#define GPIO_KEYPAD_COL2        10

#define GPIO_PMU_PWREN1         5
#define GPIO_PMU_WAKEUP         12
#define GPI0_PMU_SCUSB          16
#define GPIO_PMU_IRQ            31
#define GPIO_USB_OFFVBUSn       17

#define GPIO_GSM_DTR            25
#define GPIO_GSM_RI             28
#define GPIO_GSM_ACKON          29
#define GPIO_GSM_RESETn         32
#define GPIO_GSM_ON             33

#define GPIO_CAM_RESETn         6
#define GPIO_WIFI_RESETn        7

#define GPIO_CABLE_INS          11

#define GPIO_MIC_DET            46
#define GPIO_HP_INS             15

#define GPIO_WIFI_PA_ON         18      /*Actually Wifi PA power pin*/

#define GPIO_USB_INS            24
#define GPIO_PMU_PWREN2         19

#define GPIO_WIFI_WAKE_HOST     22
#define GPIO_WIFI_VREG_EN       35
#define GPIO_WIFI_PIO1          36

#define GPIO_BT_WAKE_HOST       26
#define GPIO_BT_RESET           39

#define GPIO_FM_I2CCLK          42
#define GPIO_FM_I2CDAT          43

#define GPIO_TV_I2CCLK          20
#define GPIO_TV_I2CDAT          21

#define GPIO_FM_SWEN            23
#define GPIO_PROXIMITY_EN       24

#define GPIO_CAM_PDN            30
#define GPIO_GPS_RXEN           14


#endif /* _CAYMAN_GPIO_H_ */

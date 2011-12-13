/*------------------------------------------------------------------------------
* (c) Copyright, Augusta Technology, Inc., 2006-present.
* (c) Copyright, Augusta Technology USA, Inc., 2006-present.
*
* This software, document, web pages, or material (the "Work") is copyrighted
* by its respective copyright owners.  The Work may be confidential and
* proprietary.	The Work may be further protected by one or more patents and
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
#ifndef ___AT2600_PM_REGS_H_______
#define ___AT2600_PM_REGS_H_______

#include <asm/io.h>
#include <common.h>
#include <linux/types.h>
#include <asm/arch-atxx/regs_base.h>

/*-----------------------------------------------------------------------------
 * please refer to AT2600_PM datasheet for the following register definition:
 ----------------------------------------------------------------------------*/


enum at2600_pm_regs {
	AT2600_PM_REG_ID,	/* Variant & Version */
	AT2600_PM_REG_INT1,	/* Interrupt Status */
	AT2600_PM_REG_INT2,
	AT2600_PM_REG_INT3,
	AT2600_PM_REG_INT4,
	AT2600_PM_REG_INT5,
	AT2600_PM_REG_INT6,
	AT2600_PM_REG_INT7,
	AT2600_PM_REG_INT8,
	AT2600_PM_REG_M_INT1,	/* Interrupt Mask */
	AT2600_PM_REG_M_INT2,
	AT2600_PM_REG_M_INT3,
	AT2600_PM_REG_M_INT4,
	AT2600_PM_REG_M_INT5,
	AT2600_PM_REG_M_INT6,
	AT2600_PM_REG_M_INT7,
	AT2600_PM_REG_M_INT8,
	AT2600_PM_REG_ERROR,	/* Error Status */
	AT2600_PM_REG_OOCC1,	/* OOC (On-Off Controller) Control */
	AT2600_PM_REG_OOCC2,
	AT2600_PM_REG_OOCPH,	/* OOC Active Phase Control */
	AT2600_PM_REG_OOCS,	/* OOC Control */
	AT2600_PM_REG_EXCEPC1, /*temperature sensor conreol*/
	AT2600_PM_REG_RTC1 = 0x1a,	/* RTC Time */
	AT2600_PM_REG_RTC2,
	AT2600_PM_REG_RTC3,
	AT2600_PM_REG_RTC4,
	AT2600_PM_REG_RTC1A,	/* RTC Alarm */
	AT2600_PM_REG_RTC2A,
	AT2600_PM_REG_RTC3A,
	AT2600_PM_REG_RTC4A,
	AT2600_PM_REG_CBCC1,	/* CBC (Complete Battery Charger) Control */
	AT2600_PM_REG_CBCC2,
	AT2600_PM_REG_CBCC3,
	AT2600_PM_REG_CBCC4,
	AT2600_PM_REG_CBCC5,
	AT2600_PM_REG_CBCC6,
	AT2600_PM_REG_CBCS1,	/* CBC Status */
	AT2600_PM_REG_CBCS2,
	AT2600_PM_REG_PWM1S,	/* PWM (Pulse Width Modulator) Control */
	AT2600_PM_REG_PWM1D,
	AT2600_PM_REG_PWM2S,
	AT2600_PM_REG_PWM2D,
	AT2600_PM_REG_LED1C,	/* LED Control */
	AT2600_PM_REG_LED2C,
	AT2600_PM_REG_LEDCC,
	AT2600_PM_REG_GPIO1C1,	/* GPIO Control & ExREG Control */
	AT2600_PM_REG_E1REGC2,
	AT2600_PM_REG_E1REGC3,
	AT2600_PM_REG_GPIO2C1,
	AT2600_PM_REG_E2REGC2,
	AT2600_PM_REG_E2REGC3,
	AT2600_PM_REG_GPIO3C1,
	AT2600_PM_REG_E3REGC2,
	AT2600_PM_REG_E3REGC3,
	AT2600_PM_REG_GPIO4C1,
	AT2600_PM_REG_E4REGC2,
	AT2600_PM_REG_E4REGC3,
	AT2600_PM_REG_GPIO5C1,
	AT2600_PM_REG_E5REGC2,
	AT2600_PM_REG_E5REGC3,
	AT2600_PM_REG_GPIO6C1,
	AT2600_PM_REG_E6REGC2,
	AT2600_PM_REG_E6REGC3,
	AT2600_PM_REG_GPIOS, /*GPIO status*/
	AT2600_PM_REG_DCD_ST1,	/* Power Supply Control */
	AT2600_PM_REG_DCD_ST2,
	AT2600_PM_REG_DCD_ST3,
	AT2600_PM_REG_DCD_ST4,
	AT2600_PM_REG_INTC1, /*LDO INT STATUS*/
	AT2600_PM_REG_S1V2C1,
	AT2600_PM_REG_S1V2C2,
	AT2600_PM_REG_S1V2C3,
	AT2600_PM_REG_S1V8C1,
	AT2600_PM_REG_S1V8C2,
	AT2600_PM_REG_S1V8C3,
	AT2600_PM_REG_VBUSC1,
	AT2600_PM_REG_VBUSC2,
	AT2600_PM_REG_VBUSC3,
	AT2600_PM_REG_CMRC1,
	AT2600_PM_REG_CMRC2,
	AT2600_PM_REG_CMRC3,
	AT2600_PM_REG_D3V0C1 = 0X58,
	AT2600_PM_REG_D3V0C2,
	AT2600_PM_REG_D3V0C3,
	AT2600_PM_REG_SIM1C1,
	AT2600_PM_REG_SIM1C2,
	AT2600_PM_REG_SIM1C3,
	AT2600_PM_REG_SIM2C1,
	AT2600_PM_REG_SIM2C2,
	AT2600_PM_REG_SIM2C3,
	AT2600_PM_REG_SDC1 = 0X64,
	AT2600_PM_REG_SDC2,
	AT2600_PM_REG_SDC3,
	AT2600_PM_REG_FOCUSC1 = 0x6a,
	AT2600_PM_REG_FOCUSC2,
	AT2600_PM_REG_FOCUSC3,
	AT2600_PM_REG_USBC1,
	AT2600_PM_REG_USBC2,
	AT2600_PM_REG_USBC3,
	AT2600_PM_REG_ABBC1 = 0X76,
	AT2600_PM_REG_ABBC2,
	AT2600_PM_REG_ABBC3,
	AT2600_PM_REG_IMGC1,
	AT2600_PM_REG_IMGC2,
	AT2600_PM_REG_IMGC3,
	AT2600_PM_REG_R2V8C1 = 0X85,
	AT2600_PM_REG_R2V8C2,
	AT2600_PM_REG_R2V8C3,
	AT2600_PM_REG_R3V0C1,
	AT2600_PM_REG_R3V0C2,
	AT2600_PM_REG_R3V0C3,
	AT2600_PM_REG_LED1C1 = 0X8E,
	AT2600_PM_REG_LED1C2,
	AT2600_PM_REG_LED1C3,
	AT2600_PM_REG_LED2C1,
	AT2600_PM_REG_LED2C2,
	AT2600_PM_REG_LED2C3,
	AT2600_PM_REG_A2V5C1,
	AT2600_PM_REG_A2V5C2,
	AT2600_PM_REG_A2V5C3,
	AT2600_PM_REG_DCDLEDC1,
	AT2600_PM_REG_DCDLEDC2,
	AT2600_PM_REG_DCDLEDC3,
	AT2600_PM_REG_DCDLEDC4,
	AT2600_PM_REG_DCDDBGC1,
	AT2600_PM_REG_DCDDBGC2,
	AT2600_PM_REG_VIBRC1 = 0xab,
	AT2600_PM_REG_VIBRC2,
	AT2600_PM_REG_VIBRC3,
	AT2600_PM_REG_PWREN_UVLO =0xb6,
	AT2600_PM_REG_PWREN_MBG,
	AT2600_PM_REG_PWREN_INT9 = 0xbb,
	AT2600_PM_REG_PWREN_INT9_MASK,
	AT2600_PM_REG_NUM,
	AT2600_PM_REG_MAX
};
/*-----------------------------------------------------------------------------
 * all interrupts can be masked.
 * all these interrupt registers are Read-Clear (R&C)
 ----------------------------------------------------------------------------*/
enum at2600_pm_reg_int1 {
	AT2600_PM_INT1_LOWBAT	= 0x01,	/* low battery voltage */
	AT2600_PM_INT1_SECOND	= 0x02,	/* RTC periodic second interrupt */
	AT2600_PM_INT1_MINUTE	= 0x04,	/* RTC periodic minute interrupt */
	AT2600_PM_INT1_ALARM	= 0x08,	/* RTC alarm time is reached */
	AT2600_PM_INT1_ONKEYR	= 0x10,	/* ONKEY rising edge */
	AT2600_PM_INT1_ONKEYF	= 0x20,	/* ONKEY falling edge */
	AT2600_PM_INT1_ONKEY1S	= 0x40,	/* OMKEY at least 1sec low */
	AT2600_PM_INT1_HIGHTMP	= 0x80,	/* high temperature condition */
};


enum at2600_pm_reg_int2 {
	AT2600_PM_INT2_RES1	= 0x01,	/* reserve */
	AT2600_PM_INT2_RES2	= 0x02,	/* reserve*/
	AT2600_PM_INT2_RES3	= 0x04,	/* reserve */
	AT2600_PM_INT2_RES4	= 0x08,	/*reserve*/
	AT2600_PM_INT2_RES5	= 0x10,	/* reserve*/
	AT2600_PM_INT2_RES6	= 0x20,	/*reserve*/
	AT2600_PM_INT2_WD_TIMOUT	= 0x40,	/* charger watchdog timer expired */
	AT2600_PM_INT2_CHGWD	= 0x80,	/* watchdog will expire in 10s */
};



enum at2600_pm_reg_int3 {
	AT2600_PM_INT3_RES1 = 0x01, /*reserve  */
	AT2600_PM_INT3_CHARGER_UNOV	= 0x02, /* charger exit from over voltage state */
	AT2600_PM_INT3_OV	= 0x04, /* charger is over voltage*/
	AT2600_PM_INT3_CBC_TEMP_OK	= 0x08, /*CBC temperature ok  */
	AT2600_PM_INT3_CBC_TEMP_HIGH	= 0x10, /*CBC temperature high */
	AT2600_PM_INT3_CHARGER_DISCONN	= 0x20, /* charger disconnected */
	AT2600_PM_INT3_CHARGER_CONN	= 0x40, /* Charger connected*/
	AT2600_PM_INT3_CHARGE_FINISH	= 0x80, /* Charging finished*/
};


enum at2600_pm_reg_int4 {
	AT2600_PM_INT4_RESERVED1	= 0x01,	/* reserve */
	AT2600_PM_INT4_S1V2_PWUP	= 0x02,	/* S1V2 power up */
	AT2600_PM_INT4_S1V8_PWUP	= 0x04,	/* S1V8 power up */
	AT2600_PM_INT4_VBUS_PWUP	= 0x08,	/* VBUS Power UP*/
	AT2600_PM_INT4_CMR_PWUP	= 0x10,	/* CMR power up*/
	AT2600_PM_INT4_RESERVED2	= 0x20,	/* reserve  */
	AT2600_PM_INT4_D3V0_PWUP	= 0x40, /* D3V0 power up */
	AT2600_PM_INT4_SIM1_PWUP	= 0x80, /* SIM1 power up*/
};

enum at2600_pm_reg_int5 {
	AT2600_PM_INT5_SIM2_PWUP	= 0x01,	/* SIM2 power up */
	AT2600_PM_INT5_RESERVED1	= 0x02,	/* reserver*/
	AT2600_PM_INT5_SD1_PWUP	= 0x04,	/* SD1 power up */
	AT2600_PM_INT5_RESERVED2	= 0x08,	/* reserver */
	AT2600_PM_INT5_FOCUS_PWUP	= 0x10,	/* focus power up */
	AT2600_PM_INT5_USB_PWUP	= 0x20,	/* usb power up  */
	AT2600_PM_INT5_RESERVED3	= 0x40,	/* reserver */
	AT2600_PM_INT5_RESERVED4	= 0x80,	/* reserver */
};


enum at2600_pm_reg_int6 {
	AT2600_PM_INT6_A2V5_PWUP	= 0x01,	/*A2V5 power up */
	AT2600_PM_INT6_ABB_PWUP	= 0x02,	/* abb power up*/
	AT2600_PM_INT6_IMG_PWUP	= 0x04,	/* img power up  */
	AT2600_PM_INT6_RESERVED1	= 0x08,	/* reserver */
	AT2600_PM_INT6_RESERVED2	= 0x10,	/* reserver*/
	AT2600_PM_INT6_RESERVED3	= 0x20,	/* reserver */
	AT2600_PM_INT6_R2V8	= 0x40,	/* r2v8 power up */
	AT2600_PM_INT6_R3V0	= 0x80,	/* r3v0 power up*/
};



enum at2600_pm_reg_int7 {
	AT2600_PM_INT7_RESERVED1	= 0x01,	/*reserver */
	AT2600_PM_INT7_LED1_PWUP	= 0x02,	/* led1 power up */
	AT2600_PM_INT7_LED2_PWUP	= 0x04,	/* led2 power up */
	AT2600_PM_INT7_RESERVED2	= 0x08,	/* reserver */
	AT2600_PM_INT7_RESERVED3	= 0x10,	/* reserver */
	AT2600_PM_INT7_VIBR_PWUP	= 0x20,	/*vibr power up */
	AT2600_PM_INT7_DCDLED	= 0x40,	/* dcdled power up*/
	AT2600_PM_INT7_RESERVED4	= 0x80,	/* reserver*/
};



enum at2600_pm_reg_error {
	AT2600_PM_ERROR_WDTEXP	= 0x01,	/* watchdog timer status */
	AT2600_PM_ERROR_LOWBAT	= 0x02,	/* condition prior to system start-up */
	AT2600_PM_ERROR_UVLO	= 0x04,	/* under-voltage lockout prior to system start-up */
	AT2600_PM_ERROR_HIGHTMP	= 0x08,	/* junction temperature prior to system start-up */
	AT2600_PM_ERROR_RESERVED1	= 0x10,	/* reserver */
	AT2600_PM_ERROR_NOPOWER	= 0x20, /* power status on start-up */
};


enum at2600_pm_reg_oocc1 {
	AT2600_PM_OOCC1_GO_OFF	= 0x01,	/* Active-to-Off transition */
	AT2600_PM_OOCC1_GO_HIB	= 0x02,	/* Active-to-Hibernate transition */
	AT2600_PM_OOCC1_WDT_RST	= 0x04,	/* OOC watchdog timer control */
	AT2600_PM_OOCC1_RESERVED1	= 0x08,	/* reserver */
	AT2600_PM_OOCC1_WD_STOP	= 0x10,	/* watchdog stop */
	AT2600_PM_OOCC1_RTC_WAK	= 0x20,	/* wake-up after RTC alarm */
	AT2600_PM_OOCC1_WD_WAK	= 0x40,	/* watchdog wake up */
	AT2600_PM_OOCC1_TEMP_CHECK = 0x80, /*HIGH TEMP CHECK */
};






enum at2600_pm_reg_oocs {
	AT2600_PM_OOCS_ONKEY	= 0x01,	/* ONKEY_N pin status */
	AT2600_PM_OOCS_REV1	= 0x02,	/* reserved */
	AT2600_PM_OOCS_BAT	= 0x04,	/* main battery status */
	AT2600_PM_OOCS_REV2	= 0x08,	/* reserved */
	AT2600_PM_OOCS_REV3	= 0x10,	/* reserved */
	AT2600_PM_OOCS_UCHG	= 0x20, /* USB charger connected? */
	AT2600_PM_OOCS_TEMP	= 0x40, /* junction temperature status */
};

enum at2600_pm_reg_cbcs1 {
	AT2600_PM_CBCS1_CV_MODE	= 0x01,	/* cv mode */
	AT2600_PM_CBCS1_CC_MODE	= 0x02,	/*cc mode*/    
	AT2600_PM_CBCS1_OV_VOLTAGE	= 0x04,	
	AT2600_PM_CBCS1_USB_SUSPEND	= 0x08,	
	AT2600_PM_CBCS1_TEMP	= 0x10,	/* battery temperature status[ */
	AT2600_PM_CBCS1_CHARGER_CON	= 0x60, /*charger connect*/
	AT2600_PM_CBCS1_RECHARGER_STATE	= 0x80,/* charge status */
};

enum at2600_pm_reg_cbcs2 {
     AT2600_PM_CBCS2_BATTERY_STATUS=0x01,
};



#endif /* ___PCF50626_REGS_H_______ */


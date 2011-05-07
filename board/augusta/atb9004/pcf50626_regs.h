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
#ifndef ___PCF50626_REGS_H_______
#define ___PCF50626_REGS_H_______

#include <asm/io.h>
#include <common.h>
#include <linux/types.h>
#include <asm/arch-atxx/regs_base.h>

enum pcf50626_regs {
	ID,	/* Variant & Version */
	INT1,	/* Interrupt Status */
	INT2,
	INT3,
	INT4,
	INT5,
	INT6,
	INT7,
	INT8,
	INT9,
	M_INT1, /* Interrupt Mask */
	M_INT2,
	M_INT3,
	M_INT4,
	M_INT5,
	M_INT6,
	M_INT7,
	M_INT8,
	M_INT9,
	ERROR,	/* Error Status */
	OOCC1,	/* OOC (On-Off Controller) Control */
	OOCC2,
	OOCPH,	/* OOC Active Phase Control */
	OOCS,	/* OOC Control */
	BVMC,	/* BVM (Battery Voltage Monitor) Control */
	RECC1,	/* REC Control */
	RECC2,
	RECS,	/* REC Status */
	RTC1,	/* RTC Time */
	RTC2,
	RTC3,
	RTC4,
	RTC1A,	/* RTC Alarm */
	RTC2A,
	RTC3A,
	RTC4A,
	CBCC1,	/* CBC (Complete Battery Charger) Control */
	CBCC2,
	CBCC3,
	CBCC4,
	CBCC5,
	CBCC6,
	CBCS1,	/* CBC Status */
	CBCS2,
	BBCC1,	/* BBC (Backup Battery Charger) Control */
	PWM1S,	/* PWM (Pulse Width Modulator) Control */
	PWM1D,
	PWM2S,
	PWM2D,
	LED1C,	/* LED Control */
	LED2C,
	LEDCC,
	ADCC2,	/* ADC Control */
	ADCC3,
	ADCC4,
	ADCC1,
	ADCS1,	/* ADC Status */
	ADCS2,
	ADCS3,
	TSIC1,	/* Touch Screen */
	TSIC2,
	TSIDAT1,
	TSIDAT2,
	TSIDAT3,
	GPIO1C1,	/* GPIO Control & ExREG Control */
	E1REGC2,
	E1REGC3,
	GPIO2C1,
	E2REGC2,
	E2REGC3,
	GPIO3C1,
	E3REGC2,
	E3REGC3,
	GPIO4C1,
	E4REGC2,
	E4REGC3,
	GPIO5C1,
	E5REGC2,
	E5REGC3,
	GPIO6C1,
	E6REGC2,
	E6REGC3,
	GPO1C1,
	EO1REGC2,
	EO1REGC3,
	GPO2C1,
	EO2REGC2,
	EO2REGC3,
	GPO3C1,
	EO3REGC2,
	EO3REGC3,
	GPO4C1,
	EO4REGC2,
	EO4REGC3,
	D1REGC1,	/* Power Supply Control */
	D1REGC2,
	D1REGC3,
	D2REGC1,
	D2REGC2,
	D2REGC3,
	D3REGC1,
	D3REGC2,
	D3REGC3,
	D4REGC1,
	D4REGC2,
	D4REGC3,
	D5REGC1,
	D5REGC2,
	D5REGC3,
	D6REGC1,
	D6REGC2,
	D6REGC3,
	D7REGC1,
	D7REGC2,
	D7REGC3,
	D8REGC1,
	D8REGC2,
	D8REGC3,
	RF1REGC1,
	RF1REGC2,
	RF1REGC3,
	RF2REGC1,
	RF2REGC2,
	RF2REGC3,
	RF3REGC1,
	RF3REGC2,
	RF3REGC3,
	RF4REGC1,
	RF4REGC2,
	RF4REGC3,
	IOREGC1,
	IOREGC2,
	IOREGC3,
	USBREGC1,
	USBREGC2,
	USBREGC3,
	USIMREGC1,
	USIMREGC2,
	USIMREGC3,
	LCREGC1,
	LCREGC2,
	LCREGC3,
	HCREGC1,
	HCREGC2,
	HCREGC3,
	DCD1C1,
	DCD1C2,
	DCD1C3,
	DCD1C4,
	DCD1DVM1,
	DCD1DVM2,
	DCD1DVM3,
	DCD1DVMTIM,
	DCD2C1,
	DCD2C2,
	DCD2C3,
	DCD2C4,
	DCD2DVM1,
	DCD2DVM2,
	DCD2DVM3,
	DCD2DVMTIM,
	DCUDC1,
	DCUDC2,
	DCUDC3,
	DCUDC4,
	DCUDDVMTIM,
	DCULEDC1,
	DCULEDC2,
	DCULEDC3,
	DIMMAN,		/* DCULED Dimming Control */
	ALMCAL,		/* ALM Calibration */
	ALMCALMEA,
	ALMCRV1,
	ALMCRV2,
	ALMCRV3,
	ALMCRV4,
	GPIOS,		/* GPIO Status */
	DREGS1,		/* DxREG Status */
	DREGS2,
	RFREGS,		/* RFxREG Status */
	GREGS,		/* xREG Status */
	GPIO7C1,
	GPIO8C1,
	USIMDETC,
	NUM,
	TSINOI = 0xfe,
	TSIDAT4 = 0xff,
};
/*-----------------------------------------------------------------------------
 *
 * all interrupts can be masked.
 * all these interrupt registers are Read-Clear (R&C)
 *
 ----------------------------------------------------------------------------*/
enum int1 {
	PCF50626_INT1_LOWBAT	= 0x01, /* low battery voltage */
	PCF50626_INT1_SECOND	= 0x02, /* RTC periodic second interrupt */
	PCF50626_INT1_MINUTE	= 0x04, /* RTC periodic minute interrupt */
	PCF50626_INT1_ALARM	= 0x08, /* RTC alarm time is reached */
	PCF50626_INT1_ONKEYR	= 0x10, /* ONKEY rising edge */
	PCF50626_INT1_ONKEYF	= 0x20, /* ONKEY falling edge */
	PCF50626_INT1_ONKEY1S	= 0x40, /* OMKEY at least 1sec low */
	PCF50626_INT1_HIGHTMP	= 0x80, /* high temperature condition */
};

enum int2 {
	PCF50626_INT2_REC1R	= 0x01, /* rising edge detect */
	PCF50626_INT2_REC1F	= 0x02, /* falling edge detect */
	PCF50626_INT2_REC2LF	= 0x04, /* REC2 < VLOW */
	PCF50626_INT2_REC2LR	= 0x08, /* REC2 > VLOW */
	PCF50626_INT2_REC2HF	= 0x10, /* REC2 < VHIGH*/
	PCF50626_INT2_REC2HR	= 0x20, /* REC2 > VHIGH*/
	PCF50626_INT2_VMAX	= 0x40, /* charger switch from CC to CV */
	PCF50626_INT2_CHGWD	= 0x80, /* watchdog will expire in 10s */
};

enum int3 {
	PCF50626_INT3_TSIPENDOWN= 0x01, /* touchscreen pen-down detected */
	PCF50626_INT3_TSIPENUP	= 0x02, /* touchscreen pen-up detected */
	PCF50626_INT3_ADCRDY	= 0x04, /* ADC conversion finished */
	PCF50626_INT3_TOB	= 0x08, /* TOP of FIFO buffer reached */
	PCF50626_INT3_RESERVED	= 0x10, /* RESERVED */
	PCF50626_INT3_SIMUV	= 0x20, /* USIMREG reported under-voltage */
	PCF50626_INT3_MCHGINS	= 0x40, /* Charger insertion detected */
	PCF50626_INT3_MCHGRM	= 0x80, /* Charger removal detected */
};

enum int4 {
	PCF50626_INT4_CHGRES	= 0x01, /* charge resume */
	PCF50626_INT4_THLIMON	= 0x02, /* charger tmp limiter actived */
	PCF50626_INT4_THLIMOFF	= 0x04, /* charger tmp limiter deactived */
	PCF50626_INT4_BATFUL	= 0x08, /* battery full */
	PCF50626_INT4_BATTMFLT	= 0x10, /* battery tmp outside window */
	PCF50626_INT4_BATTMOK	= 0x20, /* battery tmp inside window */
	PCF50626_INT4_UCHGRM	= 0x40, /* USB Charger removal detected */
	PCF50626_INT4_UCHGINS	= 0x80, /* USB Charger insertion detected */
};

enum int5 {
	PCF50626_INT5_D1REGOK	= 0x01, /* D1REG o/p v>90% programmed value */
	PCF50626_INT5_D2REGOK	= 0x02, /* D2REG o/p v>90% programmed value */
	PCF50626_INT5_D3REGOK	= 0x04, /* D3REG o/p v>90% programmed value */
	PCF50626_INT5_D4REGOK	= 0x08, /* D4REG o/p v>90% programmed value */
	PCF50626_INT5_D5REGOK	= 0x10, /* D5REG o/p v>90% programmed value */
	PCF50626_INT5_D6REGOK	= 0x20, /* D6REG o/p v>90% programmed value */
	PCF50626_INT5_D7REGOK	= 0x40, /* D7REG o/p v>90% programmed value */
	PCF50626_INT5_D8REGOK	= 0x80, /* D8REG o/p v>90% programmed value */
};

enum int6 {
	PCF50626_INT6_RF1REGOK	= 0x01, /* RF1REG o/p v>90% programmed value */
	PCF50626_INT6_RF2REGOK	= 0x02, /* RF2REG o/p v>90% programmed value */
	PCF50626_INT6_RF3REGOK	= 0x04, /* RF3REG o/p v>90% programmed value */
	PCF50626_INT6_RF4REGOK	= 0x08, /* RF4REG o/p v>90% programmed value */
	PCF50626_INT6_IOREGOK	= 0x10, /* IOREG o/p v>90% programmed value */
	PCF50626_INT6_USBREGOK	= 0x20, /* USBREG o/p v>90% programmed value */
	PCF50626_INT6_USIMREGOK = 0x40, /* USIMREG o/p v>90% programed value */
	PCF50626_INT6_RESERVED	= 0x80, /* RESERVED */
};

enum int7 {
	PCF50626_INT7_D1REGCP	= 0x01, /* D1REG current limiting active */
	PCF50626_INT7_D2REGCP	= 0x02, /* D2REG current limiting active */
	PCF50626_INT7_D3REGCP	= 0x04, /* D3REG current limiting active */
	PCF50626_INT7_D4REGCP	= 0x08, /* D4REG current limiting active */
	PCF50626_INT7_D5REGCP	= 0x10, /* D5REG current limiting active */
	PCF50626_INT7_D6REGCP	= 0x20, /* D6REG current limiting active */
	PCF50626_INT7_D7REGCP	= 0x40, /* D7REG current limiting active */
	PCF50626_INT7_D8REGCP	= 0x80, /* D8REG current limiting active */
};

enum int8 {
	PCF50626_INT8_RF1REGCP	= 0x01, /* RF1REG current limiting active */
	PCF50626_INT8_RF2REGCP	= 0x02, /* RF2REG current limiting active */
	PCF50626_INT8_RF3REGCP	= 0x04, /* RF3REG current limiting active */
	PCF50626_INT8_RF4REGCP	= 0x08, /* RF4REG current limiting active */
	PCF50626_INT8_IOREGCP	= 0x10, /* IOREG current limiting active */
	PCF50626_INT8_USBREGCP	= 0x20, /* USBREG current limiting active */
	PCF50626_INT8_USIMREGCP = 0x40, /* USIMREG current limiting active */
	PCF50626_INT8_RESERVED	= 0x80, /* RESERVED */
};

enum int9 {
	PCF50626_INT9_RESERVED0 = 0x01, /* RESERVED */
	PCF50626_INT9_RESERVED1 = 0x02, /* RESERVED */
	PCF50626_INT9_RESERVED2 = 0x04, /* RESERVED */
	PCF50626_INT9_DCULEDOVP = 0x08, /* DCULED in over-voltage protection */
	PCF50626_INT9_CBCOVPFLT = 0x10, /* CBC in over-voltage protection */
	PCF50626_INT9_CBCOVPOK	= 0x20, /* CBC no longer in protection */
	PCF50626_INT9_USIMPRESF = 0x40, /* falling edge detected on USIMPRES */
	PCF50626_INT9_USIMPRESR = 0x80, /* rising edge detected on USIMPRES */
};

enum oocc1 {
	PCF50626_OOCC1_GO_OFF	= 0x01, /* Active-to-Off transition */
	PCF50626_OOCC1_GO_HIB	= 0x02, /* Active-to-Hibernate transition */
	PCF50626_OOCC1_WDT_RST	= 0x04, /* OOC watchdog timer control */
	PCF50626_OOCC1_TOT_RST	= 0x08, /* timeout timer control */
	PCF50626_OOCC1_TSI_WAK	= 0x10, /* wake-up after pen-down */
	PCF50626_OOCC1_RTC_WAK	= 0x20, /* wake-up after RTC alarm */
	PCF50626_OOCC1_RESERVED = 0x40, /* RESERVED */
	/* a low-battery condition is detected in Hibernate state */
	PCF50626_OOCC1_LOWBATHIBEN = 0x80,
};

enum cbcc1 {
	PCF50626_CBCC1_CHGENA	= 0x01, /* enable VCHG charge path */
	PCF50626_CBCC1_USBENA	= 0x02, /* enable VBUS charge path */
	PCF50626_CBCC1_AUTOCC	= 0x04, /* automatic CC mode for VCHG */
	PCF50626_CBCC1_AUTOSTOP = 0x08, /* automatic terminate charging */
	PCF50626_CBCC1_AUTORES	= 0x30, /* automatic charge resume */
	PCF50626_CBCC1_WDRST	= 0x40, /* CBC watchdog timer reset */
	PCF50626_CBCC1_WDTIME	= 0x80, /* maximum charge time */
};

enum bbcc1 {
	PCF50626_BBCC1_BBCE	= 0x01, /* enable backup battery charger */
	PCF50626_BBCC1_BBCR	= 0x02, /* bypass output resistor */
	PCF50626_BBCC1_BBCC	= 0x0C, /* select backup bat charge current */
	PCF50626_BBCC1_BBCV	= 0x10, /* automatic terminate charging */
	PCF50626_BBCC1_BBCES	= 0x20, /* voltage for backup bat charger */
	PCF50626_BBCC1_VSAVEEN	= 0x40, /* BBC activity in Hibernate state */
	PCF50626_BBCC1_RESERVED = 0x80, /* reserved */
};

enum cbcc2 {
	PCF50626_CBCC2_OVPENA	= 0x01, /* enable CBC overvoltage protection */
	PCF50626_CBCC2_SUSPENA	= 0x02, /* enable USB suspend mode control */
};

enum recc1 {
	PCF50626_RECC1_VTH	= 0x0F,
	PCF50626_RECC1_REC1_EN	= 0x10,
	PCF50626_RECC1_REC2_EN	= 0x20,
	PCF50626_RECC1_REC2_MOD	= 0x40,
	PCF50626_RECC1_MICBEN	= 0x80,
};

enum precharge_current_VBUS {
	CURRAT2_0	= 0x0,
	CURRAT2_2	= 0x2,
	CURRAT2_4	= 0x4,
	CURRAT2_8	= 0x8,
	CURRAT2_16	= 0x10,
	CURRAT2_18	= 0x12,
	CURRAT2_32	= 0x20,
	CURRAT2_64	= 0x40,
	CURRAT2_128	= 0x80,
	CURRAT2_255	= 0xff,
};


#endif /* ___PCF50626_REGS_H_______ */


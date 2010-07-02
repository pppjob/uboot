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

#include <asm/io.h>
#include <common.h>
#include <linux/types.h>
#include <asm/arch-atxx/regs_base.h>

/**************************************************************************
 * TSC Registers Define
 **************************************************************************/

#define ATXX_TSC_XY		0x00 /* Store X and Y measured result */
#define TSC_XY_X		0x3FF /* bit 0..9 X measured result */
#define TSC_XY_Y		(0x3FF << 16) /* Y measured result */

#define ATXX_TSC_Z		0x04 /* Store Z1 and Z2 measured result */
#define TSC_Z_Z1		0x3FF /* bit 0..9 Z1 measured result */
#define TSC_Z_Z2		(0x3FF << 16) /* Z2 measured result */

#define ATXX_TSC_AUX		0x08 /* Store AUX0 and AUX1 measured result */
#define TSC_AUX_AUX0		0x3FF /* bit 0..9 AUX0 measured result */
#define TSC_AUX_AUX1		(0x3FF << 16) /* bit 16..25 AUX1 measured result */

#define ATXX_TSC_ST		0x0C /* Store AUX2 measured result,IRQ and FSM status */
#define TSC_ST_AUX2		0x3FF /* bit 0..9 AUX2 measured result */
#define TSC_ST_PAN_IRQ		(0x1 << 16) /* Panel interrupt */
#define TSC_ST_DET_IRQ		(0x1 << 17) /* detection interrupt */
#define TSC_ST_UNTOUCH_IRQ	(0x1 << 18) /* untouch interrupt */
#define TSC_ST_FSM		(0x3F << 24) /* TSC FSM state */

#define ATXX_TSC_ADC		0x10 /* PV,AV,AD,STS and FPSM */
#define TSC_ADC_PV		(0x7 << 1) /* Panel voltage stabilization control */
#define TSC_ADC_AV		(0x3 << 2) /* Converter average control */
#define TSC_ADC_AD		(0xF << 10) /* AD */
#define TSC_ADC_STS		(0x1 << 14) /* Disable/enable ADC */
#define TSC_ADC_PSM		(0x1 << 15) /* Enable H/W control to TSM */

#define ATXX_TSC_REF		0x14 /* PDN,DL and INT */
#define TSC_REF_DL		(0x3 << 18) /* Delay time */
#define TSC_REF_INT		(0x1 << 20) /* ref selected */

#define ATXX_TSC_CONFIG		0x18 /* RESET,SNS and PRE */
#define TSC_CONFIG_RESET	(0xFF << 8) /*	0xbb reset */
#define TSC_CONFIG_SNS		(0x7 << 16) /* Sense Time Selection */
#define TSC_CONFIG_PRE		(0x7 << 19) /* Pre-charge Time Selection */

#define ATXX_TSC_SWCTRL		0x20

#define ATXX_TSC_CTRL		0x1C /* SELCLKR,EXTREF,WIRE_MODE,SW_PARAM and interrupt */
#define TSC_CTRL_SELCLKR	0x03 /* Clock rate running at the TSC */
#define TSC_CTRL_EXTREF		(0x1 << 2) /* Fef select */
#define TSC_CTRL_WIRE_MODE	(0x3 << 4) /* wire mode */
#define TSC_CTRL_SW_PARAM	(0x1 << 7) /* Parameters configured by software */
#define TSC_CTRL_PAN_IRQEN	(0x1 << 8) /* Panel interrupt enable */
#define TSC_CTRL_PAN_IRQCLR	(0x1 << 9) /* Panel interrupt clear */
#define TSC_CTRL_OPTADCEN	(0x1 << 10) /* Enable ADC power saving mode */
#define TSC_CTRL_MEASURE_AUX	(0x1 << 11) /* AUX measurement */
#define TSC_CTRL_UNT_IRQ_EN	(0x1 << 12) /* Enable panel untouch IRQ */
#define TSC_CTRL_UNT_IRQ_CLR	(0x1 << 13) /* Clear panel untouch IRQ status */
#define TSC_CTRL_TGTCLKCNTR	(0xFFF << 16) /* clock counter to generate clock */

#define ATXX_TSC_DETCTRL	0x28 /* Detection interrupt control */
#define TSC_DETCTRL_LOW_THRES	0x3FF /* Low detection threshold */
#define TSC_DETCTRL_HIGH_THRES	(0x3FF << 12) /* High detection threshold */
#define TSC_DETCTRL_AUX0	(0x1 << 24) /* Select AUX0 */
#define TSC_DETCTRL_AUX1	(0x1 << 25) /* Select AUX1 */
#define TSC_DETCTRL_AUX2	(0x1 << 26) /* Select AUX2 */
#define TSC_DETCTRL_HL		(0x1 << 27) /* detect high/low */
#define TSC_DETCTRL_INT_EN	(0x1 << 28) /* detection interrupt enable */
#define TSC_DETCTRL_INT_CLR	(0x1 << 29) /* detection interrupt clear */

#define ATXX_TSC_DETCT		0x2C /* Detection counter */
#define TSC_DETCT_SW_COUNT	0x7FFFFF /* S/W control detection panel touched counter */
#define TSC_DETCT_HW_COUNT	(0x3 << 24) /* H/W control detection panel touch counter */
#define TSC_DETCT_SH_CTRL	(0x1 << 27) /* Enable S/W control detecting panel touched counter */
#define TSC_DETCT_DETCNTR	(0x7 << 28) /* Number of detection before return to pre-charge state */
#define TSC_DETCT_PC_DET_EN	(0x1 << 31) /* Enable detection state return pre-charge state */

#define ATXX_TSC_DLUPT		0x30 /* Software control delay counter for power up */
#define ATXX_TSC_PVSTT		0x34 /* Software control panel voltage stabilized */
#define ATXX_TSC_PRECT		0x38 /* Software control pre-charge counter */
#define ATXX_TSC_SNSWT		0x3C /* Software control sensor wait counter */
#define TSC_SWCT_NUMBER		0xFFFF /* bit	0..15 */
#define TSC_SWCT_EN		(0x1<<31) /* bit 31 */

#define ATXX_TSC_SWPREC		0x40 /* Software control pre-charge state */
#define ATXX_TSC_SWDET		0x44 /* Software control detection state */
#define ATXX_TSC_SWMEAX		0x48 /* Software control measure X state */
#define ATXX_TSC_SWMEAY		0x4C /* Software control measure Y state */
#define ATXX_TSC_SWMEAZ1	0x50 /* Software control measure Z1 state */
#define ATXX_TSC_SWMEAZ2	0x54 /* Software control measure Z2 state */
#define ATXX_TSC_SWMEAA0	0x58 /* Software control measure Aux0 state */
#define ATXX_TSC_SWMEAA1	0x5C /* Software control measure Aux1 state */
#define ATXX_TSC_SWMEAA2	0x60 /* Software control measure Aux1 state */
#define TSC_SWCS_XPP		0x01 /* Control TSC ADC's Xppsw */
#define TSC_SWCS_XNN		(0x1 << 1) /* Control TSC ADC's Xnnsw*/
#define TSC_SWCS_YPP		(0x1 << 2) /* Control TSC ADC's Yppsw */
#define TSC_SWCS_YNN		(0x1 << 3) /* Control TSC ADC's Ynnsw */
#define TSC_SWCS_XNP		(0x1 << 4 /* Control TSC ADC's Xnpsw */
#define TSC_SWCS_WPN		(0x1 << 5 /* Control TSC ADC's WpnSw */
#define TSC_SWCS_YPN		(0x1 << 6 /* Control TSC ADC's Ypnsw */
#define TSC_SWCS_PENIACK	(0x1 << 7) /* Control TSC ADC's Peniack */
#define TSC_SWCS_REFP_SEL	(0x7 << 8) /* Control TSC ADC's refp */
#define TSC_SWCS_IN_SEL		(0xF << 12) /* Control TSC ADC's in */
#define TSC_SWCS_REFN_SEL	(0x7 << 16) /* Control TSC ADC's refn */

#define TSC_FEATURE_AUX      	0x01
#define TSC_FEATURE_DETECT   	0x02
#define TSC_FEATURE_TP       	0x04 


static uint32_t tsc_read_reg(uint32_t addr)
{
	return readl(ATXX_TSC_BASE + addr);
}

static void tsc_write_reg(uint32_t val, uint32_t addr)
{
	writel(val, ATXX_TSC_BASE + addr);
}

uint32_t adc2_config(void)
{
	uint32_t value;
	uint32_t counter=0;
	
	/* using internal 2.5v, no interrupt,measure AUXADC2 only */	
	tsc_write_reg(0xbbff, ATXX_TSC_CONFIG);
	tsc_write_reg(0x4052b, ATXX_TSC_SWDET);
	tsc_write_reg(0x4b002, ATXX_TSC_SWMEAA2);
	tsc_write_reg(0x20000000, ATXX_TSC_SWCTRL);

	udelay(20);
	
	tsc_write_reg(0x0, ATXX_TSC_SWCTRL);
	tsc_write_reg(0x0, ATXX_TSC_DETCTRL);
	tsc_write_reg(0xd41881, ATXX_TSC_CTRL);
	tsc_write_reg(0x68000, ATXX_TSC_REF);

	tsc_write_reg(0x40003c00, ATXX_TSC_ADC);
	tsc_write_reg(0x40009800, ATXX_TSC_ADC);
	tsc_write_reg(0xffff, ATXX_TSC_CONFIG);

	value = tsc_read_reg(ATXX_TSC_ST);
	while((value >> 24) != 0xc )
	{	 
	    mdelay(1);
	    value = tsc_read_reg(ATXX_TSC_ST);
	    counter++;
	    if (counter > 30)
		break;
	}
	value = tsc_read_reg(ATXX_TSC_ST);
	value &= 0x3ff;

	return value;
}


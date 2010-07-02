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

#ifndef __ASM_ARCH_AT6600_PM_H
#define __ASM_ARCH_AT6600_PM_H

/* Power Management Unit register offset */
#define	HWCFGR		0x00 	/* HW configuration register */
#define	SWCFGR		0x04 	/* SW configuration register */
#define	SWRSTR		0x08 	/* Soft reset register */
#define	ARMCFGR		0x10 	/* ARM configuration register */
#define	PROCCFGR	0x14 	/* PROC configuration register */
#define	DSPCFGR		0x18 	/* DSP configuration register */
#define	DSPACR		0x1C 	/* DSP access control register */
#define	COMMCFGR	0x20 	/* COMM configuration register */
#define	APPICMR		0x24 	/* APP Interconnect Matrix register */
#define	PMSTATR		0x3C 	/* PM status register */
#define	PMCFGR		0x40 	/* PM configuration register */
#define	CRYCFGR		0x44 	/* Crystal pad configuration register */
#define	PWDCNTR		0x48 	/* Power down counter register */
#define	PWUCNTR		0x4C	/* Power up counter register */
#define	ARMPWDR		0x50	/* ARM power down register */
#define	MMPWDR		0x54	/* MM power down register */
#define	DSPPWDR		0x58	/* DSP power down register */
#define	COMMPWDR	0x5C	/* COMM power down register */

/* SWCFGR */
#define	SWCFGR_SUSPEND_RAM		(1 << 0)
#define	SWCFGR_REBOOT_KCORE		(1 << 1)
#define	SWCFGR_POWERON_ONKEY		(1 << 2)
#define	SWCFGR_POWERON_ALARM		(1 << 3)
#define	SWCFGR_POWERON_REBOOT		(1 << 4)

/* SWRSTR bits */
#define	SWRSTR_SOFT_RESET		0x900DB88E

/* ARMCFGR bits */
#define	ARMCFGR_ARM_ENTER_SLEEP		0x01
#define	ARMCFGR_STANDBYWFI		0x02
#define ARMCFGR_ARM_SLEEP_FLAG		0x04

/* ARMPWDR bits */
#define	ARMPWDR_SLEEP_PWD		0x10000

/* MMPWDR bits */
#define	MMPWDR_POWER_DOWN		0x01
#define	MMPWDR_POWER_UP			0x10
#define	MMPWDR_POWER_STATE		0x7000
#define	MMPWDR_POWER_ON			0x00
#define	MMPWDR_POWER_OFF		0x04 

/* PMSATR bits */
#define	PMSATR_ARM_ENTER_SLEEP		0x01
#define	PMSATR_STANDBYWFI		0x02
#define	PMSATR_ARM_SLEEP		0x04
#define	PMSATR_ARC_SLEEP		0x08
#define	PMSATR_CKE_STATUS		0x10

/* PMCFGR bits */
#define	PMCFGR_DEEPSLEEP_EN		0x0001
#define	PMCFGR_SREFRESH_EN		0x0002
#define	PMCFGR_DLL_WAIT_EN		0x0004
#define	PMCFGR_CKE_STATUS_CHECK		0x0008
#define	PMCFGR_ARC_SLEEP_MASK		0x0100
#define	PMCFGR_ARM_SLEEP_MASK		0x0200
#define	PMCFGR_DLL_START_CNT		0x80000

/* CRYCFGR bits */
#define	CRYCFGR_SLEEP_CRYEN		0x01
#define	CRYCFGR_SLEEP_PMU_LOWPWR	0x02
#define	CRYCFGR_POL_PMU_LOWPWR		0x04
#define	CRYCFGR_START_CNT		0x80000

#ifndef	__ASSEMBLY__

static inline void pm_write_reg(unsigned int reg, unsigned int val)
{
	writel(val, ATXX_PM_UNIT_BASE + reg);
}

static inline unsigned int pm_read_reg(unsigned int reg)
{
	return readl(ATXX_PM_UNIT_BASE + reg);
}


#endif	/* __ASSEMBLY__ */

#endif

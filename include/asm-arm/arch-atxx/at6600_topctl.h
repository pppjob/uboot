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

#ifndef __ASM_ARCH_AT6600_TOPCTL_H
#define __ASM_ARCH_AT6600_TOPCTL_H

#ifndef __ASM_ARCH_TOPCTL_H
#error  "Do NOT include this file directly. Include asm/arch/topctl.h"
#endif
#include <asm/io.h>
#include "regs_base.h"
#include <linux/types.h>

#define	TOPCGER				(0X00)
#define	TOPICMR				(0X04)
#define	TOPDSPCFGR			(0X20)
#define	TOPDSPACR			(0X24)
#define	TOPI2CCTRL		       	(0X28)

#define TOPCTL0				(0X2C)  
#define TOPCTL1				(0X14)
#define TOPCTL2				(0X18)
#define TOPCTL3				(0X1C)
#define TOPCTL4				(0X30)
#define TOPCTL5				(0X34)
#define TOPCTL6				(0X38)
#define TOPCTL7				(0X3C)

#define TOPCTL1_MEMORY_STICK		(1<<3) 
#define	TOPCTL1_PWM_EN			(1<<4) 
#define	TOPCTL1_DRVVBUS_USB_EN		(1<<6) 
#define TOPCTL1_GPIO_I2S		(1<<7)
#define TOPCTL1_AC_I2S			(1<<8)
#define TOPCTL1_BT_I2S			(1<<9)
#define TOPCTL1_GPIO_HDMI		(1<<10)
#define TOPCTL1_NAND_CERDY3		(1<<11) 
#define TOPCTL1_NAND_CERDY4		(1<<12) 
#define	TOPCTL1_LCD_CTRL1		(1<<13)  
#define TOPCTL1_PMU_LOWPOWER		(1<<14) 
#define TOPCTL1_USB_SUSPEND		(1<<16) 
#define	TOPCTL1_LCD_CTRL2		(1<<26)
#define TOPCTL1_SCC_INTERFACE		(1<<29)

#endif /*__ASM_ARCH_AT6600_TOPCTL_H*/


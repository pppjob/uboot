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

#ifndef __ASM_ARCH_AT7700A0_REGS_BASE_H
#define __ASM_ARCH_AT7700A0_REGS_BASE_H

#ifndef __ASM_ARCH_REGS_BASE_H
#error  "Do NOT include this file directly. Include asm/arch/regs_base.h"
#endif

/* ARM/ARC bases */
#define ATXX_ARM_ARC_BASE          	0X3FD70000

/* multimedia engines */
#define ATXX_MME_BASE          		0X3FE00000

/* master engine */
#define ATXX_ME_BASE          		0X3FE00000

/* graphics engine (include master) */
#define ATXX_GE_BASE          		ATXX_ME_BASE

/* display engine */
#define ATXX_LCD_BASE			0X3FEF0000

/* video engine */
#define ATXX_VIDEO_BASE			0X3FEF1000

/* capture engine */
#define ATXX_CAMIF_BASE			0X3FEF2000

/* tv-out engine */
#define ATXX_TVOUT_BASE			0X3FEF3000

/* Denali memory ctrl register base */
#define	ATXX_DENALI_CTRL_BASE		0X3FFBE000

/* topctl */
#define	ATXX_TOPCTL_BASE		0X3FFFEC00

/* PM register base */
#define	ATXX_PM_UNIT_BASE		0X3FFDC000

/* BlueTooth */
#define	ATXX_BT_BASE			0X3FFE0000

/* UART */
#define	ATXX_UART0_BASE			0X3FFFD800	/* UART 0 */
#define	ATXX_UART1_BASE			0X3FFFD400	/* UART 1 */
#define	ATXX_UART2_BASE			0X3FFE9400	/* UART 2, arc uart */
#define	ATXX_UART3_BASE			0X3FFFCC00	/* UART 3 */
#define	ATXX_UART4_BASE			0X40133400      /* UART 4, DSP mux with arc uart at 0X3FFE9000 */
/* keypad */
#define	ATXX_KEYPAD_BASE		0X3FFFDC00

/* i2c controller */
#define	ATXX_I2C_BASE			0X3FFFF000

/* spi */
#define	ATXX_SPI0_BASE			0X3FFFFC00
#define	ATXX_SPI1_BASE			0X3FFFF800			
#define	ATXX_SPI2_BASE			0X3FFFD000

/* nand */
#define	ATXX_NAND_BASE			0X3FD70000

/* memstick */
#define	ATXX_MEMSTICK_BASE		0X3FFDC400

/* synopmob sd controller */
#define ATXX_SD0_BASE			0X3FFDE000
#define ATXX_SD1_BASE			0X3FD78000

/* CPU LCD */
#define	ATXX_CPU_LCD_BASE		0X3FFE5000
#define	ATXX_CPU_LCD_SIZE		0X800

/* ARM Timers */
#define	ATXX_TIMER_BASE			0X3FFFF400

/* ARC timer */
#define ATXX_ARC_TIMER_BASE		0X3FFE9C00

/* Interrupt controllers */
#define	ATXX_IRQ_BASE			0X3FFDC800

/* DMA */
#define ATXX_DMA_BASE			0X3FFDF800

/* WATCH DOG */
#define ATXX_WATCHDOG_BASE		0X3FFFE400

/* GPIO */
#define ATXX_GPIO_BASE			0X3FFFE800

#define ATXX_GPIOB_BASE			0X3FFEA000

/* USB */
#define	ATXX_USB1_BASE			0X3FDC0000
#define	ATXX_USB2_BASE			0X3FD80000

/* Smart Card */
#define ATXX_SMD_BASE			0X3FFFC800
#define ATXX_SMD_SIZE			0X400

/* Touch panel */
#define ATXX_TSC_BASE			0X3FFFE000

#define	ATXX_SECURITY_BASE		0X3FCF0000


/* DSP addresses */
/* DSP TDM */
#define	ATXX_DSP_TDM_BASE		0X40000000

/* DSP L2 memory bank 0 */
#define	ATXX_DSP_L20_BASE		0X40200000

/* DSP L2 memory bank 1 */
#define	ATXX_DSP_L21_BASE		0X40400000

/* DSP regs */
#define ATXX_DSP_REG_BASE		0X40130000

/* XS1200 TDM */
#define	ATXX_XS1200_TDM_BASE		0X40110000

/* COM97 */
#define ATXX_COM97_BASE			0X40133000
#endif /*__ASM_ARCH_AT7700A0_REGS_BASE_H*/

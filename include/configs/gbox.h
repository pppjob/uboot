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

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ATXX
#define CONFIG_AT7700B0
#define CONFIG_BOARD_GAMEBOX
#define CONFIG_DISPLAY_CPUINFO

/* LCD Setting */
#define CONFIG_LCD
#define CONFIG_ATXX_LCD
#define CONFIG_VIDEO_INNO7DD
#define CONFIG_LOGO_AUGUSTA

#define CONFIG_SYS_WHITE_ON_BLACK
#define LCD_BPP				LCD_COLOR16

/* useless macro, just for compile */
#define CONFIG_SYS_HZ 			1000

/* Physical Memory Map */

/* we have 1 bank of DRAM */
#define CONFIG_NR_DRAM_BANKS   		1
#define MDDR_BASE_ADDR			0x88000000

#define CONFIG_UNCONTINUOUS_MEM
#define CONFIG_SYS_MALLOC_END		(MDDR_BASE_ADDR + 0x00600000)
#define CONFIG_SYS_MALLOC_LEN		0x00100000	/* 1MB */

/* u-boot run address */
#define	CONFIG_SYS_UBOOT_BASE		(MDDR_BASE_ADDR + 0x00008000)

/* default load address for reading (i.e. kernel zImage with header) */
#define CONFIG_SYS_LOAD_ADDR		(MDDR_BASE_ADDR + 0x00807e00)
#define CONFIG_SYS_KERN_ADDR		(MDDR_BASE_ADDR + 0x00808000)
#define CONFIG_ATAG_ADDR		(MDDR_BASE_ADDR + 0x01800100)

/* enable passing of ATAGs  */
#define CONFIG_CMDLINE_TAG		1
/* call misc_init_r during start up */
#define CONFIG_MISC_INIT_R		1

/* 
 * Exclude ZLIB Support, for downsize
 */
#define CONFIG_EXCLUDE_ZIP

/* reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	(128)

/* no irqs */
#undef CONFIG_USE_IRQ

/* Use key detect */
#define CONFIG_AUTOBOOT_KEYHW
#define CONFIG_BOOTDELAY		0

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#ifdef CONFIG_USE_IRQ
/* reserved for IRQ stack */
#define CONFIG_STACKSIZE_IRQ		(4*1024)
/* reserved for FIQ stack */
#define CONFIG_STACKSIZE_FIQ		(4*1024)
#endif
/* regular stack */
#define CONFIG_STACKSIZE		(512*1024)

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_PROMPT		"AT7700 # "
/* Console I/O Buffer Size*/
#define CONFIG_SYS_CBSIZE 		512 

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
/* max number of command args */
#define CONFIG_SYS_MAXARGS		16

/*
 * ATXX UART Configuration
 */

#define CONFIG_ATXX_SERIAL

/* FIXME : not correct */
#define CFG_UART_CLOCK_FREQ 		(3686400*16)
#define CONFIG_BAUDRATE			921600
#define CONFIG_SYS_BAUDRATE_TABLE   	\
		{ 9600, 19200, 38400, 57600, 115200, 921600 }
#define CFG_UART_LOOPENABLE 		0
#define CFG_UART_FLOWCONTROL_ENABLE	0
#define CFG_UART_FIFO_ON
#define CONFIG_SERIAL_MULTI
#define CONFIG_BOOTARGS						\
		"console=ttyS0,921600n8 console=ttyMTD androidboot.console=ttyS0 mtdparts=atxx_nd:32M(boot),2M(ttyMTD),-(system) init=/init ubi.mtd=2 root=ubi0:rootfs rootfstype=ubifs ro"
#define CONFIG_BOOTCOMMAND	 	"nand read 89807e00 600000 200000; hdcvt 89807e00; bootm 89807fc0"

#define CONFIG_BOOTARGS_SD				\
		"console=ttyS0,921600n8 console=ttyMTD androidboot.console=ttyS0 mtdparts=atxx_nd:32M(boot),2M(ttyMTD),-(system) quiet"
#define CONFIG_BOOTCOMMAND_SD		"fatload mmc 1 0x89807e00 kboot.img; hdcvt 89807e00; bootm 89807fc0"

/* nand */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x3fd70000
#define CONFIG_SYS_NAND_MAX_CHIPS	4
#define CONFIG_NAND_ECC			4
#define CONFIG_NAND_OOBSIZE		128
#define CONFIG_NAND_ATXX
#define CONFIG_NAND_FULL_HW
#define MTDOOPS_OFFSET                  0x2000000

/* Environment */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET		0x00400000	/* 4M ~ 2M */
#define CONFIG_ENV_SIZE			8192		/* 8KB */
#define CONFIG_ENV_RANGE                0x00200000

#define	CONFIG_UBOOT_LADDR		0x88007e00

#define	CONFIG_XLOADER_OFFSET		0x00000000	/* 0   + 2M */
#define	CONFIG_XLOADER_MSIZE		0x00200000

#define	CONFIG_UBOOT_OFFSET		0x00200000	/* 2M + 2M */
#define	CONFIG_UBOOT_MSIZE		0x00200000

#define	CONFIG_KERNEL_OFFSET		0x00600000	/* 6M  + 3M */
#define	CONFIG_KERNEL_MSIZE		0x00300000

/* clock env */
#define CONFIG_CLK_ARM                  806000000
#define CONFIG_CLK_AXI                  312000000
#define CONFIG_CLK_APP                  104000000
#define CONFIG_CLK_MDDR                 201000000
#define CONFIG_CLK_GCLK                 312000000
#define CONFIG_CLK_VPCLK                156000000
#define CONFIG_CLK_VSCLK                403000000

/* SD/fat */
#define CONFIG_ATXX_MMC
#define CONFIG_MMC 
#define CONFIG_DOS_PARTITION

/* I2C */
#define CONFIG_I2C_ATXX
#define CONFIG_PM_AT2600

/*
 * Command lists
 */
#define CONFIG_CMD_RUN
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_NAND
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_LOAD
#define CONFIG_CMD_LOADX	/* loadx */
#define CONFIG_CMD_FAT
#define CONFIG_CMD_MMC
#define CONFIG_CMD_BDI

#define CONFIG_CMD_STEST
#define CONFIG_CMD_HDCVT
#define CONFIG_CMD_FACTORYDATA
#define CONFIG_CMD_ATEST
#define CONFIG_CMD_ABOOT
#define CONFIG_CMD_ADOWNLOAD
#define CONFIG_CMD_CLOCKENV

#define CONFIG_SYS_LONGHELP

#define CONFIG_SYS_MEMTEST_START	(MDDR_BASE_ADDR + 0x01000000)
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 4096)

#define CONFIG_SYS_NO_FLASH

/*
 * The ARM boot monitor initializes the board.
 * However, the default U-Boot code also performs the initialization.
 * If desired, this can be prevented by defining SKIP_LOWLEVEL_INIT
 * - see documentation supplied with board for details of how to choose the
 * image to run at reset/power up
 * e.g. whether the ARM Boot Monitor runs before U-Boot
 */
#define CONFIG_SKIP_LOWLEVEL_INIT

/*
 * The ARM boot monitor does not relocate U-Boot.
 * However, the default U-Boot code performs the relocation check,
 * and may relocate the code if the memory map is changed.
 * If necessary this can be prevented by defining SKIP_RELOCATE_UBOOT
 */
#define SKIP_CONFIG_RELOCATE_UBOOT

/*-----------------------------------------------------------------------
 * There are various dependencies on the core module (CM) fitted
 * Users should refer to their CM user guide
 * - when porting adjust u-boot/Makefile accordingly
 * to define the necessary CONFIG_ s for the CM involved
 * see e.g. cp_926ejs_config
 */

#include "armcoremodule.h"

/*
 * If CONFIG_SKIP_LOWLEVEL_INIT is not defined &
 * the core module has a CM_INIT register
 * then the U-Boot initialisation code will
 * e.g. ARM Boot Monitor or pre-loader is repeated once
 * (to re-initialise any existing CM_INIT settings to safe values).
 *
 * This is usually not the desired behaviour since the platform
 * will either reboot into the ARM monitor (or pre-loader)
 * or continuously cycle thru it without U-Boot running,
 * depending upon the setting of Integrator/CP switch S2-4.
 *
 * However it may be needed if Integrator/CP switch S2-1
 * is set OFF to boot direct into U-Boot.
 * In that case comment out the line below.
#undef	CONFIG_CM_INIT
 */

#endif /* __CONFIG_H */

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

#define CONFIG_AT6600
#define CONFIG_IDENT_STRING 		" AUGUSTATEK-ATXX "
#define CONFIG_DISPLAY_CPUINFO


#define CONFIG_UNCONTINUOUS_MEM
#define CONFIG_SYS_MALLOC_START		0x40220000

/* 
 * AT6600  Config
 */
#define AT6600_BOARD_A0_FIJI 		1
#define AT6600_BOARD_A0_TONGA 		2
#define AT6600_BOARD_B0_BALI 		3
#define AT6600_BOARD_B0_TONGA 		4
#define AT6600_BOARD_B0_CAYMAN 		5
#define AT6600_BOARD_C0_BALI 		6
#define AT6600_BOARD_C0_CAYMAN 		7
#define AT6600_BOARD_D0_BALI 		8
#define AT6600_BOARD_D0_CAYMAN20 	9



#define CFG_AT6600_BOARD 		AT6600_BOARD_D0_CAYMAN20

#define	CONFIG_SYS_UBOOT_BASE		0x40400400
#define	CONFIG_SYS_PHY_UBOOT_BASE	0x40400400

/*
 * Clock  Config
 */
#define AT6600_ARM_CLOCK_104 		1
#define AT6600_ARM_CLOCK_312 		2
#define AT6600_ARM_CLOCK_624 		3
#define AT6600_AXI_CLOCK_52 		1
#define AT6600_AXI_CLOCK_104 		2
#define AT6600_AXI_CLOCK_156 		3
#define AT6600_MDDR_CLOCK_52 		1
#define AT6600_MDDR_CLOCK_104 		2
#define AT6600_MDDR_CLOCK_156 		3

#define CFG_AT6600_ARM_CLOCK_DEFAULT 		AT6600_ARM_CLOCK_312
#define CFG_AT6600_AXI_CLOCK_DEFAULT 		AT6600_AXI_CLOCK_156
#define CFG_AT6600_MDDR_CLOCK_DEFAULT 		AT6600_MDDR_CLOCK_104

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CFG_MEMTEST_START			0x8c000000
#define CFG_MEMTEST_END				0x8c100000

#define CFG_HZ					32768
#define CFG_HZ_CLOCK				32768
#define CFG_TIMERBASE				0x3FFFF400

#define CONFIG_CMDLINE_TAG			1	/* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS		1
#define CONFIG_INITRD_TAG 			1
#define CONFIG_MISC_INIT_R			1	/* call misc_init_r during start up */

/*
 * Log Buf Support
 */
#define CONFIG_LOG_BUF
#define CONFIG_LOG_BUF_SHIFT 		12	/* (1 << 12) -> 4K */

/* 
 * Exclude ZLIB Support, for downsize
 */
#define CONFIG_EXCLUDE_ZIP

#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 24*1024)	/* reserved for malloc */
#define CONFIG_SYS_GBL_DATA_SIZE	(128)		/* reserved for initial data */
/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ		(4*1024)	/* reserved for IRQ stack */
#define CONFIG_STACKSIZE_FIQ		(4*1024)	/* reserved for FIQ stack */
#endif
#define CONFIG_STACKSIZE		(24*1024)	/* regular stack */


#define CONFIG_ATXX_SERIAL
/*
 * DW APB UART Configuration
 */
#define CFG_DWAPBUART_SERIAL
/* FIXME : not correct */
#define CFG_UART_CLOCK_FREQ 		(3686400*16)
#define CONFIG_BAUDRATE			921600
#define CONFIG_SYS_BAUDRATE_TABLE      { 9600, 19200, 38400, 57600, 115200, 921600 }
#define CFG_UART_LOOPENABLE 	0
#define CFG_UART_FIFO_ON

#define CONFIG_UART_XMODEM

#define CONFIG_COMMANDS		(CFG_CMD_IMI | CFG_CMD_BDI | CFG_CMD_MEMORY | \
				CFG_CMD_ENV | CFG_CMD_AUTOSCRIPT)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
//#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY		2
#define CONFIG_BOOTARGS			"root=/dev/ram console=ttyS0,921600n8"
#define CONFIG_BOOTCOMMAND 		"help"

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP							/* undef to save memory */
#define CONFIG_SYS_PROMPT				"AT6600 # "		/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE				256				/* Console I/O Buffer Size*/

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE				(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS				16				/* max number of command args */
#define CFG_BARGSIZE			CONFIG_SYS_CBSIZE		/* Boot Argument Buffer Size*/

#undef	CFG_CLKS_IN_HZ							/* everything, incl board info, in Hz */
#define CONFIG_SYS_LOAD_ADDR			0x88000000		/* default load address, this is for flash and autoscript */

#undef CONFIG_USE_IRQ


/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1			/* we have 1 bank of DRAM */
#define PHYS_SDRAM_1			0x88000000
#define PHYS_SDRAM_1_SIZE 		0x08000000	/* 64 MB */

#define CONFIG_SYS_NO_FLASH


#define CFG_MONITOR_LEN			0x00100000

#define CONFIG_ENV_IS_NOWHERE

#define CONFIG_ENV_SIZE			8192		/* 8KB */

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

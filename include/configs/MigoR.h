/*
 * Configuation settings for the Renesas Solutions Migo-R board
 *
 * Copyright (C) 2007 Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 */

#ifndef __MIGO_R_H
#define __MIGO_R_H

#undef DEBUG
#define CONFIG_SH		1
#define CONFIG_SH4		1
#define CONFIG_CPU_SH7722	1
#define CONFIG_MIGO_R		1

#define CONFIG_CMD_LOADB
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_NFS
#define CONFIG_CMD_DFL
#define CONFIG_CMD_SDRAM
#define CONFIG_CMD_ENV

#define CONFIG_BAUDRATE		115200
#define CONFIG_BOOTDELAY	3
#define CONFIG_BOOTARGS		"console=ttySC0,115200 root=1f01"
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.10.100
#define CONFIG_SERVERIP		192.168.10.77
#define CONFIG_GATEWAYIP	192.168.10.77

#define CONFIG_VERSION_VARIABLE
#undef  CONFIG_SHOW_BOOT_PROGRESS

/* SMC9111 */
#define CONFIG_DRIVER_SMC91111
#define CONFIG_SMC91111_BASE    (0xB0000000)

/* MEMORY */
#define MIGO_R_SDRAM_BASE	(0x8C000000)
#define MIGO_R_FLASH_BASE_1	(0xA0000000)
#define MIGO_R_FLASH_BANK_SIZE	(64 * 1024 * 1024)

#define CFG_LONGHELP			/* undef to save memory	*/
#define CFG_PROMPT		"=> "	/* Monitor Command Prompt */
#define CFG_CBSIZE		256		/* Buffer size for input from the Console */
#define CFG_PBSIZE		256		/* Buffer size for Console output */
#define CFG_MAXARGS		16		/* max args accepted for monitor commands */
#define CFG_BARGSIZE	512		/* Buffer size for Boot Arguments passed to kernel */
#define CFG_BAUDRATE_TABLE	{ 115200 }	/* List of legal baudrate settings for this board */

/* SCIF */
#define CFG_SCIF_CONSOLE	1
#define CONFIG_CONS_SCIF0	1
#undef  CFG_CONSOLE_INFO_QUIET	/* Suppress display of console
								   information at boot */
#undef  CFG_CONSOLE_OVERWRITE_ROUTINE
#undef  CFG_CONSOLE_ENV_OVERWRITE

#define CFG_MEMTEST_START	(MIGO_R_SDRAM_BASE)
#define CFG_MEMTEST_END		(CFG_MEMTEST_START + (60 * 1024 * 1024))

/* Enable alternate, more extensive, memory test */
#undef  CFG_ALT_MEMTEST
/* Scratch address used by the alternate memory test */
#undef  CFG_MEMTEST_SCRATCH

/* Enable temporary baudrate change while serial download */
#undef  CFG_LOADS_BAUD_CHANGE

#define CFG_SDRAM_BASE	(MIGO_R_SDRAM_BASE)
/* maybe more, but if so u-boot doesn't know about it... */
#define CFG_SDRAM_SIZE	(64 * 1024 * 1024)
/* default load address for scripts ?!? */
#define CFG_LOAD_ADDR	(CFG_SDRAM_BASE + 16 * 1024 * 1024)

/* Address of u-boot image in Flash (NOT run time address in SDRAM) ?!? */
#define CFG_MONITOR_BASE	(MIGO_R_FLASH_BASE_1)
/* Monitor size */
#define CFG_MONITOR_LEN	(128 * 1024)
/* Size of DRAM reserved for malloc() use */
#define CFG_MALLOC_LEN	(256 * 1024)
/* size in bytes reserved for initial data */
#define CFG_GBL_DATA_SIZE	(256)
#define CFG_BOOTMAPSZ	(8 * 1024 * 1024)

/* FLASH */
#define CFG_FLASH_CFI
#define CFG_FLASH_CFI_DRIVER
#undef  CFG_FLASH_QUIET_TEST
/* print 'E' for empty sector on flinfo */
#define CFG_FLASH_EMPTY_INFO
/* Physical start address of Flash memory */
#define CFG_FLASH_BASE	(MIGO_R_FLASH_BASE_1)
/* Max number of sectors on each Flash chip */
#define CFG_MAX_FLASH_SECT	512

/* if you use all NOR Flash , you change dip-switch. Please see MIGO_R01 Manual. */
#define CFG_MAX_FLASH_BANKS	1
#define CFG_FLASH_BANKS_LIST	{ CFG_FLASH_BASE + (0 * MIGO_R_FLASH_BANK_SIZE) }

/* Timeout for Flash erase operations (in ms) */
#define CFG_FLASH_ERASE_TOUT	(3 * 1000)
/* Timeout for Flash write operations (in ms) */
#define CFG_FLASH_WRITE_TOUT	(3 * 1000)
/* Timeout for Flash set sector lock bit operations (in ms) */
#define CFG_FLASH_LOCK_TOUT	(3 * 1000)
/* Timeout for Flash clear lock bit operations (in ms) */
#define CFG_FLASH_UNLOCK_TOUT	(3 * 1000)

/* Use hardware flash sectors protection instead of U-Boot software protection */
#undef  CFG_FLASH_PROTECTION
#undef  CFG_DIRECT_FLASH_TFTP

/* ENV setting */
#define CFG_ENV_IS_IN_FLASH
#define CONFIG_ENV_OVERWRITE	1
#define CFG_ENV_SECT_SIZE	(128 * 1024)
#define CFG_ENV_SIZE		(CFG_ENV_SECT_SIZE)
#define CFG_ENV_ADDR		(CFG_FLASH_BASE + CFG_MONITOR_LEN)
/* Offset of env Flash sector relative to CFG_FLASH_BASE */
#define CFG_ENV_OFFSET		(CFG_ENV_ADDR - CFG_FLASH_BASE)
#define CFG_ENV_SIZE_REDUND	(CFG_ENV_SECT_SIZE)

/* Board Clock */
#define CONFIG_SYS_CLK_FREQ	33333333
#define TMU_CLK_DIVIDER		(4)	/* 4 (default), 16, 64, 256 or 1024 */
#define CFG_HZ			(CONFIG_SYS_CLK_FREQ / TMU_CLK_DIVIDER)

#endif	/* __MIGO_R_H */
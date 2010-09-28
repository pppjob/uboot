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

#include <common.h>
#include <asm/errno.h>
#include <linux/types.h>
#include <linux/bitops.h>
#include <asm/mach-types.h>
#include <asm/arch-atxx/topctl.h>
#include <asm/arch-atxx/cache.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/adc.h>
#include <asm/arch-atxx/delay.h>
#include <asm/arch-atxx/pm.h>
#include "map_table.c"
#include "keypad.h"
#include "bootparam.h"

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	uint32_t val;

	mmu_cache_on(memory_map);
	at6600_clock_init();

	calibrate_delay();

	val = topctl_read_reg(TOPCTL1);
	val |= (1 << 13);
	topctl_write_reg(TOPCTL1, val);
	/* Initial code from ASIC team for power save */
	topctl_write_reg(TOPCTL3, 0x03700003);
	topctl_write_reg(TOPCTL4, 0xf6ffdfef);
	topctl_write_reg(TOPCTL5, 0x081c4c98);
	topctl_write_reg(TOPCTL6, 0x00f29e94);

	at6600_i2c_init();
	if (-1 == pcf50626_set_default_power_supply())
		return -1;
	
	/* arch number of board */
	gd->bd->bi_arch_number = MACH_TYPE_CAYMAN20;
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_ATAG_ADDR;

	adc_init();
	keypad_init();

	return 0;
}

int misc_init_r(void)
{
	return 1;
}

int dram_init (void)
{
	 return 0;
}



/* Modify bootargs and bootcmd*/
int boot_from_sd(char * str)
{
	int ret = -ENODATA;
	char buffer[CONFIG_SYS_CBSIZE];

	sprintf(buffer, "%s %s%s", 
		CONFIG_BOOTARGS_SD, "liveboot=", str);
	ret = setenv("bootargs", buffer);
	printf("done %s\n", buffer);
	if (ret) {
		printf("set bootargs fail\n");
		return ret;
	}
	memset(buffer, 0, CONFIG_SYS_CBSIZE);
	sprintf(buffer, "%s", 
		CONFIG_BOOTCOMMAND_SD);
	ret = setenv("bootcmd", buffer);
	printf("done %s\n", buffer);
	if (ret) {
		printf("set bootcmd fail\n");
		return ret;
	}
	return 0;
}

/* Modify bootargs and bootcmd*/
int boot_from_nand(void)
{
	int ret = -ENODATA;
	char buffer[CONFIG_SYS_CBSIZE];

	sprintf(buffer, "%s", 
		CONFIG_BOOTARGS_NAND);
	ret = setenv("bootargs", buffer);
	if (ret) {
		printf("set bootargs fail\n");
		return ret;
	}
	sprintf(buffer, "%s", 
		CONFIG_BOOTCOMMAND_NAND);
	ret = setenv("bootcmd", buffer);
	if (ret) {
		printf("set bootcmd fail\n");
		return ret;
	}
	return 0;
}


int build_boot_cmd(enum boot_mode mode)
{
	int ret;

	switch(mode){
		case SD_PHONETEST:
			ret = boot_from_sd("phonetest");
			break;
		case SD_RECOVERY:
			ret = boot_from_sd("recovery");
			break;
		case SD_INSTALL:
			ret = boot_from_sd("install");
			break;
		case NAND_BOOT:
			ret = boot_from_nand();
			break;
		default:
			ret = -EINVAL;
			break;
		}
	return ret;
}

int do_abortboot(void)
{
	unsigned int hwcfg;
	enum boot_mode mode;
	int ret;

	hwcfg = pm_read_reg(HWCFGR);
	if (hwcfg == 2) {
		mode = SD_PHONETEST;
		ret = build_boot_cmd(mode);
	}else if (hwcfg == 0) {
		mode = keypad_detect();
		if (mode != CMD_MODE)
			ret = build_boot_cmd(mode);
		else
			ret = 1;
#if 0
		if (ret == NAND_BOOT){
			ret = on_key_detect();
			if (ret)
				while(1);
		}
#endif
	}else 
		/* abort boot, entry command line */
		return 1;
	return ret;
}

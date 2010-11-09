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
#include <asm/arch-atxx/pmu.h>
#include <asm/arch-atxx/gpio.h>
#include <asm/arch-atxx/map_table.h>
#include <asm/arch-atxx/aboot.h>

#include "keypad.h"

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	uint32_t val;

	mmu_cache_on(memory_map);
	at6600_clock_init();

	calibrate_delay();

	/* NAND-RDY workaround for 32K clk */
	val = topctl_read_reg(TOPCTL1);
	val |= (1 << 13);
	topctl_write_reg(TOPCTL1, val);
	atxx_request_gpio(GPIO_GATE_DISB);
	atxx_set_gpio_direction(GPIO_GATE_DISB, 0);
	atxx_gpio_set(GPIO_GATE_DISB, 1);

	at6600_i2c_init();

	pmu_init();

	adc_init();

	keypad_init();

	/* arch number of board */
	gd->bd->bi_arch_number = MACH_TYPE_T3C;
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_ATAG_ADDR;

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

int do_abortboot(void)
{
	enum boot_mode mode;
	int ret;

	run_command("mmc init", 0);

	mode = hwcfg_detect();
	if (mode != NAND_BOOT) {
		goto non_nand_boot;
	}

	mode = mmc_detect();
	if (mode != NAND_BOOT) {
		goto non_nand_boot;
	}

	mode = keypad_detect();
	if (mode != NAND_BOOT) {
		goto non_nand_boot;
	}

	mode = serial_detect(2);
	if (mode != NAND_BOOT) {
		goto non_nand_boot;
	}

	mode = swcfg_detect();

non_nand_boot:
	ret = build_boot_cmd(mode, "ubifs");
	return ret;
}

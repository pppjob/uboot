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
#include <asm/arch-atxx/map_table.h>
#include <asm/arch-atxx/aboot.h>
#include <asm/arch-atxx/factorydata.h>
#include <asm/arch-atxx/gpio.h>

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
	/*set GPIO24,26 as nand function*/
	val = topctl_read_reg(TOPCTL1);
	val |= (1 << 11);
	topctl_write_reg(TOPCTL1, val);

	atxx_request_gpio(GPIO_GATE_DISB);
	atxx_set_gpio_direction(GPIO_GATE_DISB, 0);
	atxx_gpio_set(GPIO_GATE_DISB, 1);

	/* disable vga dac */
	val = topctl_read_reg(TOPCTL0);
	val |= 0xf0;
	topctl_write_reg(TOPCTL0, val);

	at6600_i2c_init();

	pmu_init();

	adc_init();

	/* arch number of board */
	gd->bd->bi_arch_number = MACH_TYPE_AD8071;
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_ATAG_ADDR;

	return 0;
}

int misc_init_r(void)
{
	factory_data_t *fd = NULL;
	int i;
	struct boot_parameter *b_param = (struct boot_parameter *)MDDR_BASE_ADDR;

	if (!b_param->mddr_data_send) {
		return 0;
	}

	fd = factory_data_get(FD_MDDR);
	if (fd == NULL) {
		return -1;
	}
	fd->fd_index = FD_MDDR;
	fd->fd_length = 9;
	memcpy(fd->fd_buf, (uint8_t *)&b_param->f_mddr, 9);

	if (factory_data_store(fd)) {
		printf("MDDR Factory results save failed\n");
	} else {
		printf("MDDR Factory results save OK\n");
		printf("MDDR size: %dM\n", ((1 << fd->fd_buf[0]) * 64));
		for (i = 1; i < 9; i++)
			printf("data[%d] = 0x%02x\n", i, (fd->fd_buf[i]&0xff));

	}

	factory_data_put(fd);
	return 0;

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

	mode = serial_detect(2);
	if (mode != NAND_BOOT) {
		 goto non_nand_boot;
	}

	mode = swcfg_detect();

non_nand_boot:
	/* download all for auto download mode*/
	if (mode == SD_INSTALL) {
		printf("Donwload xloader.\n");
		run_command("adownload sd xloader", 0);
		printf("Donwload uboot.\n");
		run_command("adownload sd uboot", 0);
	}

	ret = build_boot_cmd(mode, "ubifs");
	return ret;
}

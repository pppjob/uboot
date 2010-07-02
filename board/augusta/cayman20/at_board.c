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

#include <linux/types.h>
#include <asm/arch-atxx/topctl.h>
#include <asm/arch-atxx/cache.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/mddr.h>
#include "clock_table.c"
#include "map_table.c"
int board_init(void)
{
	uint32_t val;

	mmu_cache_on(memory_map);
#if 0
	at6600_i2c_init();
	set_pmu_default();
#endif	
	at6600_clock_init();
	set_board_default_clock(pll_setting, div_setting,
		PLL_DEFSET_COUNT, DIV_DEFSET_COUNT);

	val = topctl_read_reg(TOPCTL1);
	val |= (1 << 13);
	topctl_write_reg(TOPCTL1, val);
	/* Initial code from ASIC team for power save */
	topctl_write_reg(TOPCTL3, 0x03700003);
	topctl_write_reg(TOPCTL4, 0xf6ffdfef);
	topctl_write_reg(TOPCTL5, 0x081c4c98);
	topctl_write_reg(TOPCTL6, 0x00f29e94);
	/* 
	* FIXME: add mddr power down code, not allow 
	* reinit without power down, when self-refresh
	*/
	mddr_init();
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

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
#include <asm/arch-atxx/clock.h>

int print_cpuinfo(void)
{
	
	printf("\rAT6600: ARM %luMHz ",  clk_get_rate(clk_get("arm")) / MHZ);
	printf("AXI %luMHz MDDR %luMHz \n",
	       clk_get_rate(clk_get("axi")) / MHZ, clk_get_rate(clk_get("mddr")) / MHZ);

#ifdef DEBUG_BSP
	dump_clock();
	printf("udelay start\n");
	udelay(1000);
	printf("udelay 1000 end\n");
	udelay(10000);
	printf("udelay 10000 end\n");
	mdelay(100000);
	printf("mdelay 100000 end\n");
	
	printf("get_timer(0)=%x\n",get_timer(0));
	mdelay(100);
	printf("get_timer(0)=%x\n",get_timer(0));
#endif	
	return 0;
}

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

#include <command.h>
#include <common.h>
#include <asm/string.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/cache.h>
#include <asm/arch-atxx/memory_map.h>

static int cache_test(int argc, char *argv[])
{
	char *action;
	int ret = -1;

	if (argc < 3){
		return ret;
	}
	action = argv[2];
	if (!strcmp(action, "on")) {
		printf("cache and mmu on\n");
		mmu_cache_on(memory_map);
		ret = 0;
	}else {
		if (!strcmp(action, "off")){
			printf("cache and mmu off\n");
			mmu_cache_off();
			ret = 0;
		}
	}
	return ret;
}

int do_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = -1;
	char *subcmd;

	if (argc < 2) {
		goto done;
	}
	subcmd = argv[1];

	/* stest cache on/off */
	if (!strcmp(subcmd, "cache")) {
		ret = cache_test(argc, argv);

	/* stest clkd */
	}else if (!strcmp(subcmd, "clkd")) {
			dump_clock();
			ret = 0;
	}
done:
	if (ret < 0)		
		cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	stest,   5,   1,	do_test,
	"self test for: cache, clock dump(clkd)",
	"usage: stest cache on/off, stest clkd"
);


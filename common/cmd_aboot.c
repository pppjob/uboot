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
#include <asm/arch-atxx/aboot.h>

int do_aboot(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc < 2) {
		goto failed;
	}

	if (strcmp(argv[1], "nand") == 0) {
		build_boot_cmd(NAND_BOOT);
	} else if (strcmp(argv[1], "sd") == 0) {
		if (argc == 2) {
			build_boot_cmd(SD_BOOT);
		} else if (strcmp(argv[2], "install") == 0) {
			build_boot_cmd(SD_INSTALL);
		} else if (strcmp(argv[2], "boardtest") == 0) {
			build_boot_cmd(SD_BOARDTEST);
		} else if (strcmp(argv[2], "recovery") == 0) {
			build_boot_cmd(SD_RECOVERY);
		} else {
			goto failed;
		}
	} else {
		goto failed;
	}

	return 0;

failed:
	cmd_usage(cmdtp);

	return -1;
}

U_BOOT_CMD(
	aboot, 3, 0, do_aboot,
	"atxx boot command",
	"usage: aboot nand\n"
	"       aboot sd [install|boardtest|recovery]\n"
);

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
#include <asm/arch-atxx/hdcvt.h>

int do_hdcvt(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32_t addr_from, addr_to;
	atxx_image_header_t *ahd;
	image_header_t *uhd;
	void *body;

	if (argc < 2) {
		goto failed;
	}

	addr_from = simple_strtoul(argv[1], NULL, 16);

	if (argc >= 2) {
		addr_to = simple_strtoul(argv[2], NULL, 16);
	} else {
		addr_to = addr_from + sizeof(atxx_image_header_t) - sizeof(image_header_t);
	}

	if (argc >= 3) {
		addr_to = simple_strtoul(argv[3], NULL, 16);
	} else {
		addr_to = addr_from + sizeof(atxx_image_header_t) - sizeof(image_header_t);
	}

	ahd = (atxx_image_header_t *)addr_from;
	uhd = (image_header_t *)addr_to;

	ahd_to_uhd(ahd, (void *)ahd->run_address, uhd);

	return 0;

failed:
	cmd_usage(cmdtp);

	return -1;
}

U_BOOT_CMD(
	hdcvt, 3, 0, do_hdcvt,
	"covert atxx header to u-boot header",
	"usage: hdcvt addr_from [addr_to] [body_addr]\n"
);


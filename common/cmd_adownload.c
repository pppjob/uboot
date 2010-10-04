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
#include <xyzModem.h>
#include <nand.h>
#include <environment.h>

#include <asm/string.h>
#include <asm/sizes.h>
#include <asm/arch-atxx/hdcvt.h>

#define	DOWNLOAD_ADDR	(MDDR_BASE_ADDR + 16 * SZ_1M)
#define	NANDBAK_ADDR	(MDDR_BASE_ADDR + 32 * SZ_1M)

static int nand_erase_block(int offset, int size)
{
	nand_info_t *nand = &nand_info[0];
	nand_erase_options_t opts;

	memset(&opts, 0, sizeof(opts));
	opts.offset = offset;
	opts.length = size;
	opts.jffs2  = 0;
	opts.quiet  = 1;
	opts.scrub  = 0;

	printf("Erasing 0x%08x, size 0x%08x\n", offset, size);

	return nand_erase_opts(nand, &opts);
}

static int image_to_nand(int addr, int size, int offset)
{
	int ndbeg, ndsz;
	int ret;
	nand_info_t *nand = &nand_info[0];

	ndbeg = offset & ~(SZ_1M - 1);
	ndsz = ((offset + size + SZ_1M - 1) & ~(SZ_1M - 1)) - ndbeg;

	ret = nand_read_skip_bad(nand, ndbeg, &ndsz, (u_char *)NANDBAK_ADDR);
	memcpy((void *)(offset - ndbeg + NANDBAK_ADDR), (void *)addr, size);

	ret |= nand_erase_block(ndbeg, ndsz);

	printf("Writing 0x%08x, size 0x%08x\n", offset, size);

	/* work around for xloader nand ops */
	if (offset == 0) {
		printf("write xloader nand by workaround!\n");
		board_nand_reinit(1);
	}

	ret |= nand_write_skip_bad(nand, ndbeg, &ndsz, (u_char *)NANDBAK_ADDR);

	/* work around for xloader nand ops */
	if (offset == 0) {
		board_nand_reinit(0);
	}

	return ret;
}

int do_adownload(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char cmd[CONFIG_SYS_CBSIZE];
	int addr, size, ret;
	char *fstype = "";
	atxx_image_header_t *head = (atxx_image_header_t *)DOWNLOAD_ADDR;

	if (argc < 2) {
		goto failed;
	}

	if (strcmp(argv[1], "uart") == 0) {
		addr = load_serial_modem(DOWNLOAD_ADDR, xyzModem_xmodem);
		if (addr == ~0) {
			return -1;
		}

		size = (head->firm_size + 512 + 4096) & ~4095;
		ret = image_to_nand(addr, size, head->nand_offset);

	} else if (strcmp(argv[1], "sd") == 0) {
		if (argc == 4) {
			fstype = argv[3];
		}

		if (strcmp(argv[2], "xloader") == 0) {
			sprintf(cmd, "fatload mmc 1 0x%08x xloader.img", DOWNLOAD_ADDR);
			if (run_command(cmd, 0)) {
				return -1;
			}

			size = (head->firm_size + 512 + 4096) & ~4095;
			ret = image_to_nand(DOWNLOAD_ADDR, size, head->nand_offset);

		} else if (strcmp(argv[2], "uboot") == 0) {
			sprintf(cmd, "fatload mmc 1 0x%08x uboot.img", DOWNLOAD_ADDR);
			if (run_command(cmd, 0)) {
				return -1;
			}

			size = (head->firm_size + 512 + 4096) & ~4095;
			ret = image_to_nand(DOWNLOAD_ADDR, size, head->nand_offset);

		} else if (strcmp(argv[2], "system") == 0) {
			sprintf(cmd, "aboot sd install %s", fstype);
			ret = run_command(cmd, 0);

		} else if (strcmp(argv[2], "all") == 0) {
			/* clean nand env */
			printf("clean nand env!\n\n");
			nand_erase_block(CONFIG_ENV_OFFSET, 
					CONFIG_ENV_OFFSET + CONFIG_ENV_RANGE);

			/* xloader */
			sprintf(cmd, "fatload mmc 1 0x%08x xloader.img", DOWNLOAD_ADDR);
			if (run_command(cmd, 0)) {
				return -1;
			}
			size = (head->firm_size + 512 + 4096) & ~4095;
			ret = image_to_nand(DOWNLOAD_ADDR, size, head->nand_offset);

			/* uboot */
			sprintf(cmd, "fatload mmc 1 0x%08x uboot.img", DOWNLOAD_ADDR);
			if (run_command(cmd, 0)) {
				return -1;
			}
			size = (head->firm_size + 512 + 4096) & ~4095;
			ret = image_to_nand(DOWNLOAD_ADDR, size, head->nand_offset);

			/* run livesd install */
			sprintf(cmd, "aboot sd install %s", fstype);
			ret = run_command(cmd, 0);

		} else {
			goto failed;
		}
	} else {
		goto failed;
	}

	return ret;

failed:
	cmd_usage(cmdtp);

	return -1;
}

U_BOOT_CMD(
	adownload, 4, 0, do_adownload,
	"atxx download command",
	"usage:\n"
	"adownload uart\n"
	"adownload sd xloader|uboot\n"
	"adownload sd system|all [ubifs|yaffs]\n"
);


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
#include <asm/string.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/arch-atxx/regs_base.h>
#include <asm/arch-atxx/pm.h>
#include <asm/arch-atxx/aboot.h>

enum boot_mode hwcfg_detect(void)
{
	int hwcfg, mode;

	hwcfg = pm_read_reg(HWCFGR);
	if (hwcfg == 0) {
		mode = NAND_BOOT;
	} else if (hwcfg == 2) {
		mode = SD_BOARDTEST;
	} else
		mode = CMD_MODE;

	return mode;
}

enum boot_mode swcfg_detect(void)
{
	int swcfg, mode;

	swcfg = pm_read_reg(SWCFGR);
	if ((swcfg & SWCFGR_REBOOT_MASK) == SWCFGR_REBOOT_RECOVERY) {
		mode = SD_RECOVERY;
	} else if ((swcfg & SWCFGR_REBOOT_MASK) == SWCFGR_REBOOT_UBOOTCMD) {
		mode = CMD_MODE;
	} else
		mode = NAND_BOOT;

	return mode;
}

extern int tstc(void);

enum boot_mode serial_detect(int num)
{
	int mode;
	int tick = 1000000;
	int count = tick * num;

	/* normal boot */
	mode = NAND_BOOT;
	do {
		if (count % tick == 0) {
			printf("Press KEY to enter cmd mode: %d\r", count / tick);
		}
		if (tstc()) {
			mode = CMD_MODE;
			break;
		}
	} while (count-- > 0);

	return mode;
}

/* Modify bootargs and bootcmd*/
static int boot_from_sd(char * bootstr, char *fstype)
{
	int ret = -ENODATA;
	char *cmd, *args;
	char buffer[CONFIG_SYS_CBSIZE];

	cmd = getenv("bootcmd_sd");
	if (!cmd) {
		printf("get bootcmd_sd failed!\n");
		return ret;
	}

	args = getenv("bootargs_sd");
	if (!args) {
		printf("get bootargs_sd failed!\n");
		return ret;
	}

	if (!fstype) {
		fstype = "nofs";
	}

	if (bootstr) {
		sprintf(buffer, "%s liveboot=%s livefs=%s",
				args, bootstr, fstype);
	} else {
		sprintf(buffer, "%s", args);
	}

	ret = setenv("bootargs", buffer);
	if (ret) {
		printf("set bootargs failed!\n");
		return ret;
	}

	ret = run_command(cmd, 0);

	return ret;
}

/* Modify bootargs and bootcmd*/
int boot_from_nand(void)
{
	int ret = -ENODATA;
	char *cmd;

	cmd = getenv("bootcmd");
	if (!cmd) {
		printf("get bootcmd failed!\n");
		return ret;
	}

	ret = run_command(cmd, 0);

	return ret;
}

int build_boot_cmd(enum boot_mode mode, char *fstype)
{
	int ret;

	printf("Boot mode: %d\n", mode);
	switch (mode){
		case NAND_BOOT:
			printf("Enter NAND boot mode\n");
			ret = boot_from_nand();
			break;
		case SD_BOOT:
			printf("Enter SD boot mode\n");
			ret = boot_from_sd(NULL, fstype);
			break;
		case SD_INSTALL:
			printf("Enter SD install mode\n");
			ret = boot_from_sd("install", fstype);
			break;
		case SD_PLATFORM:
			printf("Enter SD platform mode\n");
			ret = boot_from_sd("platform", fstype);
			break;
		case SD_BOARDTEST:
			printf("Enter SD boardtest mode\n");
			ret = boot_from_sd("boardtest", fstype);
			break;
		case SD_PHONETEST:
			printf("Enter SD phonetest mode\n");
			ret = boot_from_sd("phonetest", fstype);
			break;
		case SD_RECOVERY:
			printf("Enter SD recovery mode\n");
			ret = boot_from_sd("recovery", fstype);
			break;
		case CMD_MODE:
			printf("Enter command mode\n");
			ret = 1;
			break;
		default:
			ret = -EINVAL;
			break;
		}
	return ret;
}


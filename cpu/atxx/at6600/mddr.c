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
 -----------------------------------------------------------------------------*/

#include <linux/types.h>
#include <asm/io.h>
#include <config.h>
#include <common.h>
#include <asm/arch-atxx/io.h>
#include <asm/arch-atxx/mddr.h>

void mddr_core_init(uint32_t size)
{
	write64(0x3ffbe000, 0x0101010000000101ULL);
	write64(0x3ffbe008, 0x0001010100000100ULL);
	write64(0x3ffbe010, 0x0001000100000000ULL);
	write64(0x3ffbe018, 0x0101000000010001ULL);

	write64(0x3ffbe020, 0x0002020000010101ULL);

	if (size == MDDR_256) {
		write64(0x3ffbe028, 0x0003020002000003ULL);
	} else {
		write64(0x3ffbe028, 0x0003020102000003ULL);
	}
	write64(0x3ffbe030, 0x0202020200000000ULL);

	write64(0x3ffbe038, 0x0000020f07060a0fULL);

	write64(0x3ffbe040, 0x0a02000000030500ULL);
	write64(0x3ffbe048, 0x0000000000000000ULL);
	write64(0x3ffbe050, 0x0000000000000000ULL);
	write64(0x3ffbe058, 0x0c030700ff000000ULL);
	write64(0x3ffbe060, 0x0000000005050e00ULL);
	write64(0x3ffbe068, 0x0000ffff00000000ULL);
	write64(0x3ffbe070, 0x0000000000000000ULL);
	write64(0x3ffbe078, 0x00142d8900050000ULL);
	write64(0x3ffbe080, 0x00007a1200000014ULL);
	write64(0x3ffbe088, 0x0000000000000000ULL);
	write64(0x3ffbe090, 0x0000000000000000ULL);
	write64(0x3ffbe098, 0x0000000000000000ULL);
	write64(0x3ffbe0a0, 0x0000000000000000ULL);
	write64(0x3ffbe0a8, 0x0000000000000000ULL);
	write64(0x3ffbe0b0, 0x00000000aaaa8001ULL);
	write64(0x3ffbe0b8, 0x0000000000000000ULL);
	write64(0x3ffbe0c0, 0x0500010002000100ULL);
	write64(0x3ffbe0c8, 0x0000000000000200ULL);
	write64(0x3ffbe0d0, 0x0000000000000000ULL);
	write64(0x3ffbe0d8, 0x00121f1000121f10ULL);
	write64(0x3ffbe0e0, 0x00121f1000121f10ULL);
	write64(0x3ffbe0e8, 0x0012270400122704ULL);
	write64(0x3ffbe0f0, 0x0012270400122704ULL);

	write64(0x3ffbe0f8, 0x0000000000000000ULL);
	write64(0x3ffbe100, 0x0000000000000000ULL);
	write64(0x3ffbe108, 0x13000617000f1133ULL);
	write64(0x3ffbe110, 0x1300061713000617ULL);
	write64(0x3ffbe118, 0x0780000213000617ULL);
	write64(0x3ffbe120, 0x0780000207800002ULL);
	write64(0x3ffbe128, 0x0080000407800002ULL);
	write64(0x3ffbe130, 0x0000000000000000ULL);
	write64(0x3ffbe138, 0x0000000000000000ULL);
	write64(0x3ffbe140, 0x0000000000000000ULL);
	write64(0x3ffbe148, 0x0000000000000000ULL);
	write64(0x3ffbe150, 0x0000000000000000ULL);
	write64(0x3ffbe158, 0x0000000000000000ULL);
	write64(0x3ffbe160, 0x0000000000000000ULL);
	write64(0x3ffbe168, 0x0000000000000000ULL);
	write64(0x3ffbe170, 0x0000000000000000ULL);
	write64(0x3ffbe178, 0x0000000000000000ULL);
	write64(0x3ffbe180, 0x0000000000000000ULL);
	write64(0x3ffbe188, 0x0000000000000000ULL);
	write64(0x3ffbe190, 0x0000000000000000ULL);
	write64(0x3ffbe198, 0x0000000000000000ULL);
	write64(0x3ffbe1a0, 0x0000000000000000ULL);
	write64(0x3ffbe1a8, 0x0000000000000000ULL);
	write64(0x3ffbe1b0, 0x0000000000000000ULL);
	write64(0x3ffbe1b8, 0x0000000000000000ULL);
	write64(0x3ffbe1c0, 0x0000000000000000ULL);
	write64(0x3ffbe1c8, 0x0000000000000000ULL);
	write64(0x3ffbe1d0, 0x0000000000000000ULL);
	write64(0x3ffbe1d8, 0x0000000000000000ULL);
	write64(0x3ffbe1e0, 0x0000000000000000ULL);
	write64(0x3ffbe1e8, 0x0000000000000000ULL);
	write64(0x3ffbe1f0, 0x0000000000000000ULL);
	write64(0x3ffbe1f8, 0x0000000000000000ULL);
	write64(0x3ffbe200, 0x0000000000000000ULL);
	write64(0x3ffbe208, 0x0000000000000000ULL);
	write64(0x3ffbe210, 0x0000000000000000ULL);
	write64(0x3ffbe218, 0x0000000000000000ULL);
	write64(0x3ffbe220, 0x0000000000000000ULL);
	write64(0x3ffbe228, 0x0000000000000000ULL);
	write64(0x3ffbe230, 0x0000000000000000ULL);
	write64(0x3ffbe238, 0x0000000000000000ULL);
	write64(0x3ffbe240, 0x0000000000000000ULL);
	write64(0x3ffbe248, 0x0000000000000000ULL);
	write64(0x3ffbe250, 0x0000000000000000ULL);
	write64(0x3ffbe258, 0x0000000000000000ULL);

	if (size == MDDR_256) {
		write64(0x3ffbe028, 0x0003020002000003ULL);
	} else {
	write64(0x3ffbe028, 0x0003020102000003ULL);
	}

	write64(0x3ffbe018, 0x0101010000010001ULL);
	mdelay(10);
		

}

#define MDDR_128M		    (128 * 1024 * 1024)
#define MDDR_TEST_DATA1		0x12345678
#define MDDR_TEST_DATA2		0x55aa7068

/* check for 128M, 256M only */
static uint32_t cal_get_mddr_size(void)
{
	uint32_t data;

	writel(MDDR_TEST_DATA1, MDDR_BASE_ADDR);
	writel(MDDR_TEST_DATA2, MDDR_BASE_ADDR + MDDR_128M);
	mdelay(5);
	data = readl(MDDR_BASE_ADDR);
	if (data == MDDR_TEST_DATA1) {
		return MDDR_256;
	} else {
		return MDDR_128;
	}

}

static uint32_t get_mddr_size(void)
{
	uint32_t size = 0;

	/* 
	* if saved mddr data are valid in env structure,
	* get size from evionment 
	*/
	if (0) {
			//size = env_get_mddr_size();
			if (size) {
				goto done;
			}
		}
	mddr_core_init(MDDR_256);
	size = cal_get_mddr_size();
	if (size) {
		/* set calibration enable bit to env structure */
		/* save to env structure , and set modify bit */
		goto done;
	}

done:
	return size;

}

void mddr_init(void)
{
	uint32_t mddr_size;
	
	mddr_size = get_mddr_size();
	mddr_core_init(mddr_size);
	/* do mddr calibration */
	/* calibration algorithm should be rewrite using dichotomy */
	return;
}

/* called at the end of system boot */
void mddr_self_refresh(void)
{
	uint64_t val;

	/*
	* Enter low power mode. 40~44 bit: lowpower control;
	* 32~36 bit: lowpower auto enable.
	*/
	write64(0x3ffbe070, 0xff);
	write64(0x3ffbe074, 0xff00);
	
	/*
	* Memory Self-Refresh with Memory and Controller
	* Clock Gating (Mode 5)
	*/
	val = read64(0x3ffbe044);
	val |= 0x5;
	write64(0x3ffbe044, val);
	mdelay(1);
	printf("enable Mddr self refresh mode\n");
}


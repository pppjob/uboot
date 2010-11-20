/*------------------------------------------------------------------------------
* (c) Copyright, Augusta Technology, Inc., 2006-present.
* (c) Copyright, Augusta Technology USA, Inc., 2006-present.
*
* This software, document, web pages, or material (the "Work") is copyrighted
* by its respective copyright owners.  The Work may be confidential and
* proprietary.	The Work may be further protected by one or more patents and
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

#include <asm/arch-atxx/aboot.h>

#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/regs_base.h>
#include <asm/io.h>
#include <asm/arch-atxx/io.h>

/* keypad registers */
#define	KP_SET1_WR		(ATXX_KEYPAD_BASE + 0x80)
#define	KP_SET1_RD		(ATXX_KEYPAD_BASE + 0x40)
#define	KP_SET2_WR		(ATXX_KEYPAD_BASE + 0x84)
#define	KP_SET2_RD		(ATXX_KEYPAD_BASE + 0x44)
#define	KP_IRQ_CLR		(ATXX_KEYPAD_BASE + 0x88)
#define	KP_SELECT_WR		(ATXX_KEYPAD_BASE + 0x8C)
#define	KP_SELECT_RD		(ATXX_KEYPAD_BASE + 0x50)
#define	KP_RESULT_L		(ATXX_KEYPAD_BASE + 0x48)
#define	KP_RESULT_H		(ATXX_KEYPAD_BASE + 0x4C)

#define KP_RESULT_LEN		64

/* KP SET1 values */
#define KP_SET_ENABLE		0x01
#define KP_SET_INTERRUPT	0x02
#define KP_SET_SLEEPING		0x04
#define KP_SET_MASK		0xffff0000

/* KP SET2 values */
#define KP_SET_DIVIDER		(1 << 16)
#define KP_SET_DEBOUNCE		8

/* delay ms after the first key event */
#define REP_DELAY_VAL	400	

/* delay ms after the second key event */
#define REP_PERIOD_VAL	150	

#define KP_VOL_UP	(1 << 17)
#define KP_VOL_DOWN	(1 << 18)
#define KP_HOME		(1 << 2)
#define KP_HOME_VOL_UP_DOWN		(0x00060006)

int keypad_init(void)
{
	struct clk * clk;
	clk= clk_get("a_keypad");
	clk_enable(clk);
	
	/* init keypad hardware */
	writel(KP_SET_DIVIDER | KP_SET_DEBOUNCE, KP_SET2_WR);

	/* config gpio/keypad pin mux, w002: GPIO 0-14 */
	writel(0x7fff, KP_SELECT_WR);

	/* enable keypad */
	writel(KP_SET_MASK | KP_SET_ENABLE, KP_SET1_WR); 

	return 0;
}

enum boot_mode keypad_detect(void)
{
	uint32_t keystate_l;
	uint32_t keystate_h;
	//uint64_t keystate;
	
	keystate_l = readl(KP_RESULT_L);
	keystate_h = readl(KP_RESULT_H);

	/* FIXME it is not correct */
	//keystate = (uint64_t)(keystate_h << 32) | keystate_l;

	printf("keystate_l=%x\n", keystate_l);
	printf("keystate_h=%x\n", keystate_h);

	/* 2 volume + home key */
	if (~keystate_l == KP_HOME_VOL_UP_DOWN) {
		return SD_BOARDTEST;
	}
	/* 2 volume key */
	if (~keystate_l == 
		(KP_VOL_UP|KP_VOL_DOWN)) {
		return CMD_MODE;
	}
	/* volume up + home key */	
	if (~keystate_l == 
		(KP_VOL_UP|KP_HOME)) {
		return SD_RECOVERY;
	}
	/* volume down + home key */	
	if (~keystate_l == 
		(KP_VOL_DOWN|KP_HOME)) {
		return SD_INSTALL;
	}
	/* not all of above */
	return NAND_BOOT;
}


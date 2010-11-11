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
#include <linux/types.h>
#include <linux/err.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <lcd.h>
#include <malloc.h>
#include <asm/arch-atxx/delay.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/regs_base.h>
#include <asm-arm/arch-atxx/pmu.h>

#include "atxxfb.h"

/* LCD controller timing info*/
#define LCD_HFP		7
#define LCD_HBP		4
#define LCD_HSYNC	4
#define LCD_VFP		60
#define LCD_VBP		20
#define LCD_VSYNC	5

/* spi timing info*/
#define TCHW_TIMING		60	/* XCS signal "H" pulse width */
#define TCSS_TIMING		60	/* XCS signal setup time */

/* serial interface mode timing */
#define	TSCYC_TMING	120	/* Serial clock cycle time */
#define TSHW_TIMING	40	/* SCL signal "H" pulse width */
#define TSLW_TIMING	40	/* SCL signal "L" pulse width */
#define TSDS_TIMING	40	/* Data setup time */
#define TSDH_TIMING	40	/* Data hold time */

vidinfo_t panel_info = {
	.vl_col		= LCD_XRES_320,
	.vl_row		= LCD_XRES_480,
	.vl_bpix	= LCD_COLOR16,
};

/*-------------------------------------------------------------------------
 *
 *		command sets for us to control lcd panel
 *
 *-----------------------------------------------------------------------*/
static void spi_write(struct atxxfb *atfb, uint8_t index, uint16_t value)
{
	static uint8_t cmd_buf[] = { 0x74, 0x00, 0x00, };
	static uint8_t cmd_data[] = { 0x76, 0x00, 0x00, };

	cmd_buf[2] = index;
	cmd_data[1] = ((value >> 8) & 0xff);
	cmd_data[2] = (value & 0xff);
	atfb->spi_8bits_cmd(atfb, cmd_buf, sizeof(cmd_buf));
	atfb->spi_8bits_cmd(atfb, cmd_data, sizeof(cmd_data));
}

static void lms350df04init(struct atxxfb *atfb)
{
	uint16_t val;
	printf("lms350df04init\n");

	mdelay(50);
	spi_write(atfb, 0x07, 0x0000);
	mdelay(30);
	spi_write(atfb, 0x11, 0x222F);
	spi_write(atfb, 0x12, 0x0C00);
	spi_write(atfb, 0x13, 0x7FD9);
	spi_write(atfb, 0x76, 0x2213);
	spi_write(atfb, 0x74, 0x0001);
	spi_write(atfb, 0x76, 0x0000);
	spi_write(atfb, 0x10, 0x5604);
	mdelay(30);

	spi_write(atfb, 0x12, 0x0C63);
	mdelay(30);
	spi_write(atfb, 0x01, 0x0B3B);
	spi_write(atfb, 0x02, 0x0300);
	spi_write(atfb, 0x03, 0x3000);//F000
	spi_write(atfb, 0x08, 0x0002);
	spi_write(atfb, 0x09, 0x000B);
	spi_write(atfb, 0x76, 0x2213);
	spi_write(atfb, 0x0B, 0x3340);
	spi_write(atfb, 0x0C, 0x0020);//0020
	spi_write(atfb, 0x1C, 0x7770);
	spi_write(atfb, 0x76, 0x0000);
	spi_write(atfb, 0x0D, 0x0000);
	spi_write(atfb, 0x0E, 0x0500);
	spi_write(atfb, 0x14, 0x0000);
	spi_write(atfb, 0x15, 0x0803);
	spi_write(atfb, 0x16, 0x0000);
	spi_write(atfb, 0x30, 0x0003);//070F
	spi_write(atfb, 0x31, 0x070F);//0300
	spi_write(atfb, 0x32, 0x0D05);//0003
	spi_write(atfb, 0x33, 0x0405);//0003
	spi_write(atfb, 0x34, 0x090D);//090C
	spi_write(atfb, 0x35, 0x0501);//0001
	spi_write(atfb, 0x36, 0x0400);//0001
	spi_write(atfb, 0x37, 0x0504);//0303
	spi_write(atfb, 0x38, 0x0C09);//0F09
	spi_write(atfb, 0x39, 0x010C);//0105

	spi_write(atfb, 0x07, 0x0001);
	mdelay(30);
	spi_write(atfb, 0x07, 0x0101);
	mdelay(30);
	spi_write(atfb, 0x07, 0x0103);
}

static void lms350df04display_on(struct atxxfb *atfb)
{
}

static void lms350df04display_off(struct atxxfb *atfb)
{
	spi_write(atfb, 0x0b, 0x3000);
	spi_write(atfb, 0x07, 0x0102);
	mdelay(30);
	spi_write(atfb, 0x07, 0x0000);
	mdelay(30);
	spi_write(atfb, 0x12, 0x0000);
	spi_write(atfb, 0x10, 0x0600);
	spi_write(atfb, 0x10, 0x0601);
}

static struct pannel_operation lms350df04ops = {
	.initialize = lms350df04init,
	.panel_on = lms350df04display_on,
	.panel_off = lms350df04display_off,
};

int pannel_set_ops(struct atxxfb *atfb)
{
	atfb->pannel_ops = &lms350df04ops;

	atfb->spi_timing.tchw = TCHW_TIMING;
	atfb->spi_timing.tcss = TCSS_TIMING;
	atfb->spi_timing.tscyc = TSCYC_TMING;
	atfb->spi_timing.tsdh = TSDH_TIMING;
	atfb->spi_timing.tsds = TSDS_TIMING;
	atfb->spi_timing.tshw = TSHW_TIMING;
	atfb->spi_timing.tslw = TSLW_TIMING;

	atfb->fb->var.upper_margin = LCD_VBP;
	atfb->fb->var.lower_margin = LCD_VFP;
	atfb->fb->var.vsync_len = LCD_VSYNC;
	atfb->fb->var.left_margin = LCD_HBP;
	atfb->fb->var.right_margin = LCD_HFP;
	atfb->fb->var.hsync_len = LCD_HSYNC;

	atfb->format = RGB16;
	atfb->resolution = ATXX_HVGA;

	return 0;
}

int pannel_set_power(int on_off)
{
	int err;

	if(on_off > 0) {
		pmu_power_control(PPS_LCD, PS_ON);
	} else {
		pmu_power_control(PPS_LCD, PS_OFF);
	}

	return err;
}

void pannel_set_refresh_rate(struct clk *lcd_clk)
{
	/* set pclk to 13MHz using default XO */
	clk_set_rate(lcd_clk, 8670000);
}

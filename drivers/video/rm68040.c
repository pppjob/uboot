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
#define LCD_HFP		3
#define LCD_HBP		3
#define LCD_HSYNC	2
#define LCD_VFP		2
#define LCD_VBP		2
#define LCD_VSYNC	2

/* spi timing info*/
#define TCHW_TIMING	1	/* XCS signal "H" pulse width */
#define TCSS_TIMING	60	/* XCS signal setup time */

/* serial interface mode timing */
#define	TSCYC_TMING	150	/* Serial clock cycle time */
#define TSHW_TIMING	60	/* SCL signal "H" pulse width */
#define TSLW_TIMING	60	/* SCL signal "L" pulse width */
#define TSDS_TIMING	60	/* Data setup time */
#define TSDH_TIMING	60	/* Data hold time */

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
static void rm68040_init(struct atxxfb *atfb)
{
	atfb->set_csx(atfb, 0);

	atfb->spi_9bits_cmd(atfb, 0x11);
	mdelay(120);

	atfb->spi_9bits_cmd(atfb, 0x13);

	atfb->spi_9bits_cmd(atfb, 0xD0);
	atfb->spi_9bits_par(atfb, 0x07);
	atfb->spi_9bits_par(atfb, 0x41);
	atfb->spi_9bits_par(atfb, 0x1D);

	atfb->spi_9bits_cmd(atfb, 0xD1);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x18);
	atfb->spi_9bits_par(atfb, 0x1A);

	atfb->spi_9bits_cmd(atfb, 0xD2);
	atfb->spi_9bits_par(atfb, 0x01);
	atfb->spi_9bits_par(atfb, 0x11);

	atfb->spi_9bits_cmd(atfb, 0xC0);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x3B);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x02);
	atfb->spi_9bits_par(atfb, 0x11);

	atfb->spi_9bits_cmd(atfb, 0xC1);
	atfb->spi_9bits_par(atfb, 0x10);
	atfb->spi_9bits_par(atfb, 0x0B);
	atfb->spi_9bits_par(atfb, 0x88);

	atfb->spi_9bits_cmd(atfb, 0xC5);
	atfb->spi_9bits_par(atfb, 0x02);

	atfb->spi_9bits_cmd(atfb, 0xC8);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x45);
	atfb->spi_9bits_par(atfb, 0x11);
	atfb->spi_9bits_par(atfb, 0x70);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x1F);
	atfb->spi_9bits_par(atfb, 0x77);
	atfb->spi_9bits_par(atfb, 0x01);
	atfb->spi_9bits_par(atfb, 0x77);
	atfb->spi_9bits_par(atfb, 0x07);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xE4);
	atfb->spi_9bits_par(atfb, 0xA0);

#if 0
	atfb->spi_9bits_cmd(atfb, 0xF0);
	atfb->spi_9bits_par(atfb, 0x01);

	atfb->spi_9bits_cmd(atfb, 0xF3);
	atfb->spi_9bits_par(atfb, 0x40);
	atfb->spi_9bits_par(atfb, 0x0A);

	atfb->spi_9bits_cmd(atfb, 0xF7);
	atfb->spi_9bits_par(atfb, 0x80);
#endif

	atfb->spi_9bits_cmd(atfb, 0x36);
	atfb->spi_9bits_par(atfb, 0x0A);

	atfb->spi_9bits_cmd(atfb, 0x3A);
	atfb->spi_9bits_par(atfb, 0x66);

	atfb->spi_9bits_cmd(atfb, 0xB4);
	atfb->spi_9bits_par(atfb, 0x11);

	atfb->spi_9bits_cmd(atfb, 0x2A);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x01);
	atfb->spi_9bits_par(atfb, 0x3F);

	atfb->spi_9bits_cmd(atfb, 0x2B);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x01);
	atfb->spi_9bits_par(atfb, 0xDF);

	mdelay(120);

	atfb->spi_9bits_cmd(atfb, 0x29);
	atfb->spi_9bits_cmd(atfb, 0x2C);

	atfb->set_csx(atfb, 1);
}

static void rm68040_display_on(struct atxxfb *atfb)
{

}

static void rm68040_display_off(struct atxxfb *atfb)
{

}

static struct pannel_operation rm68040_ops = {
	.initialize = rm68040_init,
	.panel_on = rm68040_display_on,
	.panel_off = rm68040_display_off,
};

int pannel_set_ops(struct atxxfb *atfb)
{
	atfb->pannel_ops = &rm68040_ops;

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
	atfb->xres = panel_info.vl_col;
	atfb->yres = panel_info.vl_row;

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
	/* to get 8.67M PCLK for rm68040 screen */
	clk_set_rate(lcd_clk, 6550000);
}

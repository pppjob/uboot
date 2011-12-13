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
#define LCD_HFP_ili9486		3
#define LCD_HBP_ili9486		3
#define LCD_HSYNC_ili9486	2
#define LCD_VFP_ili9486		4 //30//3
#define LCD_VBP_ili9486		2//2 //4
#define LCD_VSYNC_ili9486	2

/* spi timing info*/
#define TCHW_TIMING_ili9486	10	/* XCS signal "H" pulse width */
#define TCSS_TIMING_ili9486	100	/* XCS signal setup time */

/* serial interface mode timing */
#define	TSCYC_TMING_ili9486	150	/* Serial clock cycle time */
#define TSHW_TIMING_ili9486	100	/* SCL signal "H" pulse width */
#define TSLW_TIMING_ili9486	100	/* SCL signal "L" pulse width */
#define TSDS_TIMING_ili9486	100	/* Data setup time */
#define TSDH_TIMING_ili9486	100	/* Data hold time */


vidinfo_t panel_info_ili9486 = {
	.vl_col		= LCD_XRES_320,
	.vl_row		= LCD_XRES_480,
	.vl_bpix	= LCD_COLOR16,
};

/*-------------------------------------------------------------------------
 *
 *		command sets for us to control lcd panel
 *
 *-----------------------------------------------------------------------*/
static void ili9486_init(struct atxxfb *atfb)
{
	atfb->set_csx(atfb, 0);
printf("****ili9486 init!\n");
	atfb->spi_9bits_cmd(atfb, 0x01);
	mdelay(60);

	atfb->spi_9bits_cmd(atfb, 0xC0);
	atfb->spi_9bits_par(atfb, 0x08);
	atfb->spi_9bits_par(atfb, 0x06);

	atfb->spi_9bits_cmd(atfb, 0xC1);
	atfb->spi_9bits_par(atfb, 0x43);

	atfb->spi_9bits_cmd(atfb, 0xC5);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x40);

	atfb->spi_9bits_cmd(atfb, 0xC2);
	atfb->spi_9bits_par(atfb, 0x33);

	atfb->spi_9bits_cmd(atfb, 0xB0);
	//atfb->spi_9bits_par(atfb, 0x80);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xB1);
	atfb->spi_9bits_par(atfb, 0xB0);//90
	atfb->spi_9bits_par(atfb, 0x11);

	atfb->spi_9bits_cmd(atfb, 0xB4);
	atfb->spi_9bits_par(atfb, 0x02);//02

	atfb->spi_9bits_cmd(atfb, 0xB7);
	atfb->spi_9bits_par(atfb, 0x86);//02

	atfb->spi_9bits_cmd(atfb, 0xB6);
	atfb->spi_9bits_par(atfb, 0x22);
	atfb->spi_9bits_par(atfb, 0x22);
	atfb->spi_9bits_par(atfb, 0x3B);

	atfb->spi_9bits_cmd(atfb, 0xE0);
	atfb->spi_9bits_par(atfb, 0x0F);
	atfb->spi_9bits_par(atfb, 0x29);
	atfb->spi_9bits_par(atfb, 0x25);
	atfb->spi_9bits_par(atfb, 0x0B);
	atfb->spi_9bits_par(atfb, 0x0E);
	atfb->spi_9bits_par(atfb, 0x07);
	atfb->spi_9bits_par(atfb, 0x42);
	atfb->spi_9bits_par(atfb, 0x87);
	atfb->spi_9bits_par(atfb, 0x2C);
	atfb->spi_9bits_par(atfb, 0x06);
	atfb->spi_9bits_par(atfb, 0x0F);
	atfb->spi_9bits_par(atfb, 0x02);
	atfb->spi_9bits_par(atfb, 0x0B);
	atfb->spi_9bits_par(atfb, 0x07);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xE1);
	atfb->spi_9bits_par(atfb, 0x0F);
	atfb->spi_9bits_par(atfb, 0x38);
	atfb->spi_9bits_par(atfb, 0x34);
	atfb->spi_9bits_par(atfb, 0x0D);
	atfb->spi_9bits_par(atfb, 0x10);
	atfb->spi_9bits_par(atfb, 0x09);
	atfb->spi_9bits_par(atfb, 0x53);
	atfb->spi_9bits_par(atfb, 0x87);
	atfb->spi_9bits_par(atfb, 0x3D);
	atfb->spi_9bits_par(atfb, 0x08);
	atfb->spi_9bits_par(atfb, 0x11);
	atfb->spi_9bits_par(atfb, 0x04);
	atfb->spi_9bits_par(atfb, 0x1A);
	atfb->spi_9bits_par(atfb, 0x16);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xF2);
	atfb->spi_9bits_par(atfb, 0x1E);
	atfb->spi_9bits_par(atfb, 0xA3);
	atfb->spi_9bits_par(atfb, 0x32);
	atfb->spi_9bits_par(atfb, 0x02);
	atfb->spi_9bits_par(atfb, 0xB2);
	atfb->spi_9bits_par(atfb, 0x52);
	atfb->spi_9bits_par(atfb, 0xFF);
	atfb->spi_9bits_par(atfb, 0x10);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xF8);
	atfb->spi_9bits_par(atfb, 0x21);
	atfb->spi_9bits_par(atfb, 0x04);

	atfb->spi_9bits_cmd(atfb, 0x36);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0x3A);
	atfb->spi_9bits_par(atfb, 0x66);

	atfb->spi_9bits_cmd(atfb, 0xF9);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x08);

	atfb->spi_9bits_cmd(atfb, 0x21);
	atfb->spi_9bits_cmd(atfb, 0x11);
	mdelay(120);
	atfb->spi_9bits_cmd(atfb, 0x29);
	mdelay(10);
	atfb->set_csx(atfb, 1);
}


static void ili9486_display_on(struct atxxfb *atfb)
{

}

static void ili9486_display_off(struct atxxfb *atfb)
{
	 atfb->spi_9bits_cmd(atfb, 0x28);
	 mdelay(10);
	 atfb->spi_9bits_cmd(atfb, 0x10);
	 mdelay(120);
}


static struct pannel_operation ili9486_ops = {
	.initialize = ili9486_init,
	.panel_on = ili9486_display_on,
	.panel_off = ili9486_display_off,
};

int pannel_set_ops_ili9486(struct atxxfb *atfb)
{
	atfb->pannel_ops = &ili9486_ops;

	atfb->spi_timing.tchw = TCHW_TIMING_ili9486;
	atfb->spi_timing.tcss = TCSS_TIMING_ili9486;
	atfb->spi_timing.tscyc= TSCYC_TMING_ili9486;
	atfb->spi_timing.tsdh = TSDH_TIMING_ili9486;
	atfb->spi_timing.tsds = TSDS_TIMING_ili9486;
	atfb->spi_timing.tshw = TSHW_TIMING_ili9486;
	atfb->spi_timing.tslw = TSLW_TIMING_ili9486;

	atfb->fb->var.upper_margin = LCD_VBP_ili9486;
	atfb->fb->var.lower_margin = LCD_VFP_ili9486;
	atfb->fb->var.vsync_len = LCD_VSYNC_ili9486;
	atfb->fb->var.left_margin = LCD_HBP_ili9486;
	atfb->fb->var.right_margin = LCD_HFP_ili9486;
	atfb->fb->var.hsync_len = LCD_HSYNC_ili9486;


	atfb->format = RGB16;
	atfb->xres = panel_info_ili9486.vl_col;
	atfb->yres = panel_info_ili9486.vl_row;
	panel_info = panel_info_ili9486;

	return 0;
}

int pannel_set_power_ili9486(int on_off)
{
	int err = 0;

	if(on_off > 0) {
		err = pmu_power_control(PPS_LCD, PS_ON);
	} else {
		err = pmu_power_control(PPS_LCD, PS_OFF);
	}

	return err;
}

void pannel_set_refresh_rate_ili9486(struct clk *lcd_clk)
{
	/* to get 8.67M PCLK for ILI9486 screen */
	clk_set_rate(lcd_clk, 8670000);
}

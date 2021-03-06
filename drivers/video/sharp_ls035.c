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
#define LCD_HFP_SHARP_LS035		16
#define LCD_HBP_SHARP_LS035		24
#define LCD_HSYNC_SHARP_LS035	16
#define LCD_VFP_SHARP_LS035		4
#define LCD_VBP_SHARP_LS035		2
#define LCD_VSYNC_SHARP_LS035	2

/* spi timing info*/
#define TCHW_TIMING_SHARP_LS035	120	/* XCS signal "H" pulse width */
#define TCSS_TIMING_SHARP_LS035	60	/* XCS signal setup time */

/* serial interface mode timing */
#define	TSCYC_TMING_SHARP_LS035	100	/* Serial clock cycle time */
#define TSHW_TIMING_SHARP_LS035	60	/* SCL signal "H" pulse width */
#define TSLW_TIMING_SHARP_LS035	60	/* SCL signal "L" pulse width */
#define TSDS_TIMING_SHARP_LS035	60	/* Data setup time */
#define TSDH_TIMING_SHARP_LS035	60	/* Data hold time */

vidinfo_t panel_info_sharp_ls035= {
        .vl_col         = LCD_XRES_480,
        .vl_row         = LCD_XRES_800,
        .vl_bpix        = LCD_COLOR16,
};

/*-------------------------------------------------------------------------
 *
 *		command sets for us to control lcd panel
 *
 *-----------------------------------------------------------------------*/
static void sharp035_init(struct atxxfb *atfb)
{
	atfb->set_csx(atfb, 0);

	atfb->spi_9bits_cmd(atfb, 0x29);
	atfb->spi_9bits_cmd(atfb, 0x11);
	mdelay(350);

	atfb->spi_9bits_cmd(atfb, 0x36);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0x3a);
	atfb->spi_9bits_par(atfb, 0x70);

	atfb->spi_9bits_cmd(atfb, 0xb0);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xb8);
	atfb->spi_9bits_par(atfb, 0x01);

	atfb->spi_9bits_cmd(atfb, 0xb9);
	atfb->spi_9bits_par(atfb, 0x01);
	atfb->spi_9bits_par(atfb, 0xff);

	atfb->spi_9bits_cmd(atfb, 0xb0);
	atfb->spi_9bits_par(atfb, 0x03);

	mdelay(100);

	atfb->set_csx(atfb, 1);
}

static void sharp035_display_on(struct atxxfb *atfb)
{
	return;
}

static void sharp035_display_off(struct atxxfb *atfb)
{
	return;
}

static struct pannel_operation sharp035_ops = {
	.initialize = sharp035_init,
	.panel_on = sharp035_display_on,
	.panel_off = sharp035_display_off,
};

int pannel_set_ops_sharp_ls035(struct atxxfb *atfb)
{
	atfb->pannel_ops = &sharp035_ops;

	atfb->spi_timing.tchw = TCHW_TIMING_SHARP_LS035;
	atfb->spi_timing.tcss = TCSS_TIMING_SHARP_LS035;
	atfb->spi_timing.tscyc = TSCYC_TMING_SHARP_LS035;
	atfb->spi_timing.tsdh = TSDH_TIMING_SHARP_LS035;
	atfb->spi_timing.tsds = TSDS_TIMING_SHARP_LS035;
	atfb->spi_timing.tshw = TSHW_TIMING_SHARP_LS035;
	atfb->spi_timing.tslw = TSLW_TIMING_SHARP_LS035;

	atfb->fb->var.upper_margin = LCD_VBP_SHARP_LS035;
	atfb->fb->var.lower_margin = LCD_VFP_SHARP_LS035;
	atfb->fb->var.vsync_len = LCD_VSYNC_SHARP_LS035;
	atfb->fb->var.left_margin = LCD_HBP_SHARP_LS035;
	atfb->fb->var.right_margin = LCD_HFP_SHARP_LS035;
	atfb->fb->var.hsync_len = LCD_HSYNC_SHARP_LS035;

	atfb->format = RGB16;
	atfb->xres = panel_info_sharp_ls035.vl_col;
	atfb->yres = panel_info_sharp_ls035.vl_row;
	panel_info = panel_info_sharp_ls035;

	return 0;
}

int pannel_set_power_sharp_ls035(int on_off)
{
	int err;

	if(on_off > 0){
		pmu_power_control(PPS_LCD, PS_ON);
	}else{
		pmu_power_control(PPS_LCD, PS_OFF);
	}

	return err;
}

void pannel_set_refresh_rate_sharp_ls035(struct clk *lcd_clk)
{
	/* set 26M PCLK for sharp screen */
	clk_set_rate(lcd_clk, 26000000);
}

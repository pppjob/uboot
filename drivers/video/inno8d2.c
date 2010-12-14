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
#include <asm/arch-atxx/gpio.h>
#include <asm/arch-atxx/delay.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/regs_base.h>
#include <asm-arm/arch-atxx/pmu.h>
#include <asm-arm/arch-atxx/topctl.h>

#include "atxxfb.h"

/* LCD controller timing info*/
#define LCD_HFP		210
#define LCD_HBP		40
#define LCD_HSYNC	6
#define LCD_VFP		12
#define LCD_VBP		14
#define LCD_VSYNC	9

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
	.vl_col		= LCD_XRES_800,
	.vl_row		= LCD_YRES_600,
	.vl_bpix	= LCD_COLOR16,
};

/*-------------------------------------------------------------------------
 *
 *		command sets for us to control lcd panel
 *
 *-----------------------------------------------------------------------*/
static void inno8d2_init(struct atxxfb *atfb)
{
	return;
}

static void inno8d2_display_on(struct atxxfb *atfb)
{
	return;
}

static void inno8d2_display_off(struct atxxfb *atfb)
{
	return;
}

static struct pannel_operation inno8d2_ops = {
	.initialize = inno8d2_init,
	.panel_on = inno8d2_display_on,
	.panel_off = inno8d2_display_off,
};

int pannel_set_ops(struct atxxfb *atfb)
{
	atfb->pannel_ops = &inno8d2_ops;

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
	atfb->resolution = ATXX_SVGA;
	
	return 0;
}

int pannel_set_power(int on_off)
{
	int err;

	set_backlight(1, 0);

	err = atxx_request_gpio(GPIO_LCD_PWEN_AVDD);
	if (err) {
		printf("Failed to request gpio pmu_pwren1 %d!\n", err);
		return err;
	}

	if(on_off > 0){
		pmu_power_control(PPS_LCD, PS_ON);
		mdelay(20);
		atxx_set_gpio_direction(GPIO_LCD_PWEN_AVDD, 0);
		atxx_gpio_set(GPIO_LCD_PWEN_AVDD, 0);
	}else{
		atxx_set_gpio_direction(GPIO_LCD_PWEN_AVDD, 1);
		mdelay(20);
		pmu_power_control(PPS_LCD, PS_OFF);
	}

	atxx_free_gpio(GPIO_LCD_PWEN_AVDD);

	return err;
}

void pannel_set_refresh_rate(struct clk *lcd_clk)
{
	clk_set_parent(lcd_clk, clk_get("pll1"));
	clk_set_rate(lcd_clk, 39000000);
}


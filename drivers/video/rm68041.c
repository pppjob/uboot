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
#define LCD_HFP_rm68041		3
#define LCD_HBP_rm68041		3
#define LCD_HSYNC_rm68041	2
#define LCD_VFP_rm68041		6
#define LCD_VBP_rm68041		10
#define LCD_VSYNC_rm68041	2

/* spi timing info*/
#define TCHW_TIMING_rm68041	10	/* XCS signal "H" pulse width */
#define TCSS_TIMING_rm68041	100	/* XCS signal setup time */
#define	TSCYC_TMING_rm68041	150	/* Serial clock cycle time */
#define TSHW_TIMING_rm68041	100	/* SCL signal "H" pulse width */
#define TSLW_TIMING_rm68041	100	/* SCL signal "L" pulse width */
#define TSDS_TIMING_rm68041	100	/* Data setup time */
#define TSDH_TIMING_rm68041	100	/* Data hold time */


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

#define	ILI9468ID		0x9486
#define RM68041			0X40FF 		////??

enum lcdtypesup
{
	LCD_RM68041=1,
	LCD_ILI9468=2,
};

uint8_t lcdtypeid;
vidinfo_t panel_info_rm68041 = {
	.vl_col		= LCD_XRES_320,
	.vl_row		= LCD_XRES_480,
	.vl_bpix	= LCD_COLOR16,
};


/**********************************
 add read LCD id
***********************************/
static void readlcdid(struct atxxfb *atfb)
{
	uint32_t id=0,temp;

	mdelay(10);
	atfb->set_csx(atfb, 0);
	atfb->spi_9bits_cmd(atfb,0xFB);
	atfb->spi_9bits_par(atfb,0x11);
	atfb->spi_9bits_cmd(atfb,0xD3);
	temp = atfb->spi_8bits_read(atfb);
	printf("lcd data1=%x\r\n",temp);
	id|=temp<<16;

	atfb->spi_9bits_cmd(atfb,0xFB);
	atfb->spi_9bits_par(atfb,0x12);
	atfb->spi_9bits_cmd(atfb,0xD3);
	temp = atfb->spi_8bits_read(atfb);
	printf("lcd data2=%x\r\n",temp);
	id|=temp<<8;

	atfb->spi_9bits_cmd(atfb,0xFB);
	atfb->spi_9bits_par(atfb,0x13);
	atfb->spi_9bits_cmd(atfb,0xD3);
	temp = atfb->spi_8bits_read(atfb);
	printf("lcd data3=%x\r\n",temp);
	id|=temp;

	atfb->spi_9bits_cmd(atfb,0xFB);
	atfb->spi_9bits_par(atfb,0x00);

	mdelay(1);
	atfb->set_csx(atfb, 1);
	if(id==ILI9468ID)
		lcdtypeid=LCD_ILI9468;
	else
		lcdtypeid=LCD_RM68041;

}


/*-------------------------------------------------------------------------
 *
 *		command sets for us to control lcd panel
 *
 *-----------------------------------------------------------------------*/
static void rm68041_init(struct atxxfb *atfb)
{
	atfb->set_csx(atfb, 0);

	atfb->spi_9bits_cmd(atfb, 0x11);
	mdelay(10);

	atfb->spi_9bits_cmd(atfb, 0xD0);
	atfb->spi_9bits_par(atfb, 0x07);
	atfb->spi_9bits_par(atfb, 0x41);
	atfb->spi_9bits_par(atfb, 0x1D);

	atfb->spi_9bits_cmd(atfb, 0xD1);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x14);
	atfb->spi_9bits_par(atfb, 0x12);

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
	atfb->spi_9bits_par(atfb, 0x13);
	atfb->spi_9bits_par(atfb, 0x88);


	atfb->spi_9bits_cmd(atfb, 0xC5);
	atfb->spi_9bits_par(atfb, 0x01);

	atfb->spi_9bits_cmd(atfb, 0xC8);
	atfb->spi_9bits_par(atfb, 0x02);
	atfb->spi_9bits_par(atfb, 0x77);
	atfb->spi_9bits_par(atfb, 0x77);
	atfb->spi_9bits_par(atfb, 0x05);
	atfb->spi_9bits_par(atfb, 0x03);
	atfb->spi_9bits_par(atfb, 0x01);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x57);
	atfb->spi_9bits_par(atfb, 0x50);
	atfb->spi_9bits_par(atfb, 0x01);
	atfb->spi_9bits_par(atfb, 0x03);
	mdelay(10);

	atfb->spi_9bits_cmd(atfb, 0xF8);
	atfb->spi_9bits_par(atfb, 0x01);

	atfb->spi_9bits_cmd(atfb, 0xFE);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x02);

	atfb->spi_9bits_cmd(atfb, 0x36);
	atfb->spi_9bits_par(atfb, 0x0A);

	atfb->spi_9bits_cmd(atfb, 0x3A);
	atfb->spi_9bits_par(atfb, 0x66);

	atfb->spi_9bits_cmd(atfb, 0xC6);
	atfb->spi_9bits_par(atfb, 0x82);

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

	atfb->spi_9bits_cmd(atfb, 0x29);

	atfb->set_csx(atfb, 1);
}

static void ili9486_init(struct atxxfb *atfb)
{

	atfb->set_csx(atfb, 0);

	atfb->spi_9bits_cmd(atfb, 0x01);
	mdelay(60);

	atfb->spi_9bits_cmd(atfb, 0xC0);
	atfb->spi_9bits_par(atfb, 0x0F);
	atfb->spi_9bits_par(atfb, 0x0F);

	atfb->spi_9bits_cmd(atfb, 0xC1);
	atfb->spi_9bits_par(atfb, 0x41);

	atfb->spi_9bits_cmd(atfb, 0xC5);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x3A);

	atfb->spi_9bits_cmd(atfb, 0xB0);
	//atfb->spi_9bits_par(atfb, 0x80);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xB1);
	atfb->spi_9bits_par(atfb, 0x90);//90
	atfb->spi_9bits_par(atfb, 0x11);

	atfb->spi_9bits_cmd(atfb, 0xB4);
	atfb->spi_9bits_par(atfb, 0x02);//02

	atfb->spi_9bits_cmd(atfb, 0xB6);
	atfb->spi_9bits_par(atfb, 0x22);
	atfb->spi_9bits_par(atfb, 0x22);
	atfb->spi_9bits_par(atfb, 0x3B);

	atfb->spi_9bits_cmd(atfb, 0xB7);
	atfb->spi_9bits_par(atfb, 0xC6);

	atfb->spi_9bits_cmd(atfb, 0xE0);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x0B);
	atfb->spi_9bits_par(atfb, 0x10);
	atfb->spi_9bits_par(atfb, 0x03);
	atfb->spi_9bits_par(atfb, 0x10);
	atfb->spi_9bits_par(atfb, 0x08);
	atfb->spi_9bits_par(atfb, 0x33);
	atfb->spi_9bits_par(atfb, 0x89);
	atfb->spi_9bits_par(atfb, 0x48);
	atfb->spi_9bits_par(atfb, 0x07);
	atfb->spi_9bits_par(atfb, 0x0E);
	atfb->spi_9bits_par(atfb, 0x0C);
	atfb->spi_9bits_par(atfb, 0x28);
	atfb->spi_9bits_par(atfb, 0x2D);
	atfb->spi_9bits_par(atfb, 0x0F);

	atfb->spi_9bits_cmd(atfb, 0xE1);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x12);
	atfb->spi_9bits_par(atfb, 0x17);
	atfb->spi_9bits_par(atfb, 0x03);
	atfb->spi_9bits_par(atfb, 0x11);
	atfb->spi_9bits_par(atfb, 0x08);
	atfb->spi_9bits_par(atfb, 0x37);
	atfb->spi_9bits_par(atfb, 0x67);
	atfb->spi_9bits_par(atfb, 0x4C);
	atfb->spi_9bits_par(atfb, 0x07);
	atfb->spi_9bits_par(atfb, 0x0F);
	atfb->spi_9bits_par(atfb, 0x0C);
	atfb->spi_9bits_par(atfb, 0x2F);
	atfb->spi_9bits_par(atfb, 0x34);
	atfb->spi_9bits_par(atfb, 0x0F);

	atfb->spi_9bits_cmd(atfb, 0xF2);
	atfb->spi_9bits_par(atfb, 0x18);
	atfb->spi_9bits_par(atfb, 0xA3);
	atfb->spi_9bits_par(atfb, 0x12);
	atfb->spi_9bits_par(atfb, 0x02);
	atfb->spi_9bits_par(atfb, 0xB2);
	atfb->spi_9bits_par(atfb, 0x12);
	atfb->spi_9bits_par(atfb, 0xFF);
	atfb->spi_9bits_par(atfb, 0x10);
	atfb->spi_9bits_par(atfb, 0x00);

	atfb->spi_9bits_cmd(atfb, 0xF7);
	atfb->spi_9bits_par(atfb, 0xA9);
	atfb->spi_9bits_par(atfb, 0x91);
	atfb->spi_9bits_par(atfb, 0x2D);
	atfb->spi_9bits_par(atfb, 0x0A);
	atfb->spi_9bits_par(atfb, 0x4F);

	atfb->spi_9bits_cmd(atfb, 0xF8);
	atfb->spi_9bits_par(atfb, 0x21);
	atfb->spi_9bits_par(atfb, 0x04);

	atfb->spi_9bits_cmd(atfb, 0x36);
	atfb->spi_9bits_par(atfb, 0x08);

	atfb->spi_9bits_cmd(atfb, 0x3A);
	atfb->spi_9bits_par(atfb, 0x66);

	atfb->spi_9bits_cmd(atfb, 0xF9);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x08);

	atfb->spi_9bits_cmd(atfb, 0xF4);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x00);
	atfb->spi_9bits_par(atfb, 0x08);
	atfb->spi_9bits_par(atfb, 0x91);
	atfb->spi_9bits_par(atfb, 0x04);

	atfb->spi_9bits_cmd(atfb, 0x21);
	atfb->spi_9bits_cmd(atfb, 0x11);
	mdelay(120);
	atfb->spi_9bits_cmd(atfb, 0x29);
	mdelay(10);
	atfb->set_csx(atfb, 1);
}



static void rm68041_display_on(struct atxxfb *atfb)
{

}

static void ili9486_display_on(struct atxxfb *atfb)
{

}

static void rm68041_display_off(struct atxxfb *atfb)
{

}

static void ili9486_display_off(struct atxxfb *atfb)
{
	 atfb->spi_9bits_cmd(atfb, 0x28);
	 mdelay(10);
	 atfb->spi_9bits_cmd(atfb, 0x10);
	 mdelay(120);
}

static struct pannel_operation rm68041_ops = {
	.initialize = rm68041_init,
	.panel_on = rm68041_display_on,
	.panel_off = rm68041_display_off,
};


static struct pannel_operation ili9486_ops = {
	.initialize = ili9486_init,
	.panel_on = ili9486_display_on,
	.panel_off = ili9486_display_off,
};

int pannel_set_ops_rm68041(struct atxxfb *atfb)
{
	readlcdid(atfb);

	if(lcdtypeid==LCD_RM68041)
	{
		atfb->pannel_ops = &rm68041_ops;

		atfb->spi_timing.tchw = TCHW_TIMING_rm68041;
		atfb->spi_timing.tcss = TCSS_TIMING_rm68041;
		atfb->spi_timing.tscyc = TSCYC_TMING_rm68041;
		atfb->spi_timing.tsdh = TSDH_TIMING_rm68041;
		atfb->spi_timing.tsds = TSDS_TIMING_rm68041;
		atfb->spi_timing.tshw = TSHW_TIMING_rm68041;
		atfb->spi_timing.tslw = TSLW_TIMING_rm68041;

		atfb->fb->var.upper_margin = LCD_VBP_rm68041;
		atfb->fb->var.lower_margin = LCD_VFP_rm68041;
		atfb->fb->var.vsync_len = LCD_VSYNC_rm68041;
		atfb->fb->var.left_margin = LCD_HBP_rm68041;
		atfb->fb->var.right_margin = LCD_HFP_rm68041;
		atfb->fb->var.hsync_len = LCD_HSYNC_rm68041;
	}
	else if(lcdtypeid==LCD_ILI9468)
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

	}

	atfb->format = RGB16;
	atfb->xres = panel_info_rm68041.vl_col;
	atfb->yres = panel_info_rm68041.vl_row;
	panel_info = panel_info_rm68041;

	return 0;
}

int pannel_set_power_rm68041(int on_off)
{
	int err = 0;

	if(on_off > 0) {
		err = pmu_power_control(PPS_LCD, PS_ON);
	} else {
		err = pmu_power_control(PPS_LCD, PS_OFF);
	}

	return err;
}

void pannel_set_refresh_rate_rm68041(struct clk *lcd_clk)
{
	/* to get 8.67M PCLK for RM68040 screen */
	clk_set_rate(lcd_clk, 8670000);
}

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
#define LCD_HFP_SSD2805C		40//500
#define LCD_HBP_SSD2805C		20//500
#define LCD_HSYNC_SSD2805C		6
#define LCD_VFP_SSD2805C		4
#define LCD_VBP_SSD2805C		2//4
#define LCD_VSYNC_SSD2805C		9

/* spi timing info*/
#define TCHW_TIMING_SSD2805C	1	/* XCS signal "H" pulse width */
#define TCSS_TIMING_SSD2805C	60	/* XCS signal setup time */

/* serial interface mode timing */
#define	TSCYC_TMING_SSD2805C	125	/* Serial clock cycle time */
#define TSHW_TIMING_SSD2805C	11	/* SCL signal "H" pulse width */
#define TSLW_TIMING_SSD2805C	11	/* SCL signal "L" pulse width */
#define TSDS_TIMING_SSD2805C	10	/* Data setup time */
#define TSDH_TIMING_SSD2805C	10	/* Data hold time */


vidinfo_t panel_info_ssd2805c = {
	.vl_col		= LCD_XRES_320,
	.vl_row		= LCD_XRES_480,
	.vl_bpix	= LCD_COLOR16,
};
static void mipi_lreg_w(struct atxxfb *atfb, uint8_t addr, uint16_t data) {
	//local register write
	atfb->set_csx(atfb, 0);
	mdelay(1);

	atfb->spi_9bits_cmd(atfb, addr);
	atfb->spi_9bits_par(atfb, (data & 0xFF));
	mdelay(1);
	atfb->spi_9bits_par(atfb, ((data >> 8) & 0xFF));
	mdelay(1);
	atfb->set_csx(atfb, 1);
	return;
}

static uint16_t mipi_lreg_r(struct atxxfb *atfb, uint8_t addr) {
	uint8_t temp_lo, temp_hi;

	atfb->set_csx(atfb, 0);
	mdelay(1);
	atfb->spi_9bits_cmd(atfb, addr);
	atfb->spi_9bits_cmd(atfb, 0xFA);
	mdelay(20);
	temp_lo = atfb->spi_8bits_read(atfb);
	mdelay(20);
	temp_hi = atfb->spi_8bits_read(atfb);
	mdelay(1);
	atfb->set_csx(atfb, 1);
	return ((temp_hi << 8) | temp_lo);
}

static void ssd2805_dcs_reg_w(struct atxxfb *atfb, uint8_t addr, uint8_t *data,
		int16_t length) {
	int i=0;
	int16_t status;

	mipi_lreg_w(atfb, 0xBC, length);
	status = mipi_lreg_r(atfb, 0xB7);
	status |= 0x40;
	status &= ~0x80;
	mipi_lreg_w(atfb, 0xB7, status);
	status = 0;
	while (status == 0) {
		status = mipi_lreg_r(atfb, 0xC6);
		status &= 0x2;
		if(i++>20)
			break;
	}
	atfb->set_csx(atfb, 0);
	mdelay(1);
	atfb->spi_9bits_cmd(atfb, addr);
	for (i = 0; i < length; i++) {
		atfb->spi_9bits_par(atfb, *(data + i));
	}

	mdelay(1);
	atfb->set_csx(atfb, 1);
	return;

}

static void ssd2805_generic_reg_w(struct atxxfb *atfb, uint8_t *data,
		int16_t length) {
	int i=0;
	int16_t status;

	mipi_lreg_w(atfb, 0xBC, length);
	status = mipi_lreg_r(atfb, 0xB7);
	status &= ~0x40;
	status &= ~0x80;
	mipi_lreg_w(atfb, 0xB7, status);

	status = 0;
	while (status == 0) {
		status = mipi_lreg_r(atfb, 0xC6);
		status &= 0x2;
		if(i++>20)
			break;
	}

	atfb->set_csx(atfb, 0);
	mdelay(1);

	atfb->spi_9bits_cmd(atfb, 0xBF);
	for (i = 0; i < length; i++) {
		atfb->spi_9bits_par(atfb, *(data + i));
	}

	mdelay(1);
	atfb->set_csx(atfb, 1);

	return;
}

/*-------------------------------------------------------------------------
 *
 *		command sets for us to control lcd panel
 *
 *-----------------------------------------------------------------------*/
static void ssd2805c_init(struct atxxfb *atfb) {

	int i=0;
	unsigned int data, count;
	unsigned char param[10], read_data[256];

	data = mipi_lreg_r(atfb, 0xB0);
	printk("SSD2805 ID=%x\r\n", data);

	mipi_lreg_w(atfb, 0xB9, 0x0000); //disalbe PLL
	mipi_lreg_w(atfb, 0xB7, 0x0142); //set TX_CLK for PLL
	mipi_lreg_w(atfb, 0xBA, 0x101A); //set PLL P N M: P=2, N=1, M=27, PLL=20X27/(1X2)=270MHz
	mipi_lreg_w(atfb, 0xB9, 0x0001); //enable PLL

	data = 0;
	while (data == 0) {
		data = mipi_lreg_r(atfb, 0xC6);
		data &= 0x80;
		if(i++>20)
			break;
	} //wait for PLL to lock

	//htydbg spi_init(0x3);		//make sure spi_clk is no more than 10MHz

	//LP clock and VC setting
	mipi_lreg_w(atfb, 0xBB, 0x0004); //SYS_CLK=TX_CLK 0x0004
	mipi_lreg_w(atfb, 0xB8, 0x0000); //virtual channel control

	//delay adjustment
	mipi_lreg_w(atfb, 0xC9, 0x0E05);
	mipi_lreg_w(atfb, 0xCA, 0x1A05);
	mipi_lreg_w(atfb, 0xCB, 0x0441);
	mipi_lreg_w(atfb, 0xCC, 0x1008);
	mipi_lreg_w(atfb, 0xCF, 0xFFFF);
	mipi_lreg_w(atfb, 0xD0, 0xFFFF);

	//RGB interface timing
	mipi_lreg_w(atfb, 0xB2, 0x0E1C); //VBP=0X0E  HBP=0X1C
	mipi_lreg_w(atfb, 0xB3, 0x060C); //VFP=0X06 HFP=0X0C
	mipi_lreg_w(atfb, 0xB4, 0x0140); //HAP=320
	mipi_lreg_w(atfb, 0xB5, 0x01E0); //VAP=480
	//for 1961, use this setting, as there is only 18 bit data bus
	//mipi_lreg_w(atfb,0xB6, 0x000A);

	//for 1963, use this setting, as there is 24 bit data bus
	mipi_lreg_w(atfb, 0xB6, 0x000B);

	ssd2805_dcs_reg_w(atfb, 0x00, param, 0); //NOP

	ssd2805_dcs_reg_w(atfb, 0x11, param, 0); //exit sleep mode

	param[0] = 0x5A;
	ssd2805_dcs_reg_w(atfb, 0xF0, param, 1);
	ssd2805_dcs_reg_w(atfb, 0xF1, param, 1);

	mdelay(200);

#if 0
	ssd2805_dcs_reg_w(atfb,0x12, param, 0); //enter partial mode
	//delay for 15ms or more
	mdelay(20);

	param[0]=0x00;
	param[1]=0x00;
	param[2]=0x00;
	param[3]=0x90;
	ssd2805_dcs_reg_w(atfb,0x2A, param, 4);//set column address

	param[0]=0x00;
	param[1]=0x00;
	param[2]=0x00;
	param[3]=0x60;
	ssd2805_dcs_reg_w(atfb,0x2B, param, 4);//set row address

	ssd2805_dcs_reg_w(atfb,0x29, param, 0);//set display on

#if 0
	count = ssd2805_dcs_reg_r(0x0A, read_data);
	param[0]=0x09;
	param[1]=0xB8;
	count = ssd2805_generic_reg_r(param, 2, read_data);
#endif

#else
	param[0] = 0xB8;
	param[1] = 0x00;
	param[2] = 0x05; //0x04;
	ssd2805_generic_reg_w(atfb, param, 3); //change to one lane

	ssd2805_dcs_reg_w(atfb, 0x29, param, 0); //set display on

	data = mipi_lreg_r(atfb, 0xB7);
	data |= 0xB;
	mipi_lreg_w(atfb, 0xB7, data); //enable video mode

	data = mipi_lreg_r(atfb, 0xB7);
	printk("ssd2805_init:Reg 0xB7= 0x%x\r\n", data);

#endif



}

static void ssd2805c_display_on(struct atxxfb *atfb) {
	return;
}

static void ssd2805c_display_off(struct atxxfb *atfb) {
	return;
}

static struct pannel_operation ssd2805c_ops = {
		.initialize = ssd2805c_init,
		.panel_on = ssd2805c_display_on,
		.panel_off = ssd2805c_display_off, };

int pannel_set_ops_ssd2805c(struct atxxfb *atfb) {
	atfb->pannel_ops = &ssd2805c_ops;

	atfb->spi_timing.tchw = TCHW_TIMING_SSD2805C;
	atfb->spi_timing.tcss = TCSS_TIMING_SSD2805C;
	atfb->spi_timing.tscyc = TSCYC_TMING_SSD2805C;
	atfb->spi_timing.tsdh = TSDH_TIMING_SSD2805C;
	atfb->spi_timing.tsds = TSDS_TIMING_SSD2805C;
	atfb->spi_timing.tshw = TSHW_TIMING_SSD2805C;
	atfb->spi_timing.tslw = TSLW_TIMING_SSD2805C;

	atfb->fb->var.upper_margin = LCD_VBP_SSD2805C;
	atfb->fb->var.lower_margin = LCD_VFP_SSD2805C;
	atfb->fb->var.vsync_len = LCD_VSYNC_SSD2805C;
	atfb->fb->var.left_margin = LCD_HBP_SSD2805C;
	atfb->fb->var.right_margin = LCD_HFP_SSD2805C;
	atfb->fb->var.hsync_len = LCD_HSYNC_SSD2805C;

	atfb->format = RGB16;
	atfb->xres = panel_info_ssd2805c.vl_col;
	atfb->yres = panel_info_ssd2805c.vl_row;

	return 0;
}

int pannel_set_power_ssd2805c(int on_off) {
	int err = 0;

#if 0
	//set_backlight(1, 0);

	if (on_off > 0) {
		pmu_power_control(PPS_LCD, PS_ON);
	} else {
		pmu_power_control(PPS_LCD, PS_OFF);
	}
#endif
	return err;
}

void pannel_set_refresh_rate_ssd2805c(struct clk *lcd_clk) {
	//clk_set_parent(lcd_clk, clk_get("pll1"));
	clk_set_rate(lcd_clk, 13000000);
}


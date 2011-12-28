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
#define LCD_HFP_NT35510		500
#define LCD_HBP_NT35510		500
#define LCD_HSYNC_NT35510	6
#define LCD_VFP_NT35510		4
#define LCD_VBP_NT35510		4
#define LCD_VSYNC_NT35510	9

/* spi timing info*/
#define TCHW_TIMING_NT35510	1	/* XCS signal "H" pulse width */
#define TCSS_TIMING_NT35510	60	/* XCS signal setup time */

/* serial interface mode timing */
#define	TSCYC_TMING_NT35510	125	/* Serial clock cycle time */
#define TSHW_TIMING_NT35510	11	/* SCL signal "H" pulse width */
#define TSLW_TIMING_NT35510	11	/* SCL signal "L" pulse width */
#define TSDS_TIMING_NT35510	10	/* Data setup time */
#define TSDH_TIMING_NT35510	10	/* Data hold time */

vidinfo_t panel_info_nt35510 = {
	.vl_col		= LCD_XRES_480,
	.vl_row		= LCD_XRES_800,
	.vl_bpix	= LCD_COLOR16,
};

static void spi_16bit_write_index(struct atxxfb *atfb, uint8_t high8, uint8_t low8)
{
	static uint8_t cmd_buf[] 	= {0x00, 0x00};

	cmd_buf[0] = 0x20;
	cmd_buf[1] = high8;

	atfb->spi_8bits_cmd(atfb, cmd_buf, sizeof(cmd_buf));


	cmd_buf[0] = 0x00;
	cmd_buf[1] = low8;

	atfb->spi_8bits_cmd(atfb, cmd_buf, sizeof(cmd_buf));

}

static void spi_16bit_write_par(struct atxxfb *atfb, uint8_t high8, uint8_t low8)
{
	static uint8_t cmd_data[] 	= {0x00, 0x00};

	cmd_data[0] = 0x40;
	cmd_data[1] = high8;

	atfb->spi_8bits_cmd(atfb, cmd_data, sizeof(cmd_data));
}

/*-------------------------------------------------------------------------
 *
 *		command sets for us to control lcd panel
 *
 *-----------------------------------------------------------------------*/
static void nt35510_init(struct atxxfb *atfb)
{

	//spi_16bit_write_index(atfb,0XFF,0X00);      高八位（0XFF）是寄存器的高八位，低八位（0X00）是寄存器的低八位
	//spi_16bit_write_par(atfb,0XAA,0Xff);		  高8位是有效数据，低八位0XFF无效
       printk("-------uboot:nt35510_init--------------\n");
	spi_16bit_write_index(atfb,0XFF,0X00);  //TEST Commands
	spi_16bit_write_par(atfb,0XAA,0Xff);

	spi_16bit_write_index(atfb,0XFF,0X01);
	spi_16bit_write_par(atfb,0X55,0Xff);
	spi_16bit_write_index(atfb,0XFF,0X02);
	spi_16bit_write_par(atfb,0X25,0Xff);
	spi_16bit_write_index(atfb,0XFF,0X03);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XF3,0X00);//TEST Commands
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XF3,0X01);
	spi_16bit_write_par(atfb,0X32,0Xff);
	spi_16bit_write_index(atfb,0XF3,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XF3,0X03);
	spi_16bit_write_par(atfb,0X38,0Xff);
	spi_16bit_write_index(atfb,0XF3,0X04);
	spi_16bit_write_par(atfb,0X31,0Xff);
	spi_16bit_write_index(atfb,0XF3,0X05);
	spi_16bit_write_par(atfb,0X08,0Xff);
	spi_16bit_write_index(atfb,0XF3,0X06);
	spi_16bit_write_par(atfb,0X11,0Xff);
	spi_16bit_write_index(atfb,0XF3,0X07);
	spi_16bit_write_par(atfb,0X00,0Xff);

	//set page 0

	spi_16bit_write_index(atfb,0XF0,0X00);//Select Command Page “0”
	spi_16bit_write_par(atfb,0X55,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X01);
	spi_16bit_write_par(atfb,0XAA,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X02);
	spi_16bit_write_par(atfb,0X52,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X03);
	spi_16bit_write_par(atfb,0X08,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XB0,0X00);//RGB Interface Signals Control?
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XB0,0X01);
	spi_16bit_write_par(atfb,0X05,0Xff);
	spi_16bit_write_index(atfb,0XB0,0X02);
	spi_16bit_write_par(atfb,0X02,0Xff);
	spi_16bit_write_index(atfb,0XB0,0X03);
	spi_16bit_write_par(atfb,0X05,0Xff);
	spi_16bit_write_index(atfb,0XB0,0X04);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XB1,0X00);//Display Option Control
	spi_16bit_write_par(atfb,0XCC,0Xff);
	spi_16bit_write_index(atfb,0XB1,0X01);
	spi_16bit_write_par(atfb,0X04,0Xff);

	spi_16bit_write_index(atfb,0X36,0X00);//Memory data access control
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XB3,0X00);//Display Clock in RGB Interface
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XB6,0X00);//Source Output Data Hold Time Control
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XB7,0X00);//EQ Control Function for Gate Signals
	spi_16bit_write_par(atfb,0X70,0Xff);
	spi_16bit_write_index(atfb,0XB7,0X01);//EQ Control Function for Gate Signals
	spi_16bit_write_par(atfb,0X70,0Xff);

	spi_16bit_write_index(atfb,0XB8,0X00);//EQ Control Function for Source Driver
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XB8,0X01);//
	spi_16bit_write_par(atfb,0X06,0Xff);
	spi_16bit_write_index(atfb,0XB8,0X02);//
	spi_16bit_write_par(atfb,0X06,0Xff);
	spi_16bit_write_index(atfb,0XB8,0X05);//
	spi_16bit_write_par(atfb,0X06,0Xff);

	spi_16bit_write_index(atfb,0XBC,0X00);//Inversion Driving Control
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XBC,0X01);
	spi_16bit_write_par(atfb,0Xc8,0Xff);
	spi_16bit_write_index(atfb,0XBC,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XBD,0X00);//Display Timing Control
	spi_16bit_write_par(atfb,0X01,0Xff);
	spi_16bit_write_index(atfb,0XBD,0X01);
	spi_16bit_write_par(atfb,0X84,0Xff);
	spi_16bit_write_index(atfb,0XBD,0X02);
	spi_16bit_write_par(atfb,0X06,0Xff);
	spi_16bit_write_index(atfb,0XBD,0X03);
	spi_16bit_write_par(atfb,0X50,0Xff);
	spi_16bit_write_index(atfb,0XBD,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0Xcc,0X00);//Display Timing Control (Gateless)
	spi_16bit_write_par(atfb,0X03,0Xff);
	spi_16bit_write_index(atfb,0Xcc,0X01);
	spi_16bit_write_par(atfb,0X01,0Xff);
	spi_16bit_write_index(atfb,0Xcc,0X02);
	spi_16bit_write_par(atfb,0X06,0Xff);

	//set page1

	spi_16bit_write_index(atfb,0XF0,0X00);//Select Command Page “1”
	spi_16bit_write_par(atfb,0X55,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X01);
	spi_16bit_write_par(atfb,0XAA,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X02);
	spi_16bit_write_par(atfb,0X52,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X03);
	spi_16bit_write_par(atfb,0X08,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X04);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XB0,0X00);//Setting AVDD Voltage
	spi_16bit_write_par(atfb,0X05,0Xff);
	spi_16bit_write_index(atfb,0XB0,0X01);
	spi_16bit_write_par(atfb,0X05,0Xff);
	spi_16bit_write_index(atfb,0XB0,0X02);
	spi_16bit_write_par(atfb,0X05,0Xff);

	spi_16bit_write_index(atfb,0XB1,0X00);//Setting AVee Voltage
	spi_16bit_write_par(atfb,0X05,0Xff);
	spi_16bit_write_index(atfb,0XB1,0X01);
	spi_16bit_write_par(atfb,0X05,0Xff);
	spi_16bit_write_index(atfb,0XB1,0X02);
	spi_16bit_write_par(atfb,0X05,0Xff);

	spi_16bit_write_index(atfb,0XB2,0X00);//Setting lvgl Voltage
	spi_16bit_write_par(atfb,0X03,0Xff);
	spi_16bit_write_index(atfb,0XB2,0X01);
	spi_16bit_write_par(atfb,0X03,0Xff);
	spi_16bit_write_index(atfb,0XB2,0X02);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XB8,0X00);//Setting vcl Voltage
	spi_16bit_write_par(atfb,0X25,0Xff);
	spi_16bit_write_index(atfb,0XB8,0X01);
	spi_16bit_write_par(atfb,0X25,0Xff);
	spi_16bit_write_index(atfb,0XB8,0X02);
	spi_16bit_write_par(atfb,0X25,0Xff);

	spi_16bit_write_index(atfb,0XB3,0X00);//Setting vgh Voltage
	spi_16bit_write_par(atfb,0X0b,0Xff);
	spi_16bit_write_index(atfb,0XB3,0X01);
	spi_16bit_write_par(atfb,0X0b,0Xff);
	spi_16bit_write_index(atfb,0XB3,0X02);
	spi_16bit_write_par(atfb,0X0b,0Xff);

	spi_16bit_write_index(atfb,0XB9,0X00);//Setting vgh Voltage
	spi_16bit_write_par(atfb,0X34,0Xff);
	spi_16bit_write_index(atfb,0XB9,0X01);
	spi_16bit_write_par(atfb,0X34,0Xff);
	spi_16bit_write_index(atfb,0XB9,0X02);
	spi_16bit_write_par(atfb,0X34,0Xff);

	spi_16bit_write_index(atfb,0XBF,0X00);//VGH Output Voltage
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XB5,0X00);//Setting VGL_REG Voltage
	spi_16bit_write_par(atfb,0X08,0Xff);
	spi_16bit_write_index(atfb,0XB5,0X01);
	spi_16bit_write_par(atfb,0X08,0Xff);
	spi_16bit_write_index(atfb,0XB5,0X02);
	spi_16bit_write_par(atfb,0X08,0Xff);

	spi_16bit_write_index(atfb,0XBA,0X00);//Power Control for VGLX
	spi_16bit_write_par(atfb,0X24,0Xff);
	spi_16bit_write_index(atfb,0XBA,0X01);
	spi_16bit_write_par(atfb,0X24,0Xff);
	spi_16bit_write_index(atfb,0XBA,0X02);
	spi_16bit_write_par(atfb,0X24,0Xff);

	spi_16bit_write_index(atfb,0XB4,0X00);//Setting VRGH Voltage
	spi_16bit_write_par(atfb,0X2D,0Xff);
	spi_16bit_write_index(atfb,0XB4,0X01);
	spi_16bit_write_par(atfb,0X2D,0Xff);
	spi_16bit_write_index(atfb,0XB4,0X02);
	spi_16bit_write_par(atfb,0X2D,0Xff);

	spi_16bit_write_index(atfb,0XBC,0X00);//Setting VGMP and VGSP Voltage
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XBC,0X01);
	spi_16bit_write_par(atfb,0X68,0Xff);
	spi_16bit_write_index(atfb,0XBC,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XBD,0X00);//Setting VGMN and VGSN Voltage
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XBD,0X01);
	spi_16bit_write_par(atfb,0X7C,0Xff);
	spi_16bit_write_index(atfb,0XBD,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XBE,0X00);//Setting VCOM Offset Voltage
	spi_16bit_write_par(atfb,0X00,0Xff);
	spi_16bit_write_index(atfb,0XBE,0X01);
	spi_16bit_write_par(atfb,0X40,0Xff);





	//set page1

	spi_16bit_write_index(atfb,0XF0,0X00);//Select Command Page “1”
	spi_16bit_write_par(atfb,0X55,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X01);
	spi_16bit_write_par(atfb,0XAA,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X02);
	spi_16bit_write_par(atfb,0X52,0Xff);
	spi_16bit_write_index(atfb,0XF0,0X03);
	spi_16bit_write_par(atfb,0X01,0Xff);
	//spi_16bit_write_index(atfb,0XF0,0X04);
	//spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD0,0X00);//Gradient Control for Gamma Voltage
	spi_16bit_write_par(atfb,0X0B,0Xff);
	spi_16bit_write_index(atfb,0XD0,0X01);
	spi_16bit_write_par(atfb,0X14,0Xff);
	spi_16bit_write_index(atfb,0XD0,0X02);
	spi_16bit_write_par(atfb,0X0C,0Xff);
	spi_16bit_write_index(atfb,0XD0,0X03);
	spi_16bit_write_par(atfb,0X0E,0Xff);

	//GAMMA RED
	spi_16bit_write_index(atfb,0XD1,0X00);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X01);
	spi_16bit_write_par(atfb,0X37,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X03);
	spi_16bit_write_par(atfb,0X61,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X05);
	spi_16bit_write_par(atfb,0X92,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X06);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X07);
	spi_16bit_write_par(atfb,0XB4,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X08);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X09);
	spi_16bit_write_par(atfb,0XCF,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X0a);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X0b);
	spi_16bit_write_par(atfb,0XF6,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X0c);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X0d);
	spi_16bit_write_par(atfb,0X2F,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X0e);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X0f);
	spi_16bit_write_par(atfb,0X7F,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X10);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X11);
	spi_16bit_write_par(atfb,0X97,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X12);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X13);/////
	spi_16bit_write_par(atfb,0XC0,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X14);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X15);
	spi_16bit_write_par(atfb,0XE5,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X16);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X17);
	spi_16bit_write_par(atfb,0X1F,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X18);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X19);
	spi_16bit_write_par(atfb,0X50,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X1a);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X1b);
	spi_16bit_write_par(atfb,0X52,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X1c);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X1d);
	spi_16bit_write_par(atfb,0X87,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X1e);
	spi_16bit_write_par(atfb,0X02,0Xff);


	spi_16bit_write_index(atfb,0XD1,0X1f);
	spi_16bit_write_par(atfb,0XBE,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X20);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X21);//////
	spi_16bit_write_par(atfb,0XE2,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X22);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X23);
	spi_16bit_write_par(atfb,0X0F,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X24);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X25);
	spi_16bit_write_par(atfb,0X30,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X26);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD1,0X27);
	spi_16bit_write_par(atfb,0X5C,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X28);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD1,0X29);
	spi_16bit_write_par(atfb,0X77,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X2a);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X2b);
	spi_16bit_write_par(atfb,0X94,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X2c);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X2d);
	spi_16bit_write_par(atfb,0X9F,0Xff);



	spi_16bit_write_index(atfb,0XD1,0X2e);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X2f);
	spi_16bit_write_par(atfb,0XAC,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X30);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X31);
	spi_16bit_write_par(atfb,0XBA,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X32);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD1,0X33);
	spi_16bit_write_par(atfb,0XF1,0Xff);

	//Positive Gamma for GREEN
	spi_16bit_write_index(atfb,0XD2,0X00);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X01);
	spi_16bit_write_par(atfb,0X37,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X03);
	spi_16bit_write_par(atfb,0X61,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X05);
	spi_16bit_write_par(atfb,0X92,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X06);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X07);
	spi_16bit_write_par(atfb,0XB4,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X08);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X09);
	spi_16bit_write_par(atfb,0XCF,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X0a);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X0b);
	spi_16bit_write_par(atfb,0XF6,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X0c);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X0d);
	spi_16bit_write_par(atfb,0X2F,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X0e);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X0f);
	spi_16bit_write_par(atfb,0X7F,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X10);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X11);
	spi_16bit_write_par(atfb,0X97,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X12);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X13);/////
	spi_16bit_write_par(atfb,0XC0,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X14);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X15);
	spi_16bit_write_par(atfb,0XE5,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X16);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X17);
	spi_16bit_write_par(atfb,0X1F,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X18);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X19);
	spi_16bit_write_par(atfb,0X50,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X1a);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X1b);
	spi_16bit_write_par(atfb,0X52,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X1c);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X1d);
	spi_16bit_write_par(atfb,0X87,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X1e);
	spi_16bit_write_par(atfb,0X02,0Xff);


	spi_16bit_write_index(atfb,0XD2,0X1f);
	spi_16bit_write_par(atfb,0XBE,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X20);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X21);//////
	spi_16bit_write_par(atfb,0XE2,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X22);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X23);
	spi_16bit_write_par(atfb,0X0F,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X24);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X25);
	spi_16bit_write_par(atfb,0X30,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X26);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD2,0X27);
	spi_16bit_write_par(atfb,0X5C,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X28);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD2,0X29);
	spi_16bit_write_par(atfb,0X77,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X2a);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X2b);
	spi_16bit_write_par(atfb,0X94,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X2c);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X2d);
	spi_16bit_write_par(atfb,0X9F,0Xff);



	spi_16bit_write_index(atfb,0XD2,0X2e);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X2f);
	spi_16bit_write_par(atfb,0XAC,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X30);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X31);
	spi_16bit_write_par(atfb,0XBA,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X32);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD2,0X33);
	spi_16bit_write_par(atfb,0XF1,0Xff);

	//Positive Gamma for BLUE
	spi_16bit_write_index(atfb,0XD3,0X00);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X01);
	spi_16bit_write_par(atfb,0X37,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X03);
	spi_16bit_write_par(atfb,0X61,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X05);
	spi_16bit_write_par(atfb,0X92,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X06);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X07);
	spi_16bit_write_par(atfb,0XB4,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X08);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X09);
	spi_16bit_write_par(atfb,0XCF,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X0a);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X0b);
	spi_16bit_write_par(atfb,0XF6,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X0c);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X0d);
	spi_16bit_write_par(atfb,0X2F,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X0e);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X0f);
	spi_16bit_write_par(atfb,0X7F,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X10);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X11);
	spi_16bit_write_par(atfb,0X97,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X12);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X13);/////
	spi_16bit_write_par(atfb,0XC0,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X14);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X15);
	spi_16bit_write_par(atfb,0XE5,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X16);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X17);
	spi_16bit_write_par(atfb,0X1F,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X18);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X19);
	spi_16bit_write_par(atfb,0X50,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X1a);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X1b);
	spi_16bit_write_par(atfb,0X52,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X1c);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X1d);
	spi_16bit_write_par(atfb,0X87,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X1e);
	spi_16bit_write_par(atfb,0X02,0Xff);


	spi_16bit_write_index(atfb,0XD3,0X1f);
	spi_16bit_write_par(atfb,0XBE,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X20);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X21);//////
	spi_16bit_write_par(atfb,0XE2,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X22);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X23);
	spi_16bit_write_par(atfb,0X0F,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X24);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X25);
	spi_16bit_write_par(atfb,0X30,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X26);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD3,0X27);
	spi_16bit_write_par(atfb,0X5C,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X28);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD3,0X29);
	spi_16bit_write_par(atfb,0X77,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X2a);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X2b);
	spi_16bit_write_par(atfb,0X94,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X2c);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X2d);
	spi_16bit_write_par(atfb,0X9F,0Xff);



	spi_16bit_write_index(atfb,0XD3,0X2e);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X2f);
	spi_16bit_write_par(atfb,0XAC,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X30);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X31);
	spi_16bit_write_par(atfb,0XBA,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X32);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD3,0X33);
	spi_16bit_write_par(atfb,0XF1,0Xff);

	//Negative Gamma for RED
	spi_16bit_write_index(atfb,0XD4,0X00);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X01);
	spi_16bit_write_par(atfb,0X37,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X03);
	spi_16bit_write_par(atfb,0X61,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X05);
	spi_16bit_write_par(atfb,0X92,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X06);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X07);
	spi_16bit_write_par(atfb,0XB4,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X08);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X09);
	spi_16bit_write_par(atfb,0XCF,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X0a);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X0b);
	spi_16bit_write_par(atfb,0XF6,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X0c);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X0d);
	spi_16bit_write_par(atfb,0X2F,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X0e);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X0f);
	spi_16bit_write_par(atfb,0X7F,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X10);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X11);
	spi_16bit_write_par(atfb,0X97,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X12);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X13);/////
	spi_16bit_write_par(atfb,0XC0,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X14);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X15);
	spi_16bit_write_par(atfb,0XE5,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X16);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X17);
	spi_16bit_write_par(atfb,0X1F,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X18);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X19);
	spi_16bit_write_par(atfb,0X50,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X1a);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X1b);
	spi_16bit_write_par(atfb,0X52,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X1c);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X1d);
	spi_16bit_write_par(atfb,0X87,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X1e);
	spi_16bit_write_par(atfb,0X02,0Xff);


	spi_16bit_write_index(atfb,0XD4,0X1f);
	spi_16bit_write_par(atfb,0XBE,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X20);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X21);//////
	spi_16bit_write_par(atfb,0XE2,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X22);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X23);
	spi_16bit_write_par(atfb,0X0F,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X24);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X25);
	spi_16bit_write_par(atfb,0X30,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X26);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD4,0X27);
	spi_16bit_write_par(atfb,0X5C,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X28);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD4,0X29);
	spi_16bit_write_par(atfb,0X77,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X2a);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X2b);
	spi_16bit_write_par(atfb,0X94,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X2c);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X2d);
	spi_16bit_write_par(atfb,0X9F,0Xff);



	spi_16bit_write_index(atfb,0XD4,0X2e);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X2f);
	spi_16bit_write_par(atfb,0XAC,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X30);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X31);
	spi_16bit_write_par(atfb,0XBA,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X32);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD4,0X33);
	spi_16bit_write_par(atfb,0XF1,0Xff);

	//Negative Gamma for GREEN
	spi_16bit_write_index(atfb,0XD5,0X00);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X01);
	spi_16bit_write_par(atfb,0X37,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X03);
	spi_16bit_write_par(atfb,0X61,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X05);
	spi_16bit_write_par(atfb,0X92,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X06);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X07);
	spi_16bit_write_par(atfb,0XB4,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X08);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X09);
	spi_16bit_write_par(atfb,0XCF,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X0a);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X0b);
	spi_16bit_write_par(atfb,0XF6,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X0c);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X0d);
	spi_16bit_write_par(atfb,0X2F,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X0e);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X0f);
	spi_16bit_write_par(atfb,0X7F,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X10);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X11);
	spi_16bit_write_par(atfb,0X97,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X12);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X13);/////
	spi_16bit_write_par(atfb,0XC0,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X14);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X15);
	spi_16bit_write_par(atfb,0XE5,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X16);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X17);
	spi_16bit_write_par(atfb,0X1F,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X18);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X19);
	spi_16bit_write_par(atfb,0X50,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X1a);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X1b);
	spi_16bit_write_par(atfb,0X52,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X1c);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X1d);
	spi_16bit_write_par(atfb,0X87,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X1e);
	spi_16bit_write_par(atfb,0X02,0Xff);


	spi_16bit_write_index(atfb,0XD5,0X1f);
	spi_16bit_write_par(atfb,0XBE,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X20);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X21);//////
	spi_16bit_write_par(atfb,0XE2,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X22);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X23);
	spi_16bit_write_par(atfb,0X0F,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X24);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X25);
	spi_16bit_write_par(atfb,0X30,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X26);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD5,0X27);
	spi_16bit_write_par(atfb,0X5C,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X28);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD5,0X29);
	spi_16bit_write_par(atfb,0X77,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X2a);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X2b);
	spi_16bit_write_par(atfb,0X94,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X2c);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X2d);
	spi_16bit_write_par(atfb,0X9F,0Xff);



	spi_16bit_write_index(atfb,0XD5,0X2e);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X2f);
	spi_16bit_write_par(atfb,0XAC,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X30);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X31);
	spi_16bit_write_par(atfb,0XBA,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X32);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD5,0X33);
	spi_16bit_write_par(atfb,0XF1,0Xff);
	//Negative Gamma for BLUE
	spi_16bit_write_index(atfb,0XD6,0X00);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X01);
	spi_16bit_write_par(atfb,0X37,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X02);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X03);
	spi_16bit_write_par(atfb,0X61,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X04);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X05);
	spi_16bit_write_par(atfb,0X92,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X06);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X07);
	spi_16bit_write_par(atfb,0XB4,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X08);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X09);
	spi_16bit_write_par(atfb,0XCF,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X0a);
	spi_16bit_write_par(atfb,0X00,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X0b);
	spi_16bit_write_par(atfb,0XF6,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X0c);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X0d);
	spi_16bit_write_par(atfb,0X2F,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X0e);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X0f);
	spi_16bit_write_par(atfb,0X7F,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X10);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X11);
	spi_16bit_write_par(atfb,0X97,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X12);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X13);/////
	spi_16bit_write_par(atfb,0XC0,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X14);
	spi_16bit_write_par(atfb,0X01,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X15);
	spi_16bit_write_par(atfb,0XE5,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X16);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X17);
	spi_16bit_write_par(atfb,0X1F,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X18);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X19);
	spi_16bit_write_par(atfb,0X50,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X1a);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X1b);
	spi_16bit_write_par(atfb,0X52,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X1c);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X1d);
	spi_16bit_write_par(atfb,0X87,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X1e);
	spi_16bit_write_par(atfb,0X02,0Xff);


	spi_16bit_write_index(atfb,0XD6,0X1f);
	spi_16bit_write_par(atfb,0XBE,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X20);
	spi_16bit_write_par(atfb,0X02,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X21);//////
	spi_16bit_write_par(atfb,0XE2,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X22);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X23);
	spi_16bit_write_par(atfb,0X0F,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X24);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X25);
	spi_16bit_write_par(atfb,0X30,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X26);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD6,0X27);
	spi_16bit_write_par(atfb,0X5C,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X28);
	spi_16bit_write_par(atfb,0X03,0Xff);


	spi_16bit_write_index(atfb,0XD6,0X29);
	spi_16bit_write_par(atfb,0X77,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X2a);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X2b);
	spi_16bit_write_par(atfb,0X94,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X2c);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X2d);
	spi_16bit_write_par(atfb,0X9F,0Xff);



	spi_16bit_write_index(atfb,0XD6,0X2e);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X2f);
	spi_16bit_write_par(atfb,0XAC,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X30);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X31);
	spi_16bit_write_par(atfb,0XBA,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X32);
	spi_16bit_write_par(atfb,0X03,0Xff);

	spi_16bit_write_index(atfb,0XD6,0X33);
	spi_16bit_write_par(atfb,0XF1,0Xff);


	spi_16bit_write_index(atfb,0X11,0X00); //
	mdelay(120);

	spi_16bit_write_index(atfb,0X3A,0X00);
	spi_16bit_write_par(atfb,0X77,0Xff);//77 is 24 bit, 66 is 18 bit

	spi_16bit_write_index(atfb,0X29,0X00); //
	mdelay(100);

	//spi_16bit_write_index(atfb,0X36,0X00);//Memory data access control
	//spi_16bit_write_par(atfb,0X02,0Xff);	//0xc2
	//mdelay(10);
	//  spi_16bit_write_index(atfb,0X2C,0X00); //
	//  Delay(20);


}

static void nt35510_display_on(struct atxxfb *atfb)
{
	return;
}

static void nt35510_display_off(struct atxxfb *atfb)
{
	return;
}

static struct pannel_operation nt35510_ops = {
	.initialize = nt35510_init,
	.panel_on = nt35510_display_on,
	.panel_off = nt35510_display_off,
};

int pannel_set_ops_nt35510(struct atxxfb *atfb)
{
	atfb->pannel_ops = &nt35510_ops;

	atfb->spi_timing.tchw = TCHW_TIMING_NT35510;
	atfb->spi_timing.tcss = TCSS_TIMING_NT35510;
	atfb->spi_timing.tscyc = TSCYC_TMING_NT35510;
	atfb->spi_timing.tsdh = TSDH_TIMING_NT35510;
	atfb->spi_timing.tsds = TSDS_TIMING_NT35510;
	atfb->spi_timing.tshw = TSHW_TIMING_NT35510;
	atfb->spi_timing.tslw = TSLW_TIMING_NT35510;

	atfb->fb->var.upper_margin = LCD_VBP_NT35510;
	atfb->fb->var.lower_margin = LCD_VFP_NT35510;
	atfb->fb->var.vsync_len = LCD_VSYNC_NT35510;
	atfb->fb->var.left_margin = LCD_HBP_NT35510;
	atfb->fb->var.right_margin = LCD_HFP_NT35510;
	atfb->fb->var.hsync_len = LCD_HSYNC_NT35510;

	atfb->format = RGB16;//RGB16;
	//atfb->resolution = ATXX_WVGA;
	atfb->xres = panel_info_nt35510.vl_col;
	atfb->yres = panel_info_nt35510.vl_row;
	
	return 0;
}

int pannel_set_power_nt35510(int on_off)
{
	int err;

	set_backlight(1, 0);

	if(on_off > 0) {
		pmu_power_control(PPS_LCD, PS_ON);
	} else {
		pmu_power_control(PPS_LCD, PS_OFF);
	}

	return err;
}

void pannel_set_refresh_rate_nt35510(struct clk *lcd_clk)
{
	clk_set_parent(lcd_clk, clk_get("pll1"));
	clk_set_rate(lcd_clk, 30000000);
}


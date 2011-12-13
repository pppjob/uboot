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
/******************
add Lcd_config
******************/
#ifndef	LCD_TABLE_H
#define	LCD_TABLE_H

#include 	<asm/arch-atxx/clock.h>
#include 	<linux/types.h>
#include 	<linux/stddef.h>
#include	<linux/string.h>
#include    "atxxfb.h"
#include	<asm/arch-atxx/factorydata.h>
#include	<lcd.h>
#include 	<common.h>


#if		defined(CONFIG_BOARD_ATB1005)
#define	DEFAULTLCDNAME			"lms350df04"
#elif   defined(CONFIG_BOARD_G01)
#define	DEFAULTLCDNAME			"rm68041"
#elif   defined(CONFIG_BOARD_GAMEBOX)
#define	DEFAULTLCDNAME			"inno7dd"
#elif   defined(CONFIG_BOARD_M70P)
#define	DEFAULTLCDNAME			"inno7dd"
#elif	defined(CONFIG_BOARD_A500)
#define DEFAULTLCDNAME			"ili9486"
#endif


//#define	DEFAULTLCDNAME			"inno7dd"
enum LCDTYPE_ID
{
	LCD_rm68041=0,
	LCD_lms350df04,
	LCD_rm68040,
	LCD_hsd0701dw1,
	LCD_inno7dd,
	LCD_inno8d2,
	LCD_r61581,
	LCD_sharpis035,
	LCD_tm320,
	LCD_ili9486,
	LCD_max,
};

struct lcdattribe
{
	uint8_t		id;			//define id
	uint32_t	readid;		//lcd factory id
	const  char	*name;
	int (*pannel_set_ops)(struct atxxfb *atfb);
	int (*pannel_set_power)(int on_off);
	void (*pannel_set_refresh_rate)(struct clk *lcd_clk);
};



extern int pannel_set_ops_rm68041(struct atxxfb *atfb);
extern int pannel_set_power_rm68041(int on_off);
extern void pannel_set_refresh_rate_rm68041(struct clk *lcd_clk);

extern int pannel_set_ops_lms350df04(struct atxxfb *atfb);
extern int pannel_set_power_lms350df04(int on_off);
extern void pannel_set_refresh_rate_lms350df04(struct clk *lcd_clk);

extern int pannel_set_ops_rm68040(struct atxxfb *atfb);
extern int pannel_set_power_rm68040(int on_off);
extern void pannel_set_refresh_rate_rm68040(struct clk *lcd_clk);

extern int pannel_set_ops_hsd0701(struct atxxfb *atfb);
extern int pannel_set_power_hsd0701(int on_off);
extern void pannel_set_refresh_rate_hsd0701(struct clk *lcd_clk);

extern int pannel_set_ops_inno7dd(struct atxxfb *atfb);
extern int pannel_set_power_inno7dd(int on_off);
extern void pannel_set_refresh_rate_inno7dd(struct clk *lcd_clk);

extern int pannel_set_ops_inno8d2(struct atxxfb *atfb);
extern int pannel_set_power_inno8d2(int on_off);
extern void pannel_set_refresh_rate_inno8d2(struct clk *lcd_clk);

extern int pannel_set_ops_ili9486(struct atxxfb *atfb);
extern int pannel_set_power_ili9486(int on_off);
extern void pannel_set_refresh_rate_ili9486(struct clk *lcd_clk);

extern vidinfo_t panel_info_rm68041;
extern vidinfo_t panel_info_lms350df04;
extern vidinfo_t panel_info_rm68040;
extern vidinfo_t panel_info_hsd0701;
extern vidinfo_t panel_info_inno7dd;
extern vidinfo_t panel_info_inno8d2;
extern vidinfo_t panel_info_r61581;
extern vidinfo_t panel_info_sharp_ls035;
extern vidinfo_t panel_info_tm320;
extern vidinfo_t panel_info_ili9486;


extern int pannel_set_ops_r61581(struct atxxfb *atfb);
extern int pannel_set_power_r61581(int on_off);
extern void pannel_set_refresh_rate_r61581(struct clk *lcd_clk);

extern int pannel_set_ops_sharp_ls035(struct atxxfb *atfb);
extern int pannel_set_power_sharp_ls035(int on_off);
extern void pannel_set_refresh_rate_sharp_ls035(struct clk *lcd_clk);

extern int pannel_set_ops_tm320(struct atxxfb *atfb);
extern int pannel_set_power_tm320(int on_off);
extern void pannel_set_refresh_rate_tm320(struct clk *lcd_clk);
extern void InitLcdops(void);

#endif



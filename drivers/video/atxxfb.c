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
#include <asm-arm/arch-atxx/topctl.h>

#include "atxxfb.h"

/* display engine */
#define ATXX_LCD_VBASE			0X3FEF0000
#define ATXX_LCD_SIZE			0X100

static uint32_t lcd_xres = 0;
static uint32_t lcd_yres = 0;

struct atxxfb atfb;
struct fb_info fbinfo;

static int bytes_per_pixel[] = {
	[RGB8]	= 1,
	[RGB16]	= 2,
	[RGB24]	= 3,
	[RGB32]	= 4,
	[YUY2]	= 0,	/* pending */
	[UYVY]	= 0,	/* pending */
	[YV16]	= 0,	/* pending */
};

void *lcd_base;			/* Start of framebuffer memory	*/
void *lcd_console_address;	/* Start of console buffer	*/

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

short console_col;
short console_row;

/*-----------------------------------------------------------------------
 *
 *	fb control register read/write/set/clear operations
 *
 *----------------------------------------------------------------------*/
static __inline uint32_t atxxfb_ctl_read(uint32_t index)
{
	return __raw_readl(ATXX_LCD_VBASE + index * 4);
}

static __inline void atxxfb_ctl_write(uint32_t index, const uint32_t value)
{
	__raw_writel(value, ATXX_LCD_VBASE + index * 4);
}

static __inline void atxxfb_ctl_set_bit(uint32_t index, uint32_t shift)
{
	atxxfb_ctl_write(index, (1 << shift) | (atxxfb_ctl_read(index)));
}

static __inline void atxxfb_ctl_clear_bit(uint32_t index, uint32_t shift)
{
	atxxfb_ctl_write(index, ((~(1 << shift))) & (atxxfb_ctl_read(index)));
}

/*-----------------------------------------------------------------------
 *
 *The following function is for ATXX LCD controler to control LCD pannel
 *
 *----------------------------------------------------------------------*/
static void send_bit(struct atxxfb *atfb, uint8_t bit)
{
	uint32_t reg_ctr24;

	/* set SCL low and delay WRITE_TSLW_TIMING */
	atxxfb_ctl_clear_bit(24, CTL24_BIT_SCL);
	ndelay(atfb->spi_timing.tslw);

	/* send bit to DOUT and delay WRITE_TSDS_TIMING ns */
	reg_ctr24 = atxxfb_ctl_read(24);
	reg_ctr24 &= CTL24_SDOUT_MASK;
	reg_ctr24 |= (bit << CTL24_BIT_SDOUT);
	atxxfb_ctl_write(24, reg_ctr24);
	ndelay(atfb->spi_timing.tsds);

	/* set SCL high and delay WRITE_TSHW_TIMING */
	atxxfb_ctl_set_bit(24, CTL24_BIT_SCL);
	ndelay(atfb->spi_timing.tshw);
}

/* send command/paramater to LCD panel */
static void write_command_parameter(struct atxxfb *atfb, uint8_t command)
{
	uint8_t val, temp;
	int i;

	temp = command;
	/*
	 * data is send from MSB to LSB
	 */
	for (i = 0; i < 8; i++) {
		val = (temp & 0x80) >> 7;
		send_bit(atfb, val);
		temp = (temp << 1);
	}
}

static void spi_8bits_cmd(struct atxxfb *atfb, uint8_t * buffer,
				uint32_t len)
{
	uint8_t *pbuffer;
	uint32_t tmp_data;
	int i;

	pbuffer = buffer;

	/* set XCS to low */
	atxxfb_ctl_clear_bit(24, CTL24_BIT_XCS);
	ndelay(atfb->spi_timing.tcss);

	for (i = 0; i < len; i++) {
		tmp_data = (*pbuffer++) & 0xFF;

		/* send command/parameter flag first */
		write_command_parameter(atfb, tmp_data);
	}

	/* set XCS to high */
	atxxfb_ctl_set_bit(24, CTL24_BIT_XCS);
	ndelay(atfb->spi_timing.tchw);
}

/* send command/paramater to LCD panel */
static void spi_9bits_cmd(struct atxxfb *atfb, uint8_t command)
{
	uint8_t val, temp;
	int i;

	temp = command;
	send_bit(atfb, 0);
	/*
	 * data is send from MSB to LSB
	 */
	for (i = 0; i < 8; i++) {
		val = (temp & 0x80) >> 7;
		send_bit(atfb, val);
		temp = (temp << 1);
	}
}

/* send command/paramater to LCD panel */
static void spi_9bits_par(struct atxxfb *atfb, uint8_t command)
{
	uint8_t val, temp;
	int i;

	temp = command;
	send_bit(atfb, 1);
	/*
	 * data is send from MSB to LSB
	 */
	for (i = 0; i < 8; i++) {
		val = (temp & 0x80) >> 7;
		send_bit(atfb, val);
		temp = (temp << 1);
	}
}

static void set_csx(struct atxxfb *atfb, uint32_t level)
{
	if (0 == level) {
		/* set XCS to low */
		atxxfb_ctl_clear_bit(24, CTL24_BIT_XCS);
		ndelay(atfb->spi_timing.tcss);
	} else {
		/* set XCS to high */
		atxxfb_ctl_set_bit(24, CTL24_BIT_XCS);
		ndelay(atfb->spi_timing.tchw);
	}
}

/*-----------------------------------------------------------------------
 *
 *This following function is for ATXX LCD controler settings
 *
 *----------------------------------------------------------------------*/

static void mlcd_general_setting(void)
{
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_ON);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_SLCD_ON);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_RST_FRAMENUM);

	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_HSYNC_POL);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_VSYNC_POL);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_RHSCAN);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_RVSCAN);
	atxxfb_ctl_set_bit(0, CTL0_BIT_MLCD_MODE);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_ACTIVE_LCD);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_INTSEL);
	atxxfb_ctl_set_bit(0, CTL0_BIT_MLCD_INTEN);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_HBLANK);
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_VBLANK);
}

static void lcd_pix_format(enum pix_format mode)
{
	uint32_t reg_ctr0;

	/* clear bit[18:16] */
	reg_ctr0 = atxxfb_ctl_read(0);
	reg_ctr0 &= CTL0_PIX_FORMAT_MASK;

	switch (mode) {
	case RGB8:
		reg_ctr0 |= (0x0 << CTL0_BIT_PIX_FORMAT);
		break;
	case RGB16:
		reg_ctr0 |= (0x1 << CTL0_BIT_PIX_FORMAT);
		break;
	case RGB24:
		reg_ctr0 |= (0x2 << CTL0_BIT_PIX_FORMAT);
		break;
	case RGB32:
		reg_ctr0 |= (0x3 << CTL0_BIT_PIX_FORMAT);
		break;
	case YUY2:
		reg_ctr0 |= (0x4 << CTL0_BIT_PIX_FORMAT);
		break;
	case UYVY:
		reg_ctr0 |= (0x5 << CTL0_BIT_PIX_FORMAT);
		break;
	case YV16:
		reg_ctr0 |= (0x6 << CTL0_BIT_PIX_FORMAT);
		break;
	default:
		atxxfb_dbg("This branch won't be reached\n");
	}

	atxxfb_ctl_write(0, reg_ctr0);
}

static void mlcd_set_pitch(uint16_t y_pitch, uint16_t cbr_pitch)
{
	uint32_t reg_ctr4;

	reg_ctr4 = atxxfb_ctl_read(4);

	/*
	 * set source pitch for Y (or RGB) channel
	 * QWORD aligned if linear mode
	 * 4x(64B/128B/256B) aligned if band mode,
	 */
	reg_ctr4 = (reg_ctr4 & CTL4_SRC_PITCH_Y_M_MASK) |
		(y_pitch << CTL4_BIT_SRC_PITCH_Y_M);

	/*
	 * set source pitch for Cb/Cr (or RGB) channel
	 * QWORD aligned if linear mode
	 * 4x(64B/128B/256B) aligned if band mode
	 * only for YV16
	 */
	reg_ctr4 = (reg_ctr4 & CTL4_SRC_PITCH_CBR_M_MASK) |
		(cbr_pitch << CTL4_BIT_SRC_PITCH_CBR_M);

	atxxfb_ctl_write(4, reg_ctr4);
}

static void mlcd_set_margin(struct fb_var_screeninfo *fb_var_sc)
{
	uint32_t reg_ctr9 = 0, reg_ctr10 = 0;
	uint32_t reg_ctr11 = 0, reg_ctr12 = 0;
	uint32_t VP, HP;

	VP = fb_var_sc->upper_margin +
		fb_var_sc->lower_margin +
		fb_var_sc->vsync_len + fb_var_sc->yres;
	HP = fb_var_sc->left_margin +
		fb_var_sc->right_margin +
		fb_var_sc->hsync_len + fb_var_sc->xres;

	reg_ctr9 |= ((VP - 1) << CTL9_BIT_VP_M) |
		((fb_var_sc->vsync_len - 1) << CTL9_BIT_VS_M) |
		((fb_var_sc->lower_margin - 1)
		<< CTL9_BIT_VFP_M);
	atxxfb_ctl_write(9, reg_ctr9);

	reg_ctr10 |= ((fb_var_sc->upper_margin - 1)
		<< CTL10_BIT_VBP_M) | ((fb_var_sc->yres - 1)
		<< CTL10_BIT_VDISP_M);
	atxxfb_ctl_write(10, reg_ctr10);

	reg_ctr11 |= ((HP - 1) << CTL11_BIT_HP_M) |
		((fb_var_sc->hsync_len - 1)
		<< CTL11_BIT_HS_M) | ((fb_var_sc->right_margin - 1)
		<< CTL11_BIT_HFP_M);
	atxxfb_ctl_write(11, reg_ctr11);

	reg_ctr12 |= ((fb_var_sc->left_margin - 1)
		<< CTL12_BIT_HBP_M) | ((fb_var_sc->xres - 1)
		<< CTL12_BIT_HDISP_M);

	atxxfb_ctl_write(12, reg_ctr12);
}

static void lcd_dither_en(uint8_t on_off)
{
	uint32_t reg_ctr20;
	reg_ctr20 = atxxfb_ctl_read(20);
	reg_ctr20 = (reg_ctr20 & (~(1 << CTL20_BIT_DITHER_EN))) |
		((on_off > 0) << CTL20_BIT_DITHER_EN);
	atxxfb_ctl_write(20, reg_ctr20);
}

static void lcd_gamma_setting(void)
{
	uint32_t reg_ctr21;
	reg_ctr21 = atxxfb_ctl_read(21);
	/*
	 * set gamma_ren to 0
	 * set gamma_gen to 0
	 * set gamma_ben to 0
	 * set gamma_luten to 0
	 */
	reg_ctr21 &= CTL21_GAMMA_REN_MASK & CTL21_GAMMA_GEN_MASK &
		CTL21_GAMMA_BEN_MASK & CTL21_GAMMA_LUTEN_MASK;
	atxxfb_ctl_write(21, reg_ctr21);
}

static void lcd_miscellaneous_setting(void)
{
	uint32_t reg_ctr23;
	reg_ctr23 = atxxfb_ctl_read(23);
	/*
	 * set threshold of line buffer to 8
	 * set memory page size to 00 (1KB)
	 * set band mode to 0
	 * set bank interleave to 0
	 */
	reg_ctr23 = (reg_ctr23 & CTL23_LBUF_THR_MASK) | 8;
	reg_ctr23 = reg_ctr23 & CTL23_PAGE_SIZE_MASK &
		CTL23_BAND_MODE_MASK & CTL23_BANK_INTERLEAVE_MASK;
	atxxfb_ctl_write(23, reg_ctr23);
}

/* pls refer to power on programming sequence in our DATABOOK */
static int mlcd_pwron(void)
{
	uint32_t status;
	uint32_t time_out = 0;

	/* set CTL0_BIT_MLCD_PWRON bit in LCDCTL0 register */
	atxxfb_ctl_set_bit(0, CTL0_BIT_MLCD_PWRON);

	status = atxxfb_ctl_read(0);

	/* check if power on or time out */
	while ((!(status & (1 << CTL0_REG_MLCD_PWR_STATUS))) &&
			(time_out < MLCD_POWER_ON_OFF_TIMEOUT)) {
		udelay(20);
		status = atxxfb_ctl_read(0);
		time_out++;
	}

	if (time_out >= MLCD_POWER_ON_OFF_TIMEOUT) {
		atxxfb_err("power on mlcd timeout\n");
		return -EAGAIN;
	}

	return 0;
}

/* pls refer to power off programming sequence in our DATABOOK */
static int mlcd_pwroff(void)
{

	uint32_t status;
	uint32_t time_out = 0;
	atxxfb_dbg("%s called\n", __func__);

	/* set CTL0_BIT_MLCD_PWRON bit in LCDCTL0 register */
	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_PWRON);

	status = atxxfb_ctl_read(0);

	/* check if power on or time out */
	while ((status & (1 << CTL0_REG_MLCD_PWR_STATUS)) &&
			(time_out < MLCD_POWER_ON_OFF_TIMEOUT)) {
		mdelay(20);
		status = atxxfb_ctl_read(0);
		time_out++;
	}

	if (time_out >= MLCD_POWER_ON_OFF_TIMEOUT) {
		atxxfb_err("power off mlcd timeout\n");
		return -EAGAIN;
	}

	atxxfb_ctl_clear_bit(0, CTL0_BIT_MLCD_ON);

	return 0;
}

static int mlcd_power_on_off(enum mlcd_power_on_off_control on_off)
{
	int ret = 0;

	switch (on_off) {
	case MLCD_POWER_OFF:
		ret = mlcd_pwroff();
		break;
	case MLCD_POWER_ON:
		ret = mlcd_pwron();
		break;
	default:
		atxxfb_err("This branch will never be reached\n");
		break;
	}
	return ret;
}

static void mlcd_on_off(enum mlcd_on_off_control on_off)
{
	uint32_t reg_ctr0;

	reg_ctr0 = atxxfb_ctl_read(0);
	reg_ctr0 = (reg_ctr0 & CTL0_MLCD_ON_MASK) | on_off;
	atxxfb_ctl_write(0, reg_ctr0);
}

static void mlcd_pixdat_start_addr(uint32_t y_start, uint32_t cb_start,
					uint32_t cr_start)
{
	atxxfb_ctl_write(1, y_start);
	atxxfb_ctl_write(2, cb_start);
	atxxfb_ctl_write(3, cr_start);
}

static void lcd_ctl_init(struct fb_info *fbinfo,
				uint32_t lcd_xres, int mode)
{
	struct atxxfb *atfb = fbinfo->par;
	mlcd_general_setting();
	lcd_pix_format(mode);

	mlcd_set_pitch(lcd_xres / 8 * (mode + 1), 0);
	mlcd_set_margin(&fbinfo->var);
	lcd_dither_en(1);

	lcd_gamma_setting();
	lcd_miscellaneous_setting();
	mlcd_pixdat_start_addr(atfb->smem_addr, 0, 0);
}

/*
 * fb_set_par - Optional function. Alters the hardware state.
 *
 * @fbinfo: frame buffer structure that represents a single frame buffer
 *
 *      Using the fb_var_screeninfo in fb_info we set the resolution of the
 *      this particular framebuffer. This function alters the par AND the
 *      fb_fix_screeninfo stored in fb_info. It doesn't not alter var in
 *      fb_info since we are using that data.
 *
 *      This function is also used to recover/restore the hardware to a
 *      known working state.
 *
 *      fb_check_var is always called before fb_set_par to ensure
 *      that the contents of var is always valid.
 *
 *      if you can't change the resolution you don't need this function.
 */

static int atxxfb_set_par(struct fb_info *fbinfo)
{
	struct fb_var_screeninfo *var = &fbinfo->var;
	int mode, bpp;
	struct atxxfb *atfb = fbinfo->par;

	/* >>3 means to be divided by 8 */
	bpp = var->bits_per_pixel >> 3;

	fbinfo->line_length = var->xres * bpp;

	/* activate this new configuration */
	switch (var->bits_per_pixel) {
	case 8:
	case 16:
	case 24:
	case 32:
		mode = bpp - 1;
		break;
	default:
		atxxfb_err("fb_set_par: invalid mode"
				"bits_per_pixel = %d\n", var->bits_per_pixel);
		return -EINVAL;
	}

	if ((var->xres == lcd_xres) && (var->yres == lcd_yres)) {
		mlcd_on_off(MLCD_OFF);
#ifdef CONFIG_BOARD_T3C
		mdelay(10);/*at least 10ms,  tPOT */
		lcd_ctl_init(fbinfo, var->xres, mode);
		mlcd_on_off(MLCD_ON);
		mdelay(10);
		/* configure VGA LCD pannel */
		atfb->pannel_ops->initialize(atfb);
#else
		/* configure VGA LCD pannel */
		atfb->pannel_ops->initialize(atfb);
		mdelay(10);/*at least 10ms,  tPOT */
		/* configure LCD controller */
		lcd_ctl_init(fbinfo, var->xres, mode);
		mlcd_on_off(MLCD_ON);
#endif
	} else {
		/*
		 * since we have check resolution in function
		 * fb_check_var. this else should never
		 * be reached
		 */
		atxxfb_err("Unknow mode %dx%d\n",
				var->xres, var->yres);
		return -EINVAL;
	}

	atfb->xres = var->xres;
	atfb->yres = var->yres;

	return 0;
}

/*
 * fb_check_par - Optional function. Validates a var passed in.
 *
 * @var: frame buffer variable screen structure
 * @fbinfo: frame buffer structure that represents a single frame buffer
 *
 *	Checks to see if the hardware supports the state requested by
 *	var passed in. This function does not alter the hardware state!!!
 *	This means the data stored in struct fb_info.
 *
 *	Exception to the above rule:  Some drivers have a fixed mode, ie,
 *	the hardware is already set at boot up, and cannot be changed.  In
 *	this case, it is more acceptable that this function just return
 *	a copy of the currently working var (fbinfo->var). Better is to not
 *	implement this function, as the upper layer will do the copying
 *	of the current var for you.
 *
 *	Note:  This is the only function where the contents of var can be
 *	freely adjusted after the driver has been registered. If you find
 *	that you have code outside of this function that alters the content
 *	of var, then you are doing something wrong.  Note also that the
 *	contents of fbinfo->var must be left untouched at all times after
 *	driver registration.
 *
 *	Returns negative errno on error, or zero on success.
 */

static int atxxfb_check_var(struct fb_var_screeninfo *var,
			struct fb_info *fbinfo)
{
	atxxfb_dbg("fb_check_par: resolution:  %dx%d,   "
			"var->bits_per_pixel = %d\n",
			var->xres, var->yres, var->bits_per_pixel);

	/* to see if resolution is supported */
	if ((var->xres != lcd_xres) || (var->yres != lcd_yres)) {
		atxxfb_err("wrong resolution,"
				"only (%dx%d) supported\n", lcd_xres, lcd_yres);
		return -EINVAL;
	}

	switch (var->bits_per_pixel) {
	case 8:
		var->red.offset = 0;
		var->red.length = 8;
		var->green.offset = 0;
		var->green.length = 8;
		var->blue.offset = 0;
		var->blue.length = 8;
		var->transp.offset = 0;
		var->transp.length = 0;
		break;
	case 16:		/* RGBA 5551 */
		if (var->transp.length) {
			var->red.offset = 0;
			var->red.length = 5;
			var->green.offset = 5;
			var->green.length = 5;
			var->blue.offset = 10;
			var->blue.length = 5;
			var->transp.offset = 15;
			var->transp.length = 1;
		} else {	/* RGB 565 */
			var->red.offset = 11;
			var->red.length = 5;
			var->green.offset = 5;
			var->green.length = 6;
			var->blue.offset = 0;
			var->blue.length = 5;
			var->transp.offset = 0;
			var->transp.length = 0;
		}
		break;
	case 24:		/* RGB 888 */
		var->red.offset = 16;
		var->red.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->blue.offset = 0;
		var->blue.length = 8;
		var->transp.offset = 0;
		var->transp.length = 0;
		break;
	case 32:		/* RGBA 8888 */
		var->red.offset = 16;
		var->red.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->blue.offset = 0;
		var->blue.length = 8;
		var->transp.offset = 24;
		var->transp.length = 8;
		break;
	default:
		atxxfb_err("wrong pixel format,"
				"only (RGB8/RGB16/RGB24/RGB32) supported\n");
		return -EINVAL;
	}
	var->red.msb_right = 0;
	var->green.msb_right = 0;
	var->blue.msb_right = 0;
	var->transp.msb_right = 0;

	return 0;
}

/*-----------------------------------------------------------------------
 * lcd init clear and painting
 *----------------------------------------------------------------------*/

void lcd_clear_screen(unsigned short **lcd_buf, unsigned short c, int w, int h)
{
	unsigned int x,y ;

    for( y = 0 ; y < w ; y++ ) {
    	for( x = 0 ; x < h ; x++ ) {
    		lcd_buf[y][x] = c;
    	}
    }
}

void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
{
	return;
}

void lcd_ctrl_init(void *lcdbase)
{
	int bpp;
	uint32_t psize;

	pannel_set_power(1);

	fbinfo.par = &atfb;
	atfb.fb = &fbinfo;

	/* get clk */
	atfb.lcd_clk = clk_get("pclk");
	if (IS_ERR(atfb.lcd_clk)) {
		atxxfb_err("get clk failed.\n");
		return;
	}

	pannel_set_refresh_rate(atfb.lcd_clk);

	/* power on mlcd */
	mlcd_power_on_off(MLCD_POWER_ON);

	/* set LCD pannel API */
	pannel_set_ops(&atfb);
	atfb.spi_8bits_cmd = spi_8bits_cmd;
	atfb.spi_9bits_cmd = spi_9bits_cmd;
	atfb.spi_9bits_par = spi_9bits_par;
	atfb.set_csx = set_csx;

	bpp = bytes_per_pixel[atfb.format];

	lcd_xres = atfb.xres;
	lcd_yres = atfb.yres;


	psize = lcd_yres * lcd_xres * bpp;

	if (lcdbase == 0) {
		atxxfb_err("No memory left for framebuffer!\n");
	}

	atfb.fb->var.bits_per_pixel = bpp * 8;
	atfb.fb->var.xres = lcd_xres;
	atfb.fb->var.yres = lcd_yres;

	atfb.fb->screen_base = lcdbase;
	atfb.fb->screen_size = psize;
	atfb.smem_addr = (uint32_t)lcdbase;

	/* put hardware to a known working state */
	atxxfb_check_var(&atfb.fb->var, atfb.fb);
	atxxfb_set_par(atfb.fb);

	return;
}

ulong calc_fbsize(void)
{
	return fbinfo.screen_size;
}

extern const unsigned short logo_data[];
extern const int logo_width;
extern const int logo_height;

static void lcd_paint_logo()
{
	int x, y;
	int x0, y0;
	unsigned short count = 0, color;
	unsigned short *lcd_buf = atfb.smem_addr;
	unsigned short *data = logo_data;

	x0 = (lcd_xres - logo_width) / 2;
	y0 = (lcd_yres - logo_height) / 2;

    for( y = y0 ; y < y0 + logo_height ; y++ ) {
    	for( x = x0 ; x < x0 + logo_width ; x++ ) {
    		if (count <= 0) {
    			count = *data++;
    			color = *data++;
    		}
            if (x > 0 && x < lcd_xres && y > 0 && y < lcd_yres)
            	lcd_buf[lcd_xres * y + x] = color;
            count--;
    	}
    }
}

void lcd_show_logo(int logo_width, int logo_height, unsigned short *logo_data)
{
	int x, y;
	int x0, y0;
	unsigned short count = 0, color;
	unsigned short *lcd_buf = atfb.smem_addr;
	unsigned short *data = logo_data;

	if(logo_data == NULL)
		return ;

	x0 = (lcd_xres - logo_width) / 2;
	y0 = (lcd_yres - logo_height) / 2;

    for( y = y0 ; y < y0 + logo_height ; y++ ) {
    	for( x = x0 ; x < x0 + logo_width ; x++ ) {
    		if (count <= 0) {
    			count = *data++;
    			color = *data++;
    		}
            if (x > 0 && x < lcd_xres && y > 0 && y < lcd_yres)
            	lcd_buf[lcd_xres * y + x] = color;
            count--;
    	}
    }

	return ;
}

extern char lcd_is_enabled;
void lcd_enable(void)
{
	/* TODO: prevent lcd print */
	lcd_is_enabled = 0;

	/*hvga logo*/
	lcd_paint_logo();
	mdelay(100); /* wait for next vsync */

	set_backlight(1, 0x3f);
}

void lcd_disable(void)
{

}

void lcd_panel_disable(void)
{

}


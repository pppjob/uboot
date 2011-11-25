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

#ifndef ATXXFB_H_
#define ATXXFB_H_

/*-------------------------------------------------------------------------
 *
 * DEBUG definition
 *
 ------------------------------------------------------------------------*/
#define CONFIG_ATXX_LCD_DEBUG
#ifdef  CONFIG_ATXX_LCD_DEBUG
#define atxxfb_dbg(fmt, args...) \
	printf( fmt, ##args)
#define atxxfb_err(fmt, args...) \
	printf( fmt, ##args)
#else
#define atxxfb_dbg(fmt,arg...) do { } while (0)
#define atxxfb_err(fmt,arg...) do { } while (0)
#endif

/*-------------------------------------------------------------------------
 *
 *	bit fields for LCD controller registers
 *
 *-----------------------------------------------------------------------*/

#define	CTL0_BIT_MLCD_ON		0
#define	CTL0_BIT_SLCD_ON		1
#define	CTL0_BIT_MLCD_RST_FRAMENUM	2
#define	CTL0_BIT_SLCD_RST_FRAMENUM	3
#define	CTL0_BIT_MLCD_PWRON		4
#define	CTL0_BIT_SLCD_PWRON		5
#define	CTL0_BIT_MLCD_HSYNC_POL		6
#define	CTL0_BIT_MLCD_VSYNC_POL		7
#define	CTL0_BIT_SLCD_HSYNC_POL		8
#define	CTL0_BIT_SLCD_VSYNC_POL		9
#define	CTL0_BIT_MLCD_RHSCAN		10
#define	CTL0_BIT_SLCD_RHSCAN		11
#define	CTL0_BIT_MLCD_RVSCAN		12
#define	CTL0_BIT_SLCD_RVSCAN		13
#define	CTL0_BIT_MLCD_MODE		14
#define	CTL0_BIT_MLCD_CLKOUT_POL	15
#define	CTL0_BIT_PIX_FORMAT		16
#define	CTL0_BIT_ACTIVE_LCD		19
#define	CTL0_BIT_MLCD_INTEN		20
#define	CTL0_BIT_SLCD_INTEN		21
#define	CTL0_BIT_MLCD_INTSEL		22
#define	CTL0_BIT_SLCD_INTSEL		23
#define	CTL0_BIT_MLCD_INTSTA		24
#define	CTL0_BIT_MLCD_HBLANK		26
#define	CTL0_BIT_SLCD_HBLANK		27
#define	CTL0_BIT_MLCD_VBLANK		28
#define	CTL0_BIT_SLCD_VBLANK		29
#define	CTL0_REG_MLCD_PWR_STATUS	30
#define	CTL0_REG_SLCD_PWR_STATUS	31
#define	CTL0_PIX_FORMAT_MASK		~(0x7 << 16)	/* bit[18:16] */
#define	CTL0_MLCD_ON_MASK		~(1 << 0)	/* bit[0] */

#define	CTL4_BIT_SRC_PITCH_Y_M		0
#define	CTL4_BIT_SRC_PITCH_CBR_M	16
#define	CTL4_SRC_PITCH_Y_M_MASK		~(0x3FF << 0)	/* bit[9:0] */
#define	CTL4_SRC_PITCH_CBR_M_MASK	~(0x3FF << 16)	/* bit[25:16] */

#define	CTL9_BIT_VP_M			0
#define	CTL9_BIT_VS_M			16
#define	CTL9_BIT_VFP_M			24
#define	CTL9_VP_M_MASK			~(0x7FF << 0)	/* bit[10:0] */
#define	CTL9_VS_M_MASK			~(0x1F << 16)	/* bit[20:16] */
#define	CTL9_VFP_M_MASK			~(0xFF << 24)	/* bit[31:24] */

#define	CTL10_BIT_VBP_M			0
#define	CTL10_BIT_VDISP_M		8
#define	CTL10_VBP_M_MASK		~(0xF << 0)	/* bit[3:0] */
#define	CTL10_VDISP_M_MASK		~(0x3FF << 8)	/* bit[17:8] */

#define	CTL11_BIT_HP_M			0
#define	CTL11_BIT_HS_M			16
#define	CTL11_BIT_HFP_M			24
#define	CTL11_HP_M_MASK			~(0x7FF << 0)	/* bit[10:0] */
#define	CTL11_HS_M_MASK			~(0x1F << 16)	/* bit[20:16] */
#define	CTL11_HFP_M_MASK		~(0xFF << 24)	/* bit[31:24] */

#define	CTL12_BIT_HBP_M			0
#define	CTL12_BIT_HDISP_M		8
#define	CTL12_HBP_M_MASK		~(0xF << 0)	/* bi[3:0] */
#define	CTL12_HDISP_M_MASK		~(0x3FF << 8)	/* bit[17:8] */

#define	CTL20_BIT_DITHER_EN		8

#define	CTL21_GAMMA_REN_MASK		~(1 << 24)	/* bit[24] */
#define	CTL21_GAMMA_GEN_MASK		~(1 << 25)	/* bit[25] */
#define	CTL21_GAMMA_BEN_MASK		~(1 << 26)	/* bit[26] */
#define	CTL21_GAMMA_LUTEN_MASK		~(1 << 27)	/* bit[27] */

#define	CTL23_LBUF_THR_MASK		~(0xFF << 0)	/* bit[7:0] */
#define	CTL23_PAGE_SIZE_MASK		~(3 << 28)	/* bit[29:28] */
#define	CTL23_BAND_MODE_MASK		~(1 << 30)	/* bit[30] */
#define	CTL23_BANK_INTERLEAVE_MASK	~(1 << 31)	/* bit[31] */

#define	CTL24_BIT_XCS			0
#define	CTL24_BIT_SDOUT			1
#define	CTL24_BIT_SDIN			2
#define	CTL24_BIT_SCL			3
#define	CTL24_BIT_PWM_FREQ		8
#define	CTL24_BIT_PWM_DC		16
#define	CTL24_BIT_PWM_ENABLE		31
#define	CTL24_SDOUT_MASK		~(1 << 1)	/* bit[1] */
#define	CTL24_PWM_FREQ_MASK		~(0xFF << 8)	/* bit[15:8] */
#define	CTL24_PWM_DC_MASK		~(0xFF << 16)	/* bit[23:16] */

/*-------------------------------------------------------------------------
 *
 * etc define.
 *
 *-----------------------------------------------------------------------*/
#define DEFAULT_PCLK	13000000
#define MLCD_POWER_ON_OFF_TIMEOUT	0xff

#define LCD_XRES_240		240
#define LCD_XRES_320		320
#define LCD_XRES_480		480
#define LCD_XRES_800		800
#define LCD_YRES_320		320
#define LCD_YRES_640		640
#define LCD_YRES_480		480
#define LCD_YRES_600		600
#define LCD_YRES_800		800

/*-------------------------------------------------------------------------
 *
 * data structures.
 *
 *-----------------------------------------------------------------------*/
enum mlcd_on_off_control {
	MLCD_OFF = 0,
	MLCD_ON = 1,
};

enum mlcd_power_on_off_control {
	MLCD_POWER_OFF = 0,
	MLCD_POWER_ON = 1,
};

enum mlcd_irq_sel {
	MLCD_IRQ_VSYNC = 0,
	MLCD_IRQ_VBLANK = 1,
};

enum mlcd_irq {
	MLCD_IRQ_DIS = 0,
	MLCD_IRQ_EN = 1,
};

enum vga_mode {
	ATXX_QVGA = 0,
	ATXX_HVGA,
	ATXX_VGA,
	ATXX_WVGA,
	ATXX_SVGA
};

enum pix_format {
	RGB8 = 0,
	RGB16,
	RGB24,
	RGB32,
	YUY2,
	UYVY,
	YV16,
};

enum RGB16_format {
	RGB16_565 = 0,
	RGB16_5551,
};

struct atxxfb;
typedef void (*lcd_ops)(struct atxxfb*);

struct pannel_operation {
	lcd_ops initialize;
	lcd_ops panel_on;
	lcd_ops panel_off;
};

struct spi_timing_t {
	uint8_t tchw;
	uint8_t tcss;
	uint8_t tscyc;
	uint8_t tshw;
	uint8_t tslw;
	uint8_t tsds;
	uint8_t tsdh;
};

/* Interpretation of offset for color fields: All offsets are from the right,
 * inside a "pixel" value, which is exactly 'bits_per_pixel' wide (means: you
 * can use the offset as right argument to <<). A pixel afterwards is a bit
 * stream and is written to video memory as that unmodified. This implies
 * big-endian byte order if bits_per_pixel is greater than 8.
 */
struct fb_bitfield {
	__u32 offset;			/* beginning of bitfield	*/
	__u32 length;			/* length of bitfield		*/
	__u32 msb_right;		/* != 0 : Most significant bit is */
					/* right */
};

struct fb_var_screeninfo {
	unsigned int xres;		/* visible resolution		*/
	unsigned int yres;

	unsigned int bits_per_pixel;	/* guess what			*/

	/* Timing: All values in pixclocks, except pixclock (of course) */
	unsigned int pixclock;		/* pixel clock in ps (pico seconds) */
	unsigned int left_margin;	/* time from sync to picture	*/
	unsigned int right_margin;	/* time from picture to sync	*/
	unsigned int upper_margin;	/* time from sync to picture	*/
	unsigned int lower_margin;
	unsigned int hsync_len;		/* length of horizontal sync	*/
	unsigned int vsync_len;		/* length of vertical sync	*/
	unsigned int sync;		/* see FB_SYNC_*		*/
	unsigned int vmode;		/* see FB_VMODE_*		*/
	unsigned int rotate;		/* angle we rotate counter clockwise */

	struct fb_bitfield red;		/* bitfield in fb mem if true color, */
	struct fb_bitfield green;	/* else only length is significant */
	struct fb_bitfield blue;
	struct fb_bitfield transp;	/* transparency			*/
};

struct fb_info {
	struct fb_var_screeninfo var;	/* Current var */
	unsigned long smem_start;	/* Start of frame buffer mem */
					/* (physical address) */
	unsigned int smem_len;		/* Length of frame buffer mem */
	unsigned int type;		/* see FB_TYPE_*		*/
	unsigned int line_length;	/* length of a line in bytes    */

	char *screen_base;
	unsigned long screen_size;
	int logo_height;
	unsigned int logo_size;
	struct atxxfb *par;
};

struct atxxfb {
	struct fb_info *fb;
	void *base;
	struct clk *lcd_clk;
	uint32_t smem_addr;		/* record fb start address in suspend & resume */
	uint32_t lcd_pwr_flag;
	uint32_t xres, yres;		/* record set_var */
	struct spi_timing_t spi_timing;
	uint32_t pseudo_pal[16];
	uint32_t format;
	uint32_t resolution;

	void (*spi_8bits_cmd)(struct atxxfb *dev, uint8_t * buffer,
			uint32_t len);
	void (*spi_9bits_cmd)(struct atxxfb *dev, uint8_t buffer);
	void (*spi_9bits_par)(struct atxxfb *dev, uint8_t buffer);
	void (*set_csx)(struct atxxfb *dev, uint32_t level);
	uint16_t (*spi_8bits_read)(struct atxxfb *dev);
	struct pannel_operation *pannel_ops;
};

extern int pannel_set_ops(struct atxxfb *dev);
extern int pannel_set_power(int on_off);
extern void pannel_set_refresh_rate(struct clk *lcd_clk);
extern void lcd_show_logo(int logo_width, int logo_height, unsigned short *logo_data);
extern void InitLcdops(void);

#endif /* ATXXFB_H_ */

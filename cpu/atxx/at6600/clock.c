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
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/topctl.h>
#include <asm/arch-atxx/regs_base.h>

const char *pll_names[] = {
	"pll1",
	"pll2",
	"pll3",
};

const char *div_names[] = {
	"arm",
	"dsphclk",
	"comm",
	"uart",
	"ms",
	"camclk",
	"camera",
	"vpclk",
	"dspcore",
	"axi",
	"spi0",
	"spi1",
	"spi2",
	"sd",
	"usb1",
	"bt",
	"proc",
	"app",
	"gclk",
	"pclk",
	"mddr",
	"gps2",
	"hdmi_s0",
	"hdmi",
	"i2s0_s0",
	"i2s0",
	"i2s1_s0",
	"i2s1",
	"i2s2_s0",
	"i2s2",
	"vsclk",
	"d_not_use0",
	"usb0",
	"iqadc_s0",
	"vgaclk",
	"tvclk",
	"iqadc",
};

const char *app_names[] = {
	"a_i2c",
	"a_timer",
	"a_tsc",
	"a_keypad",
	"a_gpio",
	"a_wdt",
	"a_uart0",
	"a_uart1",
	"a_uart2",
	"a_spi0",
	"a_spi1",
	"a_spi2",
	"a_not_use0",
	"a_not_use1",
	"a_not_use2",
	"a_not_use3",
	"a_ictl",
	"a_sd",
	"a_usb",
	"a_nfc",
	"a_sdio",
	"a_dmac",
	"a_ms",
	"a_usb1",
};

const char *crydiv_names[] = {
	"crydiv_26M",
	"crydiv_13M",
};

/* parent, cascaded */
const int cascaded_div[] = {
	22, 23,		/* hdmi_s0, hdmi */
	24, 25,		/* i2s0_s0, i2s0 */
	26, 27,		/* i2s1_s0, i2s1 */
	28, 29,		/* i2s2_s0, i2s2 */
	33, 36,		/* iqadc_s0, iqadc */
};

#define	PLL_COUNT	ARRAY_SIZE(pll_names)
#define	DIV_COUNT	ARRAY_SIZE(div_names)
#define	CASDIV_COUNT	ARRAY_SIZE(cascaded_div)

struct clk clk_xtal;
struct clk clk_pll[PLL_COUNT];
struct clk clk_div[DIV_COUNT];

/* ****************************************************************** */

/* external crystal 26M hz */
#define	CRYSTAL_FREQ_HZ		(26 * MHZ)

static int atxx_xtal_is_enable(struct clk *clk)
{
	return 1;
}

/* ****************************************************************** */

/* PLL control registers */
#define	PLLCTLR(n)		(ATXX_PM_UNIT_BASE + 0x80 + 4 * (n))

/* PLL control register field offset */
#define	PLL_PD			0	/* Powerdown PLL */
#define	PLL_BP			1	/* Bypass PLL */
#define	PLL_BS			2	/* Band Selection */
#define	PLL_OD			4	/* Output Divider Control */
#define	PLL_F			8	/* Feedback Divider Control */
#define	PLL_R			16	/* Input Divider Control */

#define	PLL_OD_MASK		0x03
#define	PLL_F_MASK		0x7f
#define	PLL_R_MASK		0x1f

/* PLL setting for different frequency */
struct pll_set {
	unsigned int freq;
	unsigned int set;
};

static struct pll_set pll_table[] = {
	{
		0   * MHZ, (1 << PLL_PD),
	}, {
		26  * MHZ, (1 << PLL_BP),
	}, {
		312 * MHZ, (1 << PLL_BS) | (11 << PLL_F),
	}, {
		481 * MHZ, (1 << PLL_BS) | (36 << PLL_F) | (1 << PLL_R),
	}, {
		624 * MHZ, (2 << PLL_BS) | (23 << PLL_F),
	}, {
		708 * MHZ, (2 << PLL_BS) | (26 << PLL_F),
	},{
		728 * MHZ, (2 << PLL_BS) | (27 << PLL_F),
	}, {
		806 * MHZ, (2 << PLL_BS) | (30 << PLL_F),
	},
};
#define	PLL_TABLE_COUNT		ARRAY_SIZE(pll_table)

static int atxx_pll_enable(struct clk *clk, int enable)
{
	volatile uint32_t pllreg;

	pllreg = readl(PLLCTLR(clk->index));

	if (enable)
		pllreg &= ~(1 << PLL_PD);
	else
		pllreg |= 1 << PLL_PD;

	writel(pllreg, PLLCTLR(clk->index));
	return 0;
}

static int atxx_pll_is_enable(struct clk *clk)
{
	volatile uint32_t pllreg;

	pllreg = readl(PLLCTLR(clk->index));

	return !(pllreg & (1 << PLL_PD));
}

static int atxx_pll_set_rate(struct clk *clk, unsigned long rate)
{
	int i;

	/* Polling all divider, put corespond dvider to other PLL*/
	for(i = 0; i < PLL_TABLE_COUNT; i++) {
		if(pll_table[i].freq == rate) {
			writel(pll_table[i].set, PLLCTLR(clk->index));
			return 0;
		}
	}

	return -EINVAL;
}

static unsigned long atxx_pll_round_rate(struct clk *clk, unsigned long rate)
{
	int i;

	for(i = 0; i < PLL_TABLE_COUNT; i++) {
		if(pll_table[i].freq == rate) {
			return rate;
		}
	}

	return 0;
}

/* Fout = (Fin * (F + 1)) / ((R + 1) * (2 ^ OD)) */
static unsigned long atxx_pll_get_rate(struct clk *clk)
{
	volatile uint32_t pllreg, freq;

	freq = clk_get_rate(clk->parent);

	pllreg = readl(PLLCTLR(clk->index));
	if(pllreg & (1 << PLL_PD)) {
		/* PLL power down */

		return 0;
	} else if (pllreg & (1 << PLL_BP)) {
		/* Bypass is set, use parent crystal clock */

		return freq;
	}

	freq *= ((pllreg >> PLL_F) & PLL_F_MASK) + 1;
	freq /= ((pllreg >> PLL_R) & PLL_R_MASK) + 1;
	freq >>= ((pllreg >> PLL_OD) & PLL_OD_MASK);

	return freq;
}

static unsigned int atxx_pll_get_reg(struct clk *clk)
{
	return readl(PLLCTLR(clk->index));
}

/* ****************************************************************** */

/* Divider control registers, 0 - 36, 0x100 - 0x190  */
#define	DIVCTLR(n)		(ATXX_PM_UNIT_BASE + 0x100 + 4 * (n))

/* Divider field offset */
#define	DIV_EN			0
#define	DIV_N			8
#define	DIV_1			13
#define	DIV_SRC			16
#define	DIV_EXT			18
#define	DIV_INV			19

#define	DIV_N_MASK		0x1f
#define	DIV_SRC_MASK		0x03

#define	DIV_MAX			32

static struct clk *atxx_div_get_parent(struct clk *clk)
{
	volatile uint32_t divreg;
	int ext, src;

	divreg = readl(DIVCTLR(clk->index));
	ext = (divreg >> DIV_EXT) & 1;
	src = (divreg >> DIV_SRC) & DIV_SRC_MASK;

	if (ext) {
		printf("Use unknown external clock!!\n");
		return NULL;
	}
	if (src == 0) {
		return &clk_xtal;
	}

	return &clk_pll[src - 1];
}

static int atxx_div_set_parent(struct clk *clk, struct clk *parent)
{
	uint32_t divreg, src;

	if (parent == &clk_xtal)
		src = 0;
	else if (parent == &clk_pll[0])
		src = 1;
	else if (parent == &clk_pll[1])
		src = 2;
	else if (parent == &clk_pll[2])
		src = 3;
	else
		return -EINVAL;

	divreg = readl(DIVCTLR(clk->index));
	divreg &= ~(1 << DIV_EXT);
	divreg &= ~(DIV_SRC_MASK << DIV_SRC);
	divreg |= src << DIV_SRC;
	writel(divreg, DIVCTLR(clk->index));

	clk->parent = parent;

	return 0;
}

static int atxx_div_enable(struct clk *clk, int enable)
{
	volatile uint32_t divreg;

	divreg = readl(DIVCTLR(clk->index));

	if(enable)
		divreg |= 1 << DIV_EN;
	else
		divreg &= ~(1 << DIV_EN);

	writel(divreg, DIVCTLR(clk->index));
	return 0;
}

static int atxx_div_is_enable(struct clk *clk)
{
	volatile uint32_t divreg;

	divreg = readl(DIVCTLR(clk->index));

	return (divreg & (1 << DIV_EN));
}

static int atxx_div_set_rate(struct clk *clk, unsigned long rate)
{
	volatile uint32_t divreg;
	int div;

	if (rate == 0) {
		return -EINVAL;
	}

	/* use floor rate */
	div = (clk_get_rate(clk->parent) + rate - 1) / rate;
	div = (div < DIV_MAX) ? div : DIV_MAX;
	divreg = readl(DIVCTLR(clk->index));

	if (div == 1) {
		divreg |= 1 << DIV_1;
	} else {
		divreg &= ~(1 << DIV_1);
		divreg &= ~(DIV_N_MASK << DIV_N);
		divreg |= (div - 2) << DIV_N;
	}
	writel(divreg, DIVCTLR(clk->index));

	return 0;
}

static unsigned long atxx_div_get_rate(struct clk *clk)
{
	volatile uint32_t divreg;
	int div;

	divreg = readl(DIVCTLR(clk->index));

	if (divreg & (1 << DIV_1)) {
		div = 1;
	} else {
		div = ((divreg >> DIV_N) & DIV_N_MASK) + 2;
	}

	return clk_get_rate(clk->parent) / div;
}

static unsigned int atxx_div_get_reg(struct clk *clk)
{
	return readl(DIVCTLR(clk->index));
}
/* ****************************************************************** */

/* clocks enable for internal controllers(using app as clock source) */
static int atxx_app_enable(struct clk *clk, int enable)
{
	volatile uint32_t appreg;

	appreg = topctl_read_reg(TOPCGER);

	if (enable)
		appreg |= 1 << clk->index;
	else
		appreg &= ~(1 << clk->index);

	topctl_write_reg(TOPCGER, appreg);

	return 0;
}

static int atxx_app_is_enable(struct clk *clk)
{
	return (topctl_read_reg(TOPCGER) & (1 << clk->index));
}

static unsigned long atxx_app_get_rate(struct clk *clk)
{
	return clk_get_rate(clk->parent);
}

/* ****************************************************************** */

void set_board_default_clock(struct clock_default_setting *pll,
				struct clock_default_setting *div, 
				uint32_t pll_size, uint32_t div_size)
{
	int i;
	struct clk *clk;
	
	for (i = (div_size - 1); i >= 0 ; i--){
		clk = &clk_div[i];
		/* enable divider and change parent, if need*/
		if (div[i].parent_id != UNCHANGED) {
			if (div[i].parent_id == CRYSTAL) {
				clk_set_parent(clk, &clk_xtal);
			}else {
				clk_set_parent(clk, &clk_pll[(div[i].parent_id -1)]);
			}
		}
		if (div[i].enable != UNCHANGED) {
			if (div[i].enable){
				clk_enable(clk);
			}else {
				/* disable divider */
				clk_disable(clk);
			}
		}
	}

	for (i = 0; i < pll_size; i++) {
		clk = &clk_pll[i];

		/* enable pll or disable it */
		if (pll[i].enable != UNCHANGED) {
			if (pll[i].enable) {
				clk_enable(clk);
			}else {
				clk_disable(clk);
			}
		}
		/* set pll rate */
		if (pll[i].rate != UNCHANGED)
			clk_set_rate(clk, pll[i].rate);
	}

	for(i = 0; i < div_size; i++) {
		clk = &clk_div[i];
		if (div[i].rate != UNCHANGED) {
			clk_set_rate(clk, div[i].rate);
			
		}
	}
}

void at6600_clock_init(void)
{
	int i;

	/* register xtal clock */
	clk_xtal.parent = NULL;
	clk_xtal.name = "xtal";
	clk_xtal.rate = CRYSTAL_FREQ_HZ;
	clk_xtal.is_enable = atxx_xtal_is_enable;

	atxx_register_clock(&clk_xtal);

	/* register pll clocks */
	for(i = 0; i < PLL_COUNT; i++) {
		clk_pll[i].parent	= &clk_xtal;
		clk_pll[i].name		= pll_names[i];
		clk_pll[i].index	= i;
		clk_pll[i].enable	= atxx_pll_enable;
		clk_pll[i].is_enable	= atxx_pll_is_enable;
		clk_pll[i].set_rate	= atxx_pll_set_rate;
		clk_pll[i].round_rate	= atxx_pll_round_rate;
		clk_pll[i].get_rate	= atxx_pll_get_rate;
		clk_pll[i].get_reg	= atxx_pll_get_reg;

		atxx_register_clock(&clk_pll[i]);
	}

	/* register div clocks */
	for(i = 0; i < DIV_COUNT; i++) {
		clk_div[i].name		= div_names[i];
		clk_div[i].index	= i;
		clk_div[i].parent	= atxx_div_get_parent(&clk_div[i]);
		clk_div[i].set_parent	= atxx_div_set_parent;
		clk_div[i].enable	= atxx_div_enable;
		clk_div[i].is_enable	= atxx_div_is_enable;
		clk_div[i].set_rate	= atxx_div_set_rate;
		clk_div[i].round_rate	= NULL;
		clk_div[i].get_rate	= atxx_div_get_rate;
		clk_div[i].get_reg	= atxx_div_get_reg;

		atxx_register_clock(&clk_div[i]);
	}

	/* update the cascaded div clocks */
	for(i = 0; i < CASDIV_COUNT; i += 2) {
		clk_div[cascaded_div[i + 1]].parent	= &clk_div[cascaded_div[i]];
		clk_div[cascaded_div[i + 1]].set_parent	= NULL;
	}

}


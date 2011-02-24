/*------------------------------------------------------------------------------
* (bk) Copyright, Augusta Technology, Inc., 2006-present.
* (bk) Copyright, Augusta Technology USA, Inc., 2006-present.
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
#include <asm/arch-atxx/gpio.h>
#include <asm/arch-atxx/regs_base.h>

/* GPIO registers offset */
#define	GPIO_PORTA_DR		0x00
#define	GPIO_PORTA_DDR		0x04
#define	GPIO_PORTA_CTL		0x08
#define	GPIO_INTEN		0x30
#define	GPIO_INTMASK		0x34
#define	GPIO_INTTYPE_LEVEL	0x38
#define	GPIO_INT_POLARITY	0x3C
#define	GPIO_INTSTATUS		0x40
#define	GPIO_RAWINTSTATUS	0x44
#define	GPIO_DEBOUNCE		0x48
#define	GPIO_PORTA_EOI		0x4C
#define	GPIO_EXT_PORTA		0x50
#define	GPIO_LS_SYNC		0x60
#define	GPIO_ID_CODE		0x64
#define	GPIO_COMP_VERSION	0x6C

struct atxx_gpio_bank {
	uint32_t		regbase;	/* register base */
};

static struct atxx_gpio_bank atxx_gpio_banks[2] = {
	{ ATXX_GPIO_BASE},
	{ ATXX_GPIOB_BASE},
};

static inline struct atxx_gpio_bank *atxx_gpio_to_bank(uint32_t gpio)
{
	return &atxx_gpio_banks[(gpio >> 5) & 1];
}

static inline uint32_t atxx_gpio_read_reg(struct atxx_gpio_bank *bk, uint32_t reg)
{
	return __raw_readl(bk->regbase + reg);
}

static inline void atxx_gpio_write_reg(struct atxx_gpio_bank *bk, 
		uint32_t val, uint32_t reg)
{
	__raw_writel(val, bk->regbase + reg);
}

static inline void atxx_gpio_set_bit(struct atxx_gpio_bank *bk, 
		uint32_t nr, uint32_t reg)
{
	uint32_t val, addr;

	addr = bk->regbase + reg;
	val = __raw_readl(addr);
	val |= 1 << nr;
	val = __raw_writel(val, addr);
}

static inline void atxx_gpio_clear_bit(struct atxx_gpio_bank *bk, 
		uint32_t nr, uint32_t reg)
{
	uint32_t val, addr;

	addr = bk->regbase + reg;
	val = __raw_readl(addr);
	val &= ~(1 << nr);
	val = __raw_writel(val, addr);
}

/* ****************************************************************** */

static int check_gpio(int gpio)
{
	if ((gpio >= 0) && (gpio < 64)) {
		return 0;
	}

	printf("ERROR : check_gpio: invalid GPIO %d\n", gpio);
	return -1;
}

int atxx_set_gpio_direction(int gpio, int is_input)
{
	int offset;
	struct atxx_gpio_bank *bk;

	bk = atxx_gpio_to_bank(gpio);
	offset = gpio & 0x1f;

	if (is_input) {
		atxx_gpio_clear_bit(bk, offset, GPIO_PORTA_DDR);
	} else {
		atxx_gpio_set_bit(bk, offset, GPIO_PORTA_DDR);
	}

	return 0;
}

int atxx_gpio_get(int gpio)
{
	int offset;
	struct atxx_gpio_bank *bk;

	bk = atxx_gpio_to_bank(gpio);
	offset = gpio & 0x1f;

	return (atxx_gpio_read_reg(bk, GPIO_EXT_PORTA) >> offset) & 1;
}

void atxx_gpio_set(int gpio, int value)
{
	int offset;
	struct atxx_gpio_bank *bk;

	bk = atxx_gpio_to_bank(gpio);
	offset = gpio & 0x1f;

	if (value) {
		atxx_gpio_set_bit(bk, offset, GPIO_PORTA_DR);
	} else {
		atxx_gpio_clear_bit(bk, offset, GPIO_PORTA_DR);
	}
}

int atxx_request_gpio(int gpio)
{
	if (check_gpio(gpio) < 0)
		return -EINVAL;

	return 0;
}

void atxx_free_gpio(int gpio)
{
	return;
}


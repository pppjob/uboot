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

#ifndef __ARM_ARCH_CLOCK_H
#define __ARM_ARCH_CLOCK_H

#include <linux/list.h>

#define	MHZ			(1000 * 1000)
#define UNCHANGED		(-1)

enum parent_table {
	CRYSTAL,
	PLL1,
	PLL2,
	PLL3,
};

struct clock_default_setting {
	enum parent_table	parent_id;
	int		enable;
	unsigned long	rate;
};

/* internal data struct for ATXX clock system.
 * 	is_enable() is MUST for register,
 * 	either rate or get_rate() is MUST
 */
struct clk {
	struct list_head        list;
	struct clk              *parent;
	const char              *name;
	unsigned long           rate;	/* set if this clock rate is fixed */
	unsigned int	        index;

	int                     (*set_parent)(struct clk *c, struct clk *parent);
	int                     (*enable)(struct clk *c, int enable);
	int                     (*is_enable)(struct clk *c);	/* for initial register */
	int                     (*set_rate)(struct clk *c, unsigned long rate);
	unsigned int            (*get_reg)(struct clk *c);
	unsigned long           (*round_rate)(struct clk *c, unsigned long rate);
	unsigned long           (*get_rate)(struct clk *c);	/* dynamical rate */
};

struct clk *clk_get(const char *id);
int clk_enable(struct clk *clk);
void clk_disable(struct clk *clk);
unsigned long clk_get_rate(struct clk *clk);
unsigned long clk_round_rate(struct clk *clk, unsigned long rate);
int clk_set_rate(struct clk *clk, unsigned long rate);
struct clk *clk_get_parent(struct clk *clk);
int clk_set_parent(struct clk *clk, struct clk *parent);
int atxx_register_clock(struct clk *clk);
void at6600_clock_init(void);
void set_board_default_clock(struct clock_default_setting *pll,
				struct clock_default_setting *div, 
				uint32_t pll_size, uint32_t div_size);
void dump_clock(void);

#endif /* __ARM_ARCH_CLOCK_H */

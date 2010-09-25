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
#include <linux/types.h>
#include <linux/list.h>
#include <linux/err.h>
#include <common.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/bug.h>
/* ATXX clock API implementation */

static LIST_HEAD(clocks);

void dump_clock(void)
{
	struct clk *p;

	printf("%-16s%-16s%-16s%-16s%-16s\n",
			"NAME", "PARENT", "RATE", "ENABLE", "REG");

	list_for_each_entry(p, &clocks, list) {
		printf("%-16s", p->name);
		if (p->parent) {
			printf("%-16s", p->parent->name);
		} else {
			printf("%-16s", "NULL");
		}

		printf("%-16ld", clk_get_rate(p));
		printf("%-16d", p->is_enable(p));

		if (p->get_reg){
			printf("0x%08x", p->get_reg(p));
		}
		printf("\n");
	}
	printf("\n");

	return;
}

/* only match clock id */
struct clk *clk_get(const char *id)
{
	struct clk *p, *clk = ERR_PTR(-ENOENT);

	list_for_each_entry(p, &clocks, list) {
		if (strcmp(id, p->name) == 0) {
			clk = p;
			break;
		}
	}

	return clk;
}

int clk_enable(struct clk *clk)
{
	if (IS_ERR(clk) || clk == NULL)
		return -EINVAL;
	if(!clk->is_enable(clk))
		(clk->enable)(clk, 1);

	return 0;
}

void clk_disable(struct clk *clk)
{
	if (IS_ERR(clk) || clk == NULL)
		return;
	
	if(clk->is_enable(clk))
		(clk->enable)(clk, 0);

}

unsigned long clk_get_rate(struct clk *clk)
{
	if (IS_ERR(clk) || clk == NULL)
		return 0;

	if (clk->rate != 0)
		return clk->rate;

	if (clk->get_rate != NULL)
		return (clk->get_rate)(clk);

	if (clk->parent != NULL)
		return clk_get_rate(clk->parent);

	return clk->rate;
}

unsigned long clk_round_rate(struct clk *clk, unsigned long rate)
{
	if (!IS_ERR(clk) && clk && clk->round_rate)
		return (clk->round_rate)(clk, rate);

	return rate;
}

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	int ret;
	int e_switch = 0;

	if (IS_ERR(clk) || clk == NULL)
		return -EINVAL;

	WARN_ON(clk->set_rate == NULL);

	if (clk->set_rate == NULL)
		return -EINVAL;
	if (clk->is_enable == NULL)
		return -EINVAL;

	if (!clk->is_enable(clk)) {
		clk->enable(clk, 1);
		e_switch = 1;
	}
	ret = (clk->set_rate)(clk, rate);

	if(e_switch){
		clk->enable(clk, 0);
	}
	return ret;
}

struct clk *clk_get_parent(struct clk *clk)
{
	struct clk *r_clk = ERR_PTR(-ENOENT);

	if (IS_ERR(clk) || clk == NULL)
		return r_clk;

	return clk->parent;
}

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	int ret = 0;

	if (IS_ERR(clk) || clk == NULL)
		return -EINVAL;

	if (clk->set_parent)
		ret = (clk->set_parent)(clk, parent);

	return ret;
}

int atxx_register_clock(struct clk *clk)
{

	/* add to the list of available clocks */
	list_add_tail(&clk->list, &clocks);

	return 0;
}


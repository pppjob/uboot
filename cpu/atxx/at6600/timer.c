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
 -----------------------------------------------------------------------------*/

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/errno.h>
#include <asm/arch-atxx/regs_base.h>

/* Timer register offset */
#define	ATXX_TIMER_LOAD_REG	0x00	/* load count */
#define	ATXX_TIMER_CURR_REG	0x04	/* current value */
#define	ATXX_TIMER_CNTL_REG	0x08	/* control reg */
#define	ATXX_TIMER_NEOI_REG	0x0C	/* end of interrupt */
#define	ATXX_TIMER_STAT_REG	0x10	/* intr status */

#define	ATXX_TIMER_ADDR(id)	(ATXX_TIMER_BASE + 0x14 * id)
#define	ATXX_TIMER_LOAD(id)	(ATXX_TIMER_ADDR(id) + ATXX_TIMER_LOAD_REG)
#define	ATXX_TIMER_CURR(id)	(ATXX_TIMER_ADDR(id) + ATXX_TIMER_CURR_REG)
#define	ATXX_TIMER_CNTL(id)	(ATXX_TIMER_ADDR(id) + ATXX_TIMER_CNTL_REG)
#define	ATXX_TIMER_NEOI(id)	(ATXX_TIMER_ADDR(id) + ATXX_TIMER_NEOI_REG)
#define	ATXX_TIMER_STAT(id)	(ATXX_TIMER_ADDR(id) + ATXX_TIMER_STAT_REG)

/* Timer control bits */
#define	ATXX_TIMER_ENABLE	(1 << 0)
#define	ATXX_TIMER_USER_MODE	(1 << 1)
#define	ATXX_TIMER_INTR_MASK	(1 << 2)
#define	ATXX_TIMERS_TOTAL	8	/* timer 0 ~ 7 */
#define TIMER_FREQ_HZ		(32768)

#define	ATXX_SYS_TIMER_ID	0
#define	ATXX_SYS_COUNTER_ID	1
#define	ATXX_REF_COUNTER_ID	2

#define FREE_RUN_MODE	0
#define COUNT_MODE	1

static inline uint32_t timer_read_reg(uint32_t addr)
{
	return readl(addr);
}

static inline void timer_write_reg(uint32_t val, uint32_t addr)
{
	writel(val, addr);
}

static void timer_disable(int id)
{
	timer_write_reg(0, ATXX_TIMER_CNTL(id));
}


static void timer_enable(int id, int mode)
{
	uint32_t t1, t2;
	switch (mode) {
		case FREE_RUN_MODE:
			/* enable reference counter , free-run mode */
			timer_write_reg(ATXX_TIMER_ENABLE | ATXX_TIMER_INTR_MASK,
					ATXX_TIMER_CNTL(id));
			break;
		case COUNT_MODE:
			
			t1 = timer_read_reg(ATXX_TIMER_CURR(ATXX_REF_COUNTER_ID));
			timer_write_reg(ATXX_TIMER_USER_MODE 
				| ATXX_TIMER_ENABLE, ATXX_TIMER_CNTL(id));
			t2 = timer_read_reg(ATXX_TIMER_CURR(ATXX_REF_COUNTER_ID));
			if (t1 != t2) {
				timer_write_reg(0, ATXX_TIMER_CNTL(id));
				timer_write_reg(ATXX_TIMER_USER_MODE 
					| ATXX_TIMER_ENABLE, ATXX_TIMER_CNTL(id));
			}
			break;
		default:
			printf("No support\n");
			break;
		}
	return;
}

static int timer_reset(int id, uint32_t value, int mode)
{
	if (id >= ATXX_TIMERS_TOTAL)
		return -EINVAL;
	timer_disable(id);
	timer_write_reg(value, ATXX_TIMER_LOAD(id));
	timer_enable(id, mode);
	return 0;
}

#define ULONG_MAX (~0UL)
static inline unsigned long timer_read_cycles(int index)
{
	uint32_t t1,t2,t3;
	
	t1 = timer_read_reg(ATXX_TIMER_CURR(index));
	t2 = timer_read_reg(ATXX_TIMER_CURR(index));
	t3 = timer_read_reg(ATXX_TIMER_CURR(index));

	if ( ((t1 - t2) & ~1) == 0)
		return (unsigned long)(ULONG_MAX - t2);
	else
		return (unsigned long)(ULONG_MAX - t3);
}

static inline unsigned int timer_get_status(unsigned int id)
{
	unsigned int status;
	
	status = timer_read_reg(ATXX_TIMER_STAT(id));
	if (status & 0x1)
	{
		/*read EOI to clear timerx interrupt.*/
		status = timer_read_reg(ATXX_TIMER_NEOI(id));
		return 1;
	}
	return 0;
}
#ifdef USE_COUNT_MODE
void mdelay (unsigned long msec)
{
	unsigned long ticks;
	unsigned int time_status;

	ticks = msec * TIMER_FREQ_HZ / 1000;

	timer_reset(ATXX_SYS_COUNTER_ID, ticks, COUNT_MODE);
	
	time_status = timer_get_status(ATXX_SYS_COUNTER_ID);
	while (!time_status){
		time_status = timer_get_status(ATXX_SYS_COUNTER_ID);
	}

}
#else
void mdelay (unsigned long msec)
{
	unsigned long start, end, ticks;

	ticks = (msec * TIMER_FREQ_HZ) / 1000;
	start = timer_read_cycles(ATXX_SYS_TIMER_ID);
	do {
		end = timer_read_cycles(ATXX_SYS_TIMER_ID);
	}while(((ULONG_MAX + end - start) % ULONG_MAX) < ticks);

	return;	
}

#endif
static inline unsigned long tick_to_ms(unsigned long tick)
{
	tick *= 1000;
	tick /= TIMER_FREQ_HZ;
	return tick;
}

static inline unsigned long us_to_tick(unsigned long us)
{
	unsigned long tmp;

	tmp = (us * TIMER_FREQ_HZ) / 1000000;
	if (tmp == 0)
		return 1;
	return tmp;
}

/* return msec */
unsigned long get_timer (unsigned long base)
{
	unsigned long tick;

	tick = timer_read_cycles(ATXX_SYS_TIMER_ID);

	return tick_to_ms(tick) - base;
}

void udelay(unsigned long time)
{
	unsigned long tick;
	unsigned long start, end;

	if (time > 2000) {
		mdelay(time/1000);
	} else {
			start = timer_read_cycles(ATXX_SYS_TIMER_ID);
			tick = us_to_tick(time);
			do {
				end = timer_read_cycles(ATXX_SYS_TIMER_ID);
			}while(((ULONG_MAX + end - start) % ULONG_MAX) < tick);
	}
}

int timer_init (void)
{
	//calibrate_delay();

	timer_enable(ATXX_REF_COUNTER_ID, FREE_RUN_MODE);
 	timer_enable(ATXX_SYS_TIMER_ID, FREE_RUN_MODE);
}


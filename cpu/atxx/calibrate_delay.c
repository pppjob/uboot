#include <common.h>
#include <asm/arch-atxx/clock.h>

struct loop_item {
	unsigned long rate;
	unsigned int  loop;
};

static struct loop_item loop_table[] = {
	{312 * MHZ, 1560000},
	{624 * MHZ, 3112960},
	{702 * MHZ, 3510000},
};

#define LOOP_TABLE_SIZE		ARRAY_SIZE(loop_table)
unsigned int    loops_per_jiffy;

void calibrate_delay(void)
{
	int i;
	unsigned long rate;

	loops_per_jiffy = 0;
	rate = clk_get_rate(clk_get("arm"));
	for (i = 0; i < LOOP_TABLE_SIZE; i++){
		if(rate == loop_table[i].rate){
			loops_per_jiffy = loop_table[i].loop;
		}
	}

	if (loops_per_jiffy == 0){
		loops_per_jiffy = 3120000;
		printf("loops_per_jiffy NULL, rate: %ld\n", rate);
	}
	return;
}


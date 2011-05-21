#include <asm/arch-atxx/delay.h>
#include <common.h>

void udelay(unsigned long n)
{
	do {
		if (n < 2000){
			__udelay(n);
			break;
			} 
		else {
			__udelay(2000);
			n -= 2000;
		}
	}while(1);

	return;
}

void mdelay(unsigned long n)
{
	u32 t1, t2;

	t1 = get_timer (0);
	do {
		t2 = get_timer (0);
		if ((t2 - t1) >= n)
			break;
	} while (1);

	return;
}


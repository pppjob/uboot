#include <asm/arch-atxx/delay.h>

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
	udelay(n * 1000);
	return;
}


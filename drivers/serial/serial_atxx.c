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
#include <serial.h>
#include <asm/io.h>
#include "serial_atxx.h"
#include <asm/arch-atxx/regs_base.h>


uart_t uart;

static int dwapbuart_init(uart_t *p_uart);
static void dwapbuart_putc(char ch);
static int dwapbuart_getc(void);
static int dwapbuart_tstc(uart_t *p_uart);
static void dwapbuart_setbrg(uart_t *p_uart);

static void serial_write_reg(uint32_t reg, uint32_t val)
{
	writel(val, ATXX_UART0_BASE + reg);
}

static uint32_t serial_read_reg(uint32_t reg)
{
	return readl(ATXX_UART0_BASE + reg);
}

unsigned long uart_get_divisor(unsigned long clkfreq, unsigned long baudrate)
{
	return ((clkfreq/baudrate) >> 4);
}

static void uart_clear_rx_fifo(void)
{
	uint32_t val;

	val = serial_read_reg(UART_FCR_OFS);
	val |= bUART_FCR_RFIFOR;
	serial_write_reg(UART_FCR_OFS, val);
}

static void uart_clear_tx_fifo(void)
{

	uint32_t val;

	val = serial_read_reg(UART_FCR_OFS);
	val |= bUART_FCR_XFIFOR;
	serial_write_reg(UART_FCR_OFS, val);
}

static int dwapbuart_init(uart_t *p_uart)
{
	unsigned long val;

	/* FIFO control */
	serial_write_reg(UART_FCR_OFS, p_uart->fifo_cfg);
	/* enable DLL/DLH R/W */
	serial_write_reg(UART_LCR_OFS, bUART_LCR_DLAB);
	/* CFG LOW */
	val = uart_get_divisor(p_uart->clkfreq, p_uart->baudrate);
	serial_write_reg(UART_DLL_OFS, (val & 0xFF));
	/* CFG HIGH */
	serial_write_reg(UART_DLH_OFS, (val >> 8));

	/* 8bits, No parity */
	serial_write_reg(UART_LCR_OFS, bUART_LCR_DLS);

	/* enable loopback */
	if(p_uart->loop_enable == 1)
		serial_write_reg(UART_MCR_OFS, bUART_MCR_LB);
	else	/* disable loopback */
		serial_write_reg(UART_MCR_OFS, ~bUART_MCR_LB);

	uart_clear_rx_fifo();
	uart_clear_tx_fifo();

	return 0;
}

static void dwapbuart_putc(char ch)
{
	uint32_t val;
	
	/* wait TX empty */
	do{
		val = serial_read_reg(UART_LSR_OFS);
	} while( (val & bUART_LSR_THRE) == 0 );
	/* send a character */
	serial_write_reg(UART_THR_OFS, ch);
}


static int dwapbuart_getc()
{
	uint32_t val;
	
	/*wait for data ready */

	do{
		val = serial_read_reg(UART_LSR_OFS);
	} while(!(val & bUART_LSR_DR));

	return serial_read_reg(UART_RBR_OFS);
}

static int dwapbuart_tstc(uart_t *p_uart)
{
	uint32_t val;

	val = serial_read_reg(UART_LSR_OFS);
	return ((val & bUART_LSR_DR));
}

static void dwapbuart_setbrg(uart_t *p_uart)
{
	/* not implemented yet */
}

#ifdef CONFIG_SERIAL_MULTI

static int atxx_ffuart_init(void)
{
	uart_t *p_uart = &uart;

	p_uart->clkfreq = CFG_UART_CLOCK_FREQ;
	p_uart->baudrate = CONFIG_BAUDRATE;
#ifdef CFG_UART_FIFO_ON
	p_uart->fifo_cfg = bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF | bUART_FCR_FIFOE;
#else
	p_uart->fifo_cfg = bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF;
#endif
	p_uart->loop_enable = CFG_UART_LOOPENABLE;

	dwapbuart_init(p_uart);

	return 0;
}

static void atxx_ffuart_putc(const char c)
{
	if (c == '\n')
		dwapbuart_putc('\r');

	dwapbuart_putc(c);
}

static int atxx_ffuart_getc(void)
{
	return dwapbuart_getc();
}

static void atxx_ffuart_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

static void atxx_ffuart_setbrg(void)
{
	uart_t *p_uart = &uart;
 	dwapbuart_setbrg(p_uart);
}

static int atxx_ffuart_tstc(void)
{
	uart_t *p_uart = &uart;

	return dwapbuart_tstc(p_uart);
}

struct serial_device atxx_ffuart_device =
{
	"atxx_ffuart",
	"ATK",
	atxx_ffuart_init,
	atxx_ffuart_setbrg,
	atxx_ffuart_getc,
	atxx_ffuart_tstc,
	atxx_ffuart_putc,
	atxx_ffuart_puts,
};
#else
int serial_init(void)
{
	uart_t *p_uart = &uart;

	p_uart->clkfreq = CFG_UART_CLOCK_FREQ;
	p_uart->baudrate = CONFIG_BAUDRATE;
#ifdef CFG_UART_FIFO_ON
	p_uart->fifo_cfg = bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF | bUART_FCR_FIFOE;
#else
	p_uart->fifo_cfg = bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF;
#endif
	p_uart->loop_enable = CFG_UART_LOOPENABLE;

	return dwapbuart_init(p_uart);
}

void serial_putc(const char ch)
{
	uart_t *p_uart = &uart;

	if (ch == '\n')
		dwapbuart_putc('\r');

	dwapbuart_putc(ch);
}

void serial_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

int serial_getc(void)
{
	uart_t *p_uart = &uart;

	return dwapbuart_getc();
}

int serial_tstc(void)
{
	uart_t *p_uart = &uart;

	return dwapbuart_tstc(p_uart);
}

void serial_setbrg(void)
{
	uart_t *p_uart = &uart;
 	dwapbuart_setbrg(p_uart);
}
#endif /* CONFIG_SERIAL_MULTI */

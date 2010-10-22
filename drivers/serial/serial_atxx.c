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
#include <asm/arch-atxx/delay.h>

static int gsm_bridge_flag = 0;

/*--------------------------- basic operation----------------------------------*/

static void serial_write_reg(uint32_t base_addr, uint32_t reg, uint32_t val)
{
	writel(val, base_addr + reg);
}

static uint32_t serial_read_reg(uint32_t base_addr, uint32_t reg)
{
	return readl(base_addr + reg);
}

unsigned long uart_get_divisor(unsigned long clkfreq, unsigned long baudrate)
{
	return ((clkfreq/baudrate) >> 4);
}

static void uart_clear_rx_fifo(uint32_t base_addr)
{
	uint32_t val;

	val = serial_read_reg(base_addr, UART_FCR_OFS);
	val |= bUART_FCR_RFIFOR;
	serial_write_reg(base_addr, UART_FCR_OFS, val);
}

static void uart_clear_tx_fifo(uint32_t base_addr)
{

	uint32_t val;

	val = serial_read_reg(base_addr, UART_FCR_OFS);
	val |= bUART_FCR_XFIFOR;
	serial_write_reg(base_addr, UART_FCR_OFS, val);
}

static void uart_set_flowcontrol(uint32_t base_addr, uint32_t enable)
{
	uint32_t val;

	if (enable) {
		val = serial_read_reg(base_addr, UART_MCR_OFS);
		serial_write_reg(base_addr, UART_MCR_OFS, 
				val | bUART_MCR_AFCE | bUART_MCR_RTS);
		serial_write_reg(base_addr, UART_FCR_OFS, 
				bUART_FCR_RCVR_2LESS | bUART_FCR_TET_ONE 
				| bUART_FCR_FIFOE);
	} else {
		val = serial_read_reg(base_addr, UART_MCR_OFS);
		val |= bUART_MCR_RTS;
		serial_write_reg(base_addr, UART_MCR_OFS, val & ~bUART_MCR_AFCE);
	}
}


static int dwapbuart_init(uint32_t base_addr, uart_t *p_uart)
{
	unsigned long val, val1;

	uart_clear_rx_fifo(base_addr);
	uart_clear_tx_fifo(base_addr);

	/* FIFO control */
	if (p_uart->fifo_cfg == 1) {
		serial_write_reg(base_addr, UART_FCR_OFS, 
				bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF | bUART_FCR_FIFOE);
	} else if (p_uart->fifo_cfg == 2) {
		serial_write_reg(base_addr, UART_FCR_OFS, 
				bUART_FCR_RCVR_2LESS | bUART_FCR_TET_ONE | bUART_FCR_FIFOE);
	} else {
		serial_write_reg(base_addr, UART_FCR_OFS, 
				bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF);
	}

	/* enable DLL/DLH R/W */
	serial_write_reg(base_addr, UART_LCR_OFS, bUART_LCR_DLAB);

	/* config DLL/DLH */
	val = uart_get_divisor(p_uart->clkfreq, p_uart->baudrate);
	serial_write_reg(base_addr, UART_DLL_OFS, (val & 0xFF));
	serial_write_reg(base_addr, UART_DLH_OFS, (val >> 8));

	/* 8bits, No parity */
	serial_write_reg(base_addr, UART_LCR_OFS, bUART_LCR_DLS);

	/* enable loopback */
	if(p_uart->loop_enable == 1) {
		val = serial_read_reg(base_addr, UART_MCR_OFS);
		serial_write_reg(base_addr, UART_MCR_OFS, val | bUART_MCR_LB);
	} else {	/* disable loopback */
		val = serial_read_reg(base_addr, UART_MCR_OFS);
		serial_write_reg(base_addr, UART_MCR_OFS, val & ~bUART_MCR_LB);
	}

	uart_clear_rx_fifo(base_addr);
	uart_clear_tx_fifo(base_addr);

	uart_set_flowcontrol(base_addr, p_uart->flow_control);

	return 0;
}

static void dwapbuart_putc(uint32_t base_addr, char ch)
{
	uint32_t val;
	
	/* wait TX empty */
	do{
		val = serial_read_reg(base_addr, UART_LSR_OFS);
	} while( (val & bUART_LSR_THRE) == 0 );
	/* send a character */
	serial_write_reg(base_addr, UART_THR_OFS, ch);
}


static int dwapbuart_getc(uint32_t base_addr)
{
	uint32_t val;
	
	/*wait for data ready */

	do{
		val = serial_read_reg(base_addr, UART_LSR_OFS);
	} while(!(val & bUART_LSR_DR));

	return serial_read_reg(base_addr, UART_RBR_OFS);
}

static int dwapbuart_tstc(uint32_t base_addr)
{
	uint32_t val;

	val = serial_read_reg(base_addr, UART_LSR_OFS);
	return ((val & bUART_LSR_DR));
}

static void dwapbuart_setbrg(uint32_t base_addr)
{
	/* not implemented yet */
}

#ifdef CONFIG_SERIAL_MULTI
/*--------------------------- uart0 ----------------------------------------------------*/
static int atxx_uart0_init(void)
{
	uart_t uart;

	uart.clkfreq = CFG_UART_CLOCK_FREQ;
	uart.baudrate = CONFIG_BAUDRATE;
#ifdef CFG_UART_FIFO_ON
	uart.fifo_cfg = 1;
#else
	uart.fifo_cfg = 0;
#endif
	uart.loop_enable = CFG_UART_LOOPENABLE;
	uart.flow_control = CFG_UART_FLOWCONTROL_ENABLE;

	dwapbuart_init(ATXX_UART0_BASE, &uart);

	return 0;
}

static int atxx_uart0_init_adv(uart_t *p_uart)
{
	dwapbuart_init(ATXX_UART0_BASE, p_uart);

	return 0;
}

static void atxx_uart0_putc(const char c)
{
	if (!gsm_bridge_flag) {
		if (c == '\n')
			dwapbuart_putc(ATXX_UART0_BASE, '\r');
	}

	dwapbuart_putc(ATXX_UART0_BASE, c);
}

static int atxx_uart0_getc(void)
{
	return dwapbuart_getc(ATXX_UART0_BASE);
}

static void atxx_uart0_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

static void atxx_uart0_setbrg(void)
{
 	dwapbuart_setbrg(ATXX_UART0_BASE);
}

static int atxx_uart0_tstc(void)
{
	return dwapbuart_tstc(ATXX_UART0_BASE);
}

static int atxx_uart0_get_bridge(char *pch)
{
	uint32_t val;

	gsm_bridge_flag = 1;
	do{
		val = serial_read_reg(ATXX_UART0_BASE, UART_LSR_OFS);
		if ((val & bUART_LSR_DR) != 0) {
			*pch = serial_read_reg(ATXX_UART0_BASE, UART_RBR_OFS);
			return 1;
		}

		val = serial_read_reg(ATXX_UART1_BASE, UART_LSR_OFS);
		if ((val & bUART_LSR_DR) != 0) {
			*pch = serial_read_reg(ATXX_UART1_BASE, UART_RBR_OFS);
			return 2;
		}

	} while(1);
}

/*--------------------------- uart1 ----------------------------------------------------*/
static int atxx_uart1_init(void)
{
	uart_t uart;
	uart.clkfreq = CFG_UART_CLOCK_FREQ;
	uart.baudrate = CONFIG_BAUDRATE;
#ifdef CFG_UART_FIFO_ON
	uart.fifo_cfg = 1;
#else
	uart.fifo_cfg = 1;
#endif
	uart.loop_enable = CFG_UART_LOOPENABLE;
	uart.flow_control = CFG_UART_FLOWCONTROL_ENABLE;

	dwapbuart_init(ATXX_UART1_BASE, &uart);

	return 0;
}

static int atxx_uart1_init_adv(uart_t *p_uart)
{
	dwapbuart_init(ATXX_UART1_BASE, p_uart);

	return 0;
}

static void atxx_uart1_putc(const char c)
{
	if (!gsm_bridge_flag) {
		if (c == '\n')
			dwapbuart_putc(ATXX_UART1_BASE, '\r');
	}
	dwapbuart_putc(ATXX_UART1_BASE, c);
}

static int atxx_uart1_getc(void)
{
	return dwapbuart_getc(ATXX_UART1_BASE);
}

static void atxx_uart1_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

static void atxx_uart1_setbrg(void)
{
 	dwapbuart_setbrg(ATXX_UART1_BASE);
}

static int atxx_uart1_tstc(void)
{
	return dwapbuart_tstc(ATXX_UART1_BASE);
}

static int atxx_uart1_get_bridge(char *pch)
{
	uint32_t val;

	gsm_bridge_flag = 1;
	do{
		val = serial_read_reg(ATXX_UART0_BASE, UART_LSR_OFS);
		if ((val & bUART_LSR_DR) != 0) {
			*pch = serial_read_reg(ATXX_UART0_BASE, UART_RBR_OFS);
			return 1;
		}

		val = serial_read_reg(ATXX_UART1_BASE, UART_LSR_OFS);
		if ((val & bUART_LSR_DR) != 0) {
			*pch = serial_read_reg(ATXX_UART1_BASE, UART_RBR_OFS);
			return 2;
		}

	} while(1);
}

struct serial_device atxx_uart0_device =
{
	"serial0",
	"uart0",
	atxx_uart0_init,
	atxx_uart0_setbrg,
	atxx_uart0_getc,
	atxx_uart0_tstc,
	atxx_uart0_putc,
	atxx_uart0_puts,
	atxx_uart0_get_bridge,
	atxx_uart0_init_adv,
};

struct serial_device atxx_uart1_device =
{
	"serial1",
	"uart1",
	atxx_uart1_init,
	atxx_uart1_setbrg,
	atxx_uart1_getc,
	atxx_uart1_tstc,
	atxx_uart1_putc,
	atxx_uart1_puts,
	atxx_uart1_get_bridge,
	atxx_uart1_init_adv,
};

#else
#endif /* CONFIG_SERIAL_MULTI */

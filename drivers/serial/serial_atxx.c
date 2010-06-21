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
#ifdef CFG_DWAPBUART_SERIAL

#include <asm/io.h>
#include "serial_atxx.h"
#include <asm/arch-atxx/regs_base.h>

#ifdef CONFIG_LOG_BUF
#define __LOG_BUF_LEN   (1 << CONFIG_LOG_BUF_SHIFT)

static unsigned long log_end = 0;   /* Index into log_buf: most-recently-written-char + 1 */

static char __log_buf[__LOG_BUF_LEN];
static char *log_buf = __log_buf;
static int log_buf_len = __LOG_BUF_LEN;

#define LOG_BUF_MASK    (log_buf_len - 1)
#define LOG_BUF(idx) (log_buf[(idx) & LOG_BUF_MASK])

static void emit_log_char(char c)
{
	LOG_BUF(log_end) = c;
	log_end ++;
}

static void emit_log(char *print_buf)
{
	char *p;

	for (p = print_buf; *p; p++) {
		emit_log_char(*p);
	}	
}
#endif /* CONFIG_LOG_BUF */

uart_t uart;

static int dwapbuart_init(uart_t *pUart);
static void dwapbuart_putc(char ch);
static int dwapbuart_getc(void);
static int dwapbuart_tstc(uart_t *pUart);
static void dwapbuart_setbrg(uart_t *pUart);

static void serial_write_reg(uint32_t reg, uint32_t val)
{
	writel(val, ATXX_UART0_BASE + reg);
}

static uint32_t serial_read_reg(uint32_t reg)
{
	return readl(ATXX_UART0_BASE + reg);
}

int serial_init(void)
{
	uart_t *pUart = &uart;

	pUart->clkfreq = CFG_UART_CLOCK_FREQ;
	pUart->baudrate = CONFIG_BAUDRATE;
#ifdef CFG_UART_FIFO_ON
	pUart->fifo_cfg = bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF | bUART_FCR_FIFOE;
#else
	pUart->fifo_cfg = bUART_FCR_RCVR_2LESS | bUART_FCR_TET_HALF;
#endif
	pUart->loop_enable = CFG_UART_LOOPENABLE;

#ifdef CONFIG_UART_XMODEM
	pUart->xm_running = 0;
#endif

	return dwapbuart_init(pUart);
}

void serial_putc(const char ch)
{
	uart_t *pUart = &uart;

#ifdef CONFIG_LOG_BUF
	emit_log_char (ch);
#endif

#ifdef CONFIG_UART_XMODEM
	if (pUart->xm_running) {
		return;
	}
#endif

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
	uart_t *pUart = &uart;

#ifdef CONFIG_UART_XMODEM
        if (pUart->xm_running) {
                return -1;
        }
#endif

	return dwapbuart_getc();
}

int serial_tstc(void)
{
	uart_t *pUart = &uart;

#ifdef CONFIG_UART_XMODEM
        if (pUart->xm_running) {
                return 0;
        }
#endif

	return dwapbuart_tstc(pUart);
}

void serial_setbrg(void)
{
	uart_t *pUart = &uart;
 	dwapbuart_setbrg(pUart);
}

unsigned long uart_get_divisor(unsigned long clkfreq, unsigned long baudrate)
{
	return ((clkfreq/baudrate) >> 4);
}

static void uart_clear_rx_fifo()
{
	uint32_t val;

	val = serial_read_reg(UART_FCR_OFS);
	val |= bUART_FCR_RFIFOR;
	serial_write_reg(UART_FCR_OFS, val);
}

static void uart_clear_tx_fifo()
{

	uint32_t val;

	val = serial_read_reg(UART_FCR_OFS);
	val |= bUART_FCR_XFIFOR;
	serial_write_reg(UART_FCR_OFS, val);
}

static int dwapbuart_init(uart_t *pUart)
{
	unsigned long val;

	/* FIFO control */
	serial_write_reg(UART_FCR_OFS, pUart->fifo_cfg);
	/* enable DLL/DLH R/W */
	serial_write_reg(UART_LCR_OFS, bUART_LCR_DLAB);
	/* CFG LOW */
	val = uart_get_divisor(pUart->clkfreq, pUart->baudrate);
	serial_write_reg(UART_DLL_OFS, (val & 0xFF));
	/* CFG HIGH */
	serial_write_reg(UART_DLH_OFS, (val >> 8));

	/* 8bits, No parity */
	serial_write_reg(UART_LCR_OFS, bUART_LCR_DLS);

	/* enable loopback */
	if(pUart->loop_enable == 1)
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
	} while( (val & bUART_LSR_DR) == 0 );

	return serial_read_reg(UART_RBR_OFS);
}

static int dwapbuart_tstc(uart_t *pUart)
{
	uint32_t val;

	val = serial_read_reg(UART_LSR_OFS);
	return ((val & bUART_LSR_DR) != 0);
}

static void dwapbuart_setbrg(uart_t *pUart)
{
	/* not implemented yet */
}

#ifdef CONFIG_UART_XMODEM
uint8_t dwapbuart_get_rbr(uart_t *pUart)
{
	uint32_t val;
	
	val = serial_read_reg(UART_RBR_OFS);
	val &= 0xff;
	return (uint8_t)val;
}

int32_t dwapbuart_getsts(uart_t *pUart, uint8_t *pch)
{
	*pch = 0;

	if(!dwapbuart_tstc(pUart)) {
		return -1;
	}
	else {
		*pch = dwapbuart_getc();
		return 0;
	}
}

int32_t dwapbuart_getc_timeout(uart_t *pUart, uint32_t timeout, uint8_t *pch)
{
	uint32_t i, mytimeout;

	mytimeout = ((uint32_t)timeout)<<8;
	if(mytimeout == 0) {
		mytimeout = 1;
	}

	for(i = 0; i < mytimeout; i++) {
		if( dwapbuart_getsts(pUart, pch) == 0) {
			return 0;
		}
	}

	return -1;
}

uint8_t dlch;

/* Table of CRC constants - implements x^16+x^12+x^5+1 */
static const uint16_t crc16[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

int32_t xm_getblk(uart_t *pUart, uint8_t *buf, uint8_t *block)
{
	uint8_t ch, blk, cblk, crc1, crc2, *ptr;
	int32_t ret, i, len;
	int16_t hdr_found = 0;
	uint16_t cksum;

	len = 128;

	/*  Find the start of a header */
	while (!hdr_found) {
		if( dlch == xm_POLL_C ) {
			dwapbuart_putc(dlch);
			ret = dwapbuart_getc_timeout( pUart, xyzModem_CHAR_TIMEOUT_C, &ch );
		}
		else {
			ret = (int32_t)dwapbuart_getc_timeout( pUart, xyzModem_CHAR_TIMEOUT, &ch );
		}

		if (!ret) {
			switch (ch) {
			case xm_SOH_128:
			case xm_SOH_1024:
				hdr_found = 1;
				break;
			case xm_CAN:
				return (int32_t)XMODEM_CANCEL;
			case xm_EOT:
				return (int32_t)XMODEM_EOF;
			default:
				break;
			}
		}
		else {
			if( dlch != xm_POLL_C ) {
				ch = dwapbuart_get_rbr(pUart);
				if( ch == xm_EOT) {
					return (int32_t)XMODEM_EOF;
				}
			}
		}
	}

	len = (ch == xm_SOH_128) ? 128 : 1024;

	if(dwapbuart_getc_timeout(pUart, xyzModem_CHAR_TIMEOUT, &blk)) {
		return (int32_t)XMODEM_TIMEOUT;
	}

	if(dwapbuart_getc_timeout(pUart, xyzModem_CHAR_TIMEOUT, &cblk)) {
		return (int32_t)XMODEM_TIMEOUT;
	}

	cksum = 0;
	for (i = 0, ptr = buf;  i < len;  i++, ptr++) {
		if (dwapbuart_getc_timeout(pUart, xyzModem_CHAR_TIMEOUT, ptr)) {
			return (int32_t)XMODEM_TIMEOUT;
		}
		cksum = crc16[((cksum>>8) ^ (*ptr)) & 0xFF] ^ (cksum << 8);
	}

	if (dwapbuart_getc_timeout(pUart, xyzModem_CHAR_TIMEOUT, &crc1)) {
		return (int32_t)XMODEM_TIMEOUT;
	}

	if (dwapbuart_getc_timeout(pUart, 1000, &crc2)) {
		/* crc2 = uart_read_rx_reg(pUart); */
		return (int32_t)XMODEM_TIMEOUT;
	}

	if ((blk ^ cblk) != (unsigned char)0xFF) {
		return (int32_t)XMODEM_CRC;
	}

	if (cksum != ((crc1 << 8) | crc2)) {
		return (int32_t)XMODEM_CRC;
	}

	*block  = blk;

	return len;
}

int32_t xm_read(uart_t *pUart, uint8_t *buf, uint32_t *length)
{
	uint8_t blk, next_blk;
	int32_t ret, total;
	int16_t retries;

	*length = 0;
	ret = 0;
	next_blk = 1;
	total = 0;
	retries = XMODEM_STARTUP_RETRIES;
	/* uart_putc(pUart, xm_POLL_C); */

	while (ret != XMODEM_EOF) {
		ret = xm_getblk(pUart, buf, &blk);
		if (ret > 0) {
			*length += ret;
			if( (blk == 0) || (blk == next_blk) ) {
				next_blk = blk+1;
				total += ret;
				buf += ret;
				dwapbuart_putc(xm_ACK);
				dlch = xm_ACK;
				retries = XMODEM_RETRIES;
				continue;
			} 
			else {
				dwapbuart_putc(xm_NACK);
				dlch = xm_NACK;
			}
		}
		else if (ret == XMODEM_CANCEL) {
			dwapbuart_putc(xm_NACK);
			dlch = xm_NACK;
			return ret;
		}
		else if(ret == XMODEM_EOF) {
			dwapbuart_putc(xm_ACK);
			dlch = xm_ACK;
		}
		else if(ret == XMODEM_CRC) {
			dwapbuart_putc(xm_NACK);
			dlch = xm_NACK;
			continue;
		}
		else {
			;
		}

		if( (retries--)==0 ) {
                        /* dwapbuart_putc(pUart, xm_POLL_C); */
			return (int32_t)XMODEM_TIMEOUT;
		}
	}

	return total;
}

int32_t xm_load(uint8_t *buf, uint32_t *length)
{
	uart_t *pUart = &uart;
	int32_t ret;

	pUart->xm_running = 1;

        dlch = xm_POLL_C;
        ret = xm_read(pUart, buf, length);

	/* delay 1 sec to let the remote host recover */
	mdelay(1000);

	pUart->xm_running = 0;

	return ret;
}
#endif /* CONFIG_UART_XMODEM */
#endif /* CFG_DWAPBUART_SERIAL */


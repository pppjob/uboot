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

#ifndef __SERIAL_DWAPBUART_H
#define __SERIAL_DWAPBUART_H

#define  UART_RBR_OFS			0x00		/* Receive Buffer Register */
#define  UART_THR_OFS			0x00		/* Transmit Holding Register */
#define  UART_DLL_OFS			0x00		/*Divisor Latch Low */
#define  UART_DLH_OFS			0x04		/*Divisor Latch High */
#define  UART_IER_OFS			0x04		/*Interrupt Enable Register */
#define  UART_IIR_OFS			0x08		/*Interrupt Identification Register */
#define  UART_FCR_OFS			0x08		/*FIFO Control Register */
#define  UART_LCR_OFS			0x0C		/*Line Control Register */
#define  UART_MCR_OFS			0x10		/*Modem Control Register */
#define  UART_LSR_OFS			0x14		/*Line Status Register */
#define  UART_MSR_OFS			0x18		/*Modem Status Register */
#define  UART_SCR_OFS			0x1C		/*Scratchpad Register */
#define  UART_FAR_OFS			0x70		/*FIFO Access Register */
#define  UART_TFR_OFS			0x74		/*Transmit FIFO Read */
#define  UART_RFW_OFS			0x78		/*Receive FIFO Write */
#define  UART_USR_OFS			0x7C		/*Uart Status Register */
#define  UART_TFL_OFS			0x80		/*Transmit FIFO Level */
#define  UART_RFL_OFS			0x84		/*Receive FIFO Level */
#define  UART_SRR_OFS			0x88		/*Software Reset Register */
#define  UART_SRTS_OFS			0x8C		/*Shadow Request to Send */
#define  UART_SBCR_OFS			0x90		/*Shadow Break Control Register */
#define  UART_SDMAM_OFS			0x94		/*Shadow DMA Mode */
#define  UART_SFE_OFS			0x98		/*Shadow FIFO Enable */
#define  UART_SRT_OFS			0x9C		/*Shadow RCVR Trigger */
#define  UART_TET_OFS			0xA0		/*Shadow TX Empty Trigger */
#define  UART_HTX_OFS			0xA4		/*Halt TX */
#define  UART_DMASA_OFS			0xA8		/*DMA Software Acknowledge */
#define  UART_CPR_OFS			0xF4		/*Component Parameter Register */
#define  UART_UCV_OFS			0xF8		/*Uart Component Version */
#define  UART_CTR_OFS			0xFC		/*Component Type Register */

#define bit_0   0x01
#define bit_1   0x02
#define bit_2   0x04
#define bit_3   0x08
#define bit_4   0x10
#define bit_5   0x20
#define bit_6   0x40
#define bit_7   0x80

#define bUART_IER_PTIME		bit_7
#define bUART_IER_EDSSI		bit_3
#define bUART_IER_ELSI		bit_2
#define bUART_IER_ETBEI		bit_1
#define bUART_IER_ETBFI		bit_0

#define bUART_IIR_FIFOSE	(bit_7 | bit_6)
#define bUART_IIR_IID		(bit_3 | bit_2 | bit_1 | bit_0)

#define bUART_FCR_RCVR		(bit_7 | bit_6)
#define bUART_FCR_RCVR_2LESS	(bit_7 | bit_6)
#define bUART_FCR_TET		(bit_5 | bit_4)
#define bUART_FCR_TET_HALF	(bit_5 | bit_4)
#define bUART_FCR_TET_ONE	(bit_4)
#define bUART_FCR_TET_ZERO	0
#define bUART_FCR_DMAM		bit_3
#define bUART_FCR_XFIFOR	bit_2
#define bUART_FCR_RFIFOR	bit_1
#define bUART_FCR_FIFOE		bit_0

#define bUART_LCR_DLAB		bit_7
#define bUART_LCR_BC		bit_6
#define bUART_LCR_RAZ		bit_5
#define bUART_LCR_EPS		bit_4
#define bUART_LCR_PEN		bit_3
#define bUART_LCR_STOP		bit_2
#define bUART_LCR_DLS		(bit_1 | bit_0)

#define bUART_MCR_SIRE		bit_6
#define bUART_MCR_AFCE		bit_5
#define bUART_MCR_LB		bit_4
#define bUART_MCR_OUT2		bit_3
#define bUART_MCR_OUT1		bit_2
#define bUART_MCR_RTS		bit_1
#define bUART_MCR_DTR		bit_0

#define bUART_LSR_RFE		bit_7
#define bUART_LSR_TEMT		bit_6
#define bUART_LSR_THRE		bit_5
#define bUART_LSR_BI		bit_4
#define bUART_LSR_FE		bit_3
#define bUART_LSR_PE		bit_2
#define bUART_LSR_OE		bit_1
#define bUART_LSR_DR		bit_0

#define bUART_MSR_DCD		bit_7
#define bUART_MSR_RI		bit_6
#define bUART_MSR_DSR		bit_5
#define bUART_MSR_CTS		bit_4
#define bUART_MSR_DDCD		bit_3
#define bUART_MSR_TERI		bit_2
#define bUART_MSR_DDSR		bit_1
#define bUART_MSR_DCTS		bit_0

#define bUART_USR_RFF		bit_4
#define bUART_USR_RFNE		bit_3
#define bUART_USR_TFE		bit_2
#define bUART_USR_TFNF		bit_1
#define bUART_USR_BUSY		bit_0

typedef struct
{
	unsigned long	clkfreq;
	unsigned long	baudrate;
	unsigned long	fifo_cfg;
	unsigned long	loop_enable;
#ifdef CONFIG_UART_XMODEM
	unsigned long	xm_running;
#endif
} uart_t;

#ifdef CONFIG_UART_XMODEM
#define xm_FF			0xFF
#define xm_SOH_128		0x01
#define xm_SOH_1024		0x02
#define xm_EOT			0x04
#define xm_ACK			0x06
#define xm_NACK			0x15
#define xm_CAN			0x18
#define xm_POLL_C		0x43
#define xm_PAD_Data		0x1A

#define xyzModem_CHAR_TIMEOUT_C			2000
#define xyzModem_CHAR_TIMEOUT			2000
#define xyzModem_MAX_RETRIES			10

#define XMODEM_STARTUP_RETRIES			800
#define XMODEM_RETRIES				10

#define XMODEM_TIMEOUT				-1
#define XMODEM_EOF				-2
#define XMODEM_CANCEL				-3
#define XMODEM_CRC				-4
#endif /* CONFIG_UART_XMODEM */

#endif /* __SERIAL_DWAPBUART_H */


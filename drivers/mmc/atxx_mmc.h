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
------------------------------------------------------------------------------*/

#ifndef _ATXX_MMC_H_
#define _ATXX_MMC_H_

#include <common.h>

/* command index */
#define CMD(index) 		index

#ifdef	DEBUG_ATXX_MMC
#define ATXX_MMC_DEBUG(x...)  printf(x)
#else
#define ATXX_MMC_DEBUG(x...) 
#endif

/* command index */
enum sd_card_command
{
	CMD0,
	CMD1,
	CMD2,
	CMD3,
	CMD4,
	CMD5,
	CMD6,
	CMD7,
	CMD8,
	CMD9,
	CMD10,
	CMD11,
	CMD12,
	CMD13,
	CMD14,
	CMD15,
	CMD16,
	CMD17,
	CMD18,
	CMD19,
	CMD20,
	CMD21,
	CMD22,
	CMD23,
	CMD24,
	CMD25,
	CMD26,
	CMD27,
	CMD28,
	CMD29,
	CMD30,
	CMD31,
	CMD32,
	CMD33,
	CMD34,
	CMD35,
	CMD36,
	CMD37,
	CMD38,
	CMD39,
	CMD40,
	CMD41,
	CMD42,
	CMD43,
	CMD44,
	CMD45,
	CMD46,
	CMD47,
	CMD48,
	CMD49,
	CMD50,
	CMD51,
	CMD52,
	CMD53,
	CMD54,
	CMD55,
	CMD56,
	CMD57,
	CMD58,
	CMD59,
	CMD60,
	CMD61,
	CMD62,
	CMD63,
	CMD_COUNT,
};

enum sd_card_application_command
{
	ACMD0,
	ACMD1,
	ACMD2,
	ACMD3,
	ACMD4,
	ACMD5,
	ACMD6,
	ACMD7,
	ACMD8,
	ACMD9,
	ACMD10,
	ACMD11,
	ACMD12,
	ACMD13,
	ACMD14,
	ACMD15,
	ACMD16,
	ACMD17,
	ACMD18,
	ACMD19,
	ACMD20,
	ACMD21,
	ACMD22,
	ACMD23,
	ACMD24,
	ACMD25,
	ACMD26,
	ACMD27,
	ACMD28,
	ACMD29,
	ACMD30,
	ACMD31,
	ACMD32,
	ACMD33,
	ACMD34,
	ACMD35,
	ACMD36,
	ACMD37,
	ACMD38,
	ACMD39,
	ACMD40,
	ACMD41,
	ACMD42,
	ACMD43,
	ACMD44,
	ACMD45,
	ACMD46,
	ACMD47,
	ACMD48,
	ACMD49,
	ACMD50,
	ACMD51,
	ACMD52,
	ACMD53,
	ACMD54,
	ACMD55,
	ACMD56,
	ACMD57,
	ACMD58,
	ACMD59,
	ACMD60,
	ACMD61,
	ACMD62,
	ACMD63,
	ACMD_COUNT = 64,
};

#define CNULL	0xff		/*command not support*/
#define ACNULL	0xff		/*application command not support*/

/* define command's response */
#define COMMAND_R_NONE	   		0
#define COMMAND_R1 			1
#define COMMAND_R1B 			2
#define COMMAND_R2 			3
#define COMMAND_R3 			4
#define COMMAND_R4 			5
#define COMMAND_R5 			6
#define COMMAND_R6 			7
#define COMMAND_R7 			8	/*added for SDHC,2007-12-18*/

typedef struct
{
	uint8_t     	 cmd_index;
	uint8_t      	 arg;
	uint8_t		 respone_type;	 
	uint8_t		 send_init;
	uint8_t		 stop_abort;
	uint8_t		 data_expected;
	uint8_t		 read_write;
	uint8_t		 transfer_mode;
}command_set_struct;

#define COMMAND_BASE			0	/*base command*/
#define COMMAND_APPLICATION		1	/*application command*/

/* register address offset */
#define STORAGE_CTRL 		    	0x00
#define STORAGE_PWREN 		    	0x04
#define STORAGE_CLKDIV 		    	0x08
#define STORAGE_CLKSRC 		    	0x0C
#define STORAGE_CLKENA 		    	0x10
#define STORAGE_TMOUT 		    	0x14
#define STORAGE_CTYPE 		    	0x18
#define STORAGE_BLKSIZ 		    	0x1C
#define STORAGE_BYTCNT 		    	0x20
#define STORAGE_INTMASK 	    	0x24
#define STORAGE_CMDARG 		    	0x28
#define STORAGE_CMD 		    	0x2C
#define STORAGE_RESP0 		    	0x30
#define STORAGE_RESP1 		    	0x34
#define STORAGE_RESP2 		    	0x38
#define STORAGE_RESP3 		    	0x3C
#define STORAGE_MINTSTS 	    	0x40
#define STORAGE_RINTSTS 	    	0x44
#define STORAGE_STATUS 		    	0x48
#define STORAGE_FIFOTH 		    	0x4C
#define STORAGE_CDETECT 	    	0x50
#define STORAGE_WRTPRT 		    	0x54
#define STORAGE_GPIO 		    	0x58
#define STORAGE_TCBCNT 		    	0x5C
#define STORAGE_TBBCNT 		    	0x60
#define STORAGE_DEBNCE 		    	0x64
#define STORAGE_USRID 		    	0x68
#define STORAGE_VERID 		    	0x6C
#define STORAGE_HCON 		    	0x70

/* read/write data fifo */
#define STORAGE_FIFO_ADDR 		0x100
#define STORAGE_FIFO_DEPTH 		16
#define STORAGE_BLOCK_SIZE 		512

/* control register */
#define bSTORAGE_CTRL_USE_DMA           (1<<25)/*bit_25*/
#define bSTORAGE_CTRL_ENABLE_OD_PULLUP  (1<<24)/*bit_24*/
#define bSTORAGE_CTRL_ENABLE_DMA	(1<<5)/*bit_5*/
#define bSTORAGE_CTRL_ENABLE_INTERRUPT  (1<<4)/*bit_4*/
#define bSTORAGE_CTRL_DMA_RESET 	(1<<2)/*bit_2*/
#define bSTORAGE_CTRL_FIFO_RESET	(1<<1)/*bit_1*/
#define bSTORAGE_CTRL_COTROLLER_RESET 	(1<<0)/*bit_0*/

/* interrupt register */
#define bSTORAGE_INTSTS_EBE			(1<<15)/*bit_15	 End Bit Error (read)/Write no CRC */
#define bSTORAGE_INTSTS_ACD			(1<<14)/*bit_14	 Auto Command Done */
#define bSTORAGE_INTSTS_SBE			(1<<13)/*bit_13	 Start Bit Error */
#define bSTORAGE_INTSTS_HLE			(1<<12)/*bit_12	 Hardware Locked write Error */
#define bSTORAGE_INTSTS_FRUN			(1<<11)/*bit_11	 FIFO Underrun/Overrun Error */
#define bSTORAGE_INTSTS_HTO			(1<<10)/*bit_10	 Data Starvation by Host Timeout */
#define bSTORAGE_INTSTS_DRTO			(1<<9)/*bit_9	 Data Read Timeout */
#define bSTORAGE_INTSTS_RTO			(1<<8)/*bit_8	 Response Timeout */
#define bSTORAGE_INTSTS_DCRC			(1<<7)/*bit_7	 Data CRC Error */
#define bSTORAGE_INTSTS_RCRC			(1<<6)/*bit_6	 Response CRC Error */
#define bSTORAGE_INTSTS_RXDR			(1<<5)/*bit_5	 Receive FIFO Data Request */
#define bSTORAGE_INTSTS_TXDR			(1<<4)/*bit_4	 Transmit FIFO Data Request */
#define bSTORAGE_INTSTS_DTO			(1<<3)/*bit_3	 Data Tranfer Over */
#define bSTORAGE_INTSTS_CD			(1<<2)/*bit_2	 Command Done */
#define bSTORAGE_INTSTS_RE			(1<<1)/*bit_1	 Response Error */
#define bSTORAGE_INTSTS_CDT			(1<<0)/*bit_0	 Card Detection */
#define bSTORAGE_INTSTS_ALL			0xffff/* all interrupt source */

#define bSTORAGE_STATUS_MC_BUSY			(1<<10)/*bit_10	 Data state mc busy */
#define bSTORAGE_STATUS_DATA_BUSY		(1<<9)/*bit_9	 Data  busy */
#define bSTORAGE_STATUS_DATA3_STATUS		(1<<8)/*bit_8	 Data 3 status */
#define bSTORAGE_STATUS_FIFO_FULL		(1<<3)/*bit_3	 fifo full status */
#define bSTORAGE_STATUS_FIFO_EMPTY		(1<<2)/*bit_2	 fifo empty status */
#define bSTORAGE_STATUS_FIFO_TX_WATERMARK	(1<<1)/*bit_1	 fifo reached transmit watermark */
#define bSTORAGE_STATUS_FIFO_RX_WATERMARK	(1<<0)/*bit_0	 fifo reached receive watermark */


/* command register */
#define bSTORAGE_CMD_START_CMD			(1<<31)/*bit_31*/
#define bSTORAGE_CMD_UPDATE_CLK_REGS_ONLY	(1<<21)/*bit_21*/
#define	bSTORAGE_CMD_SEND_INITIALIZATION	(1<<15)/*bit_15*/
#define	bSTORAGE_CMD_STOP_ABORT			(1<<14)/*bit_14*/
#define	bSTORAGE_CMD_WAIT_PRVDATA_COMPLETE	(1<<13)/*bit_13*/
#define bSTORAGE_CMD_SEND_AUTO_STOP		(1<<12)/*bit_12*/
#define bSTORAGE_CMD_TRANSFER_MODE		(1<<11)/*bit_11*/
#define bSTORAGE_CMD_READ_WRITE			(1<<10)/*bit_10*/
#define bSTORAGE_CMD_DATA_EXPECTED		(1<<9)/*bit_9*/
#define	bSTORAGE_CMD_CHECK_RESPONSE_CRC		(1<<8)/*bit_8*/
#define	bSTORAGE_CMD_RESPONSE_LENGTH		(1<<7)/*bit_7*/
#define	bSTORAGE_CMD_RESPONSE_EXPECT		(1<<6)/*bit_6*/

#define STORAGE_MAX_CARD_NUMBER			16

enum storage_command_return
{
	STORAGE_COMMAND_APP_ERR = -14,		/* send CMD55 error */
	STORAGE_COMMAND_LENGTH_ERR = -13,	/* length is not 512 when write/read to/from card */
	STORAGE_COMMAND_EBE = -12, 		/* End Bit Error (read)/Write no CRC */
	STORAGE_COMMAND_SBE = -11,		/* Start Bit Error */
	STORAGE_COMMAND_HLE = -10,		/* Hardware Locked write Error */
	STORAGE_COMMAND_FRUN = -9,		/* FIFO Underrun/Overrun Error */
	STORAGE_COMMAND_HTO = -8,		/* Data Starvation by Host Timeout */
	STORAGE_COMMAND_DRTO = -7,		/* Data Read Timeout */
	STORAGE_COMMAND_RTO = -6,		/* Response Timeout */
	STORAGE_COMMAND_DCRC = -5,		/* Data CRC Error */
	STORAGE_COMMAND_RCRC = -4,		/* Response CRC Error */
	STORAGE_COMMAND_RXDR = -3,		/* Receive FIFO Data Request */
	STORAGE_COMMAND_TXDR = -2,		/* Transmit FIFO Data Request */
	STORAGE_COMMAND_RE = -1,		/* Response Error */
	STORAGE_COMMAND_DTO = 0,		/* Data Tranfer Over */
	STORAGE_COMMAND_ACD	= 0,		/* Auto Command Done ok*/
	STORAGE_COMMAND_CD = 0,			/* Command Done OK*/
};

#define bSTORAGE_INTSTS_EBE			(1<<15)/*bit_15	 End Bit Error (read)/Write no CRC */
#define bSTORAGE_INTSTS_ACD			(1<<14)/*bit_14	 Auto Command Done */
#define bSTORAGE_INTSTS_SBE			(1<<13)/*bit_13	 Start Bit Error */
#define bSTORAGE_INTSTS_HLE			(1<<12)/*bit_12	 Hardware Locked write Error */
#define bSTORAGE_INTSTS_FRUN			(1<<11)/*bit_11	 FIFO Underrun/Overrun Error */
#define bSTORAGE_INTSTS_HTO			(1<<10)/*bit_10	 Data Starvation by Host Timeout */
#define bSTORAGE_INTSTS_DRTO			(1<<9)/*bit_9	 Data Read Timeout */
#define bSTORAGE_INTSTS_RTO			(1<<8)/*bit_8	 Response Timeout */
#define bSTORAGE_INTSTS_DCRC			(1<<7)/*bit_7	 Data CRC Error */
#define bSTORAGE_INTSTS_RCRC			(1<<6)/*bit_6	 Response CRC Error */
#define bSTORAGE_INTSTS_RXDR			(1<<5)/*bit_5	 Receive FIFO Data Request */
#define bSTORAGE_INTSTS_TXDR			(1<<4)/*bit_4	 Transmit FIFO Data Request */
#define bSTORAGE_INTSTS_DTO			(1<<3)/*bit_3	 Data Tranfer Over */
#define bSTORAGE_INTSTS_CD			(1<<2)/*bit_2	 Command Done */
#define bSTORAGE_INTSTS_RE			(1<<1)/*bit_1	 Response Error */

enum storage_card_type
{
	CARD_TYPE_UNKNOW = 0,
	CARD_TYPE_MMC,
	CARD_TYPE_SDMEM,
	CARD_TYPE_SDIO,
	CARD_TYPE_COMBO,
	CARD_TYPE_CEATA,
	CARD_TYPE_ATA,
};

#define CARD_STATUS_APP_CMD			(1<<5)/*bit_5*/

#define KHZ                     		(1000)
#define MHZ                     		(1000 * 1000)
#define STORAGE_CLOCK_IN        		(25 * MHZ)/*25MHZ*/

#define storage_set_card_number(cmd,num)	((cmd) = ((cmd) | (num << 16)))
#define storage_get_card_number(cmd)		(((cmd) & (0x1f << 16)) >> 16)
#define storage_set_cmd_index(cmd,index)	((cmd) = ((cmd) | (index)))
#define storage_set_cmd_bit(cmd,bit)			((cmd) = ((cmd) | (bit)))
#define storage_clear_cmd_bit(cmd,bit)		((cmd) = ((cmd) & ~(bit)))

#define storage_get_data(reg) 			(reg)
#define storage_set_data(reg,data) 		((reg) = (data))
#define storage_set_bit(reg,bit) 		storage_set_data((reg),(storage_get_data(reg) | (bit)))

#define storage_power_enable(num) 		((uint32_t)((1 << num) - 1))

#define SD_BUF_SIZE  2048

void sd_controller_init(void);
void sd_read_card_type(int card);
int sd_set_clock(int card,uint32_t clock);
int sd_init_card(uint32_t card);
void sd_read_card(uint32_t card, uint32_t addr);
void sd_write_card(uint32_t card, uint32_t addr);
void sd_erase_card(uint32_t card, uint32_t addr);
uint32_t sd_base_command(int card,int index,uint32_t arg,uint32_t *resp);
uint32_t sd_application_command(int card,int index,uint32_t arg);
void sd_set_buswidth(uint32_t card,int width);
void sd_select_card(uint32_t card);
void sd_get_status(uint32_t card);
void sd_card_power_on(uint32_t card,uint32_t flag);

void sd_read_fifo(uint64_t *buf,uint32_t *len);
void sd_write_fifo(uint64_t *data,uint32_t size,uint32_t *len);
void sd_set_blocksize(int card,uint32_t size);
void sd_card_detect(void);

extern void sd_multiblock_read(int card, uint32_t addr, uint32_t blk_cnts, uint8_t *pdata); /*test for jone to read linux image*/
extern void sd_multiblock_write(int card, uint32_t addr, uint32_t blk_cnts, uint8_t *pdata); /*test for jone to write linux image*/

void sd_multiblock_erase(int card, uint32_t addr, uint32_t blk_cnts);
void sd_get_scr_register(void);
void sd_get_status_register(void);
extern void sd_get_card_info(void);

/* function type */
int storage_send_base_command(uint32_t command,uint32_t arg,uint32_t *resp);

extern int mmc_legacy_init(int verbose);
#endif



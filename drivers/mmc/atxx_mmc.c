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
 -----------------------------------------------------------------------------*/

#include <fat.h>
#include <part.h>
#include <asm/io.h>
#include <asm/arch-atxx/pmu.h>
#include "atxx_mmc.h"

/* SD/MMC/CE-ATA Controller - #0 0x3FFDE000 , for SD/MMC*/
/* SD/MMC/CE-ATA Controller - #1 0X3FD78000 , for SDIO*/
/* SD/MMC/CE-ATA Controller - #2 0X3FFDE800 , for CE-ATA*/
uint32_t sd_controller_base_addr = 0x3FFDE000;
static block_dev_desc_t mmc_blk_dev = {0}; 

uint32_t sd_rca = 0;
uint32_t sd_ocr = 0;
uint32_t sd_cid[4];
uint32_t sd_csd[4];
uint8_t sd_scr[8];
uint8_t sd_status[64];

uint8_t buf[SD_BUF_SIZE];
uint32_t address;
uint32_t cd_detect = 0x0000ffff;
uint32_t cd_wrprotect = 0x0000000;

static const char sd_state[16][12] =
{
	"idle", 
	"ready",
	"ident",
	"stby",
	"tran",
	"data",
	"rcv",
	"prg",
	"dis",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
};

command_set_struct command_set[64] = 
{
	{CMD0,	0,	COMMAND_R_NONE,	1,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD2,	0,	COMMAND_R2,	0,	0,	0,	0,	0},		/*checked*/
	{CMD3,	0,	COMMAND_R6,	0,	0,	0,	0,	0},		/*checked*/
	{CMD4,	0,	COMMAND_R_NONE,	0,	0,	0,	0,	0},
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD6,	0,	COMMAND_R1,	0,	0,	0,	0,	0},
	{CMD7,	0,	COMMAND_R1,	0,	0,	0,	0,	0},
	{CMD8,	0,	COMMAND_R7,	0,	0,	0,	0,	0},		/*checked,added for SDHC,2007-12-18*/
	{CMD9,	0,	COMMAND_R2, 	0,	0,	0,	0,	0},
	{CMD10,	0,	COMMAND_R2, 	0,	0,	0,	0,	0},	
	{CMD11,	0,	COMMAND_R1,	0,	0,	0,	0,	0},
	{CMD12,	0,	COMMAND_R1B,	0,	0,	0,	0,	0},
	{CMD13,	0,	COMMAND_R1,	0,	0,	0,	0,	0},
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD15,	0,	COMMAND_R_NONE,	0,	0,	0,	0,	0},
	{CMD16,	0,	COMMAND_R1,	0,	0,	0,	0,	0},
	{CMD17,	0,	COMMAND_R1,	0,	0,	1,	0,	0},		/*checked*/
	{CMD18,	0,	COMMAND_R1, 	0,	0,	1,	0,	0},
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD24,	0,	COMMAND_R1,	0,	0,	1,	1,	0},		/*checked*/
	{CMD25,	0,	COMMAND_R1,	0,	0,	1,	1,	0},
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD27,	0,	COMMAND_R1,	0,	0,	0,	0,	0},
	{CMD28,	0,	COMMAND_R1B,	0,	0,	0,	0,	0},
	{CMD29,	0,	COMMAND_R1B,	0,	0,	0,	0,	0},
	{CMD30,	0,	COMMAND_R1, 	0,	0,	0,	0,	0},	
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD32,	0,	COMMAND_R1,	0,	0,	0,	0,	0},		/*checked*/
	{CMD33,	0,	COMMAND_R1,	0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD38,	0,	COMMAND_R1B,	0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD42,	0,	COMMAND_R2,	0,	0,	0,	0,	0},
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CMD55,	0,	COMMAND_R1,	0,	0,	0,	0,	0},		/*checked*/
	{CMD56,	0,	COMMAND_R1,	0,	0,	0,	0,	0},
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
	{CNULL,	0,	0,		0,	0,	0,	0,	0},		/*checked*/
};

command_set_struct command_application_set[64] = 
{
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACMD6,		0,	COMMAND_R1,			0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACMD13,	0,	COMMAND_R1,			0,	0,	1,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACMD22,	0,	COMMAND_R1,			0,	0,	0,	0,	0},
	{ACMD23,	0,	COMMAND_R1,			0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACMD41,	0,	COMMAND_R3,			0,	0,	0,	0,	0},	/*checked*/
	{ACMD42,	0,	COMMAND_R1,			0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACMD51,	0,	COMMAND_R1,			0,	0,	1,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
	{ACNULL,	0,	0,				0,	0,	0,	0,	0},
};

static  uint32_t atxx_sd_read_reg(uint32_t reg)
{
	return readl(sd_controller_base_addr + reg);
}

static  void atxx_sd_write_reg(uint32_t reg, uint32_t val)
{
	writel(val, sd_controller_base_addr + reg);
}

static void storage_clear_bit(uint32_t reg,uint32_t bit) 
{
	atxx_sd_write_reg(reg, (atxx_sd_read_reg(reg)& (~(bit))));
}

static void storage_set_datatimeout(uint32_t data)
{
	atxx_sd_write_reg(STORAGE_TMOUT, ((atxx_sd_read_reg(STORAGE_TMOUT) & 0x000000ff) | (data << 8)));
}

static void  storage_set_resptimeout(uint32_t data)
{
	atxx_sd_write_reg(STORAGE_TMOUT, ((atxx_sd_read_reg(STORAGE_TMOUT) &  0xffffff00) | (data)));
}

static void  storage_set_txwmark(uint32_t data)
{
	atxx_sd_write_reg(STORAGE_FIFOTH, ((atxx_sd_read_reg(STORAGE_FIFOTH) &  0xffffff00) | (data)));
}

static void  storage_set_rxwmark(uint32_t data)
{
	atxx_sd_write_reg(STORAGE_FIFOTH, ((atxx_sd_read_reg(STORAGE_FIFOTH) & 0xf000ffff) | (data << 16)));
}

static void storage_set_debouncecount(uint32_t data)
{
	atxx_sd_write_reg(STORAGE_DEBNCE, ((atxx_sd_read_reg(STORAGE_DEBNCE) & 0x00ffffff) | (data)));
}

int storage_send_base_command(uint32_t command,uint32_t arg,uint32_t *resp)
{
	uint32_t status=0;
	int ret = STORAGE_COMMAND_CD;
	uint32_t timeout = 5000000;

	/*rSTORAGE_CMDARG = arg;*/
	/*rSTORAGE_CMD = command;*/
	atxx_sd_write_reg(STORAGE_CMDARG, arg);
	atxx_sd_write_reg(STORAGE_CMD, command);

	/* wait for completion of command*/
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & bSTORAGE_INTSTS_HLE) {
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			ret = STORAGE_COMMAND_HLE;
			return ret;
		}

		if(status & bSTORAGE_INTSTS_CD)
			break;

		atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);/*clear all interrupt status*/
		if(status & bSTORAGE_INTSTS_RE)
			return STORAGE_COMMAND_RE;

		if(status & bSTORAGE_INTSTS_RCRC)
			return STORAGE_COMMAND_RCRC;

		if(status & bSTORAGE_INTSTS_RTO)
			return STORAGE_COMMAND_RTO;

		udelay(1);
	}

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	if(command & bSTORAGE_CMD_RESPONSE_EXPECT) {
		resp[0] = atxx_sd_read_reg(STORAGE_RESP0);
		resp[1] = atxx_sd_read_reg(STORAGE_RESP1);
		resp[2] = atxx_sd_read_reg(STORAGE_RESP2);
		resp[3] = atxx_sd_read_reg(STORAGE_RESP3);
	}

	return ret;
}

uint32_t command_base_build(int card,int index,int flag)
{
	uint32_t command = 0x80000100;	/*start_cmd bit is 1*/
	command_set_struct stcmdset;

	if(index >= CMD_COUNT)
		return 0;

	storage_set_card_number(command,card);
	storage_set_cmd_index(command,index);

        /*application command*/
	if(flag) {
		stcmdset = command_application_set[index];
	} else {
		stcmdset = command_set[index];
	}

	if(stcmdset.send_init) {
		storage_set_cmd_bit(command,bSTORAGE_CMD_SEND_INITIALIZATION);
	}

	if(stcmdset.stop_abort)	{
		storage_set_cmd_bit(command,bSTORAGE_CMD_STOP_ABORT);
	}

	if(stcmdset.data_expected) {
		storage_set_cmd_bit(command,bSTORAGE_CMD_DATA_EXPECTED);
	}

	if(stcmdset.read_write)	{
		storage_set_cmd_bit(command,bSTORAGE_CMD_READ_WRITE);
	}	

	if(stcmdset.transfer_mode) {
		storage_set_cmd_bit(command,bSTORAGE_CMD_TRANSFER_MODE);
	}

	if(stcmdset.respone_type) {
		storage_set_cmd_bit(command,bSTORAGE_CMD_RESPONSE_EXPECT);
		if(stcmdset.respone_type == COMMAND_R2)
			storage_set_cmd_bit(command,bSTORAGE_CMD_RESPONSE_LENGTH);
	}

	return command;	
}

uint32_t sd_base_command(int card,int index,uint32_t arg,uint32_t *resp)
{
	uint32_t status,command,cmd,timeout;

	resp[0] = 0;
	resp[1] = 0;
	resp[2] = 0;
	resp[3] = 0;

	/*cmd = (index >> 4) * 10 + (index & 0xf);*/
	cmd = index;
	command = command_base_build(card,cmd,COMMAND_BASE);

	if((cmd == 7) || (cmd == 9)  || (cmd == 10) || (cmd == 13) || (cmd == 15)  || (cmd == 55)) {
                atxx_sd_write_reg(STORAGE_CMDARG, sd_rca);
	} else {
                atxx_sd_write_reg(STORAGE_CMDARG, arg);
        }
	atxx_sd_write_reg(STORAGE_CMD, command);

	/* wait for completion of command*/
	timeout=5000000;
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & bSTORAGE_INTSTS_HLE) {
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			break;
		}

		if(status & (bSTORAGE_INTSTS_RE | bSTORAGE_INTSTS_RTO)) {
			ATXX_MMC_DEBUG("status 0x%x\n", status);
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			return 1;
		}

		if(status & bSTORAGE_INTSTS_CD)
			break;

		udelay(1);
		/*rSTORAGE_RINTSTS = 0xffffffff;*/
	}

	ATXX_MMC_DEBUG("status 0x%x\n", status);

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	/*if(command & bSTORAGE_CMD_RESPONSE_EXPECT)*/
	{
		resp[0] = atxx_sd_read_reg(STORAGE_RESP0);
		resp[1] = atxx_sd_read_reg(STORAGE_RESP1);
		resp[2] = atxx_sd_read_reg(STORAGE_RESP2);
		resp[3] = atxx_sd_read_reg(STORAGE_RESP3);
	}

	ATXX_MMC_DEBUG("resp0 0x%x\n", resp[0]);
	ATXX_MMC_DEBUG("resp1 0x%x\n", resp[1]);
	ATXX_MMC_DEBUG("resp2 0x%x\n", resp[2]);
	ATXX_MMC_DEBUG("resp3 0x%x\n", resp[3]);

	if(index == 3)
		sd_rca = resp[0];

	return 0;
}


uint32_t sd_application_command(int card,int index,uint32_t arg)
{
	uint32_t status,command,resp,cmd,rs[4];
	uint32_t timeout = 5000000;

	resp = sd_base_command(0,CMD55,sd_rca,rs);
	if(resp) {
		ATXX_MMC_DEBUG("send command 55 error\n");
		return 0;
	}

	resp = 0;
	/*cmd = (index >> 4) * 10 + (index & 0xf);*/
	cmd = index;
	command = command_base_build(card,cmd,COMMAND_APPLICATION);

	/*now send application command*/
	if(cmd == 41) {
		atxx_sd_write_reg(STORAGE_CMDARG, arg);
		command &= (~0x100);
	} else if(cmd == 6)
        {
                atxx_sd_write_reg(STORAGE_CMDARG, arg);
        } else{
                atxx_sd_write_reg(STORAGE_CMDARG, 0);
        }

	atxx_sd_write_reg(STORAGE_CMD, command);

	/* wait for completion of command*/
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & bSTORAGE_INTSTS_HLE) {
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			break;
		}

		if(status & (bSTORAGE_INTSTS_RE | bSTORAGE_INTSTS_RTO)) {
			ATXX_MMC_DEBUG("app command err status 0 0x%x\n", status);
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			return resp;
		}

		if(status & bSTORAGE_INTSTS_CD)
			break;

		udelay(1);
	}

	ATXX_MMC_DEBUG("app command err status 0 0x%x\n", status);

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	resp = atxx_sd_read_reg(STORAGE_RESP0);

	ATXX_MMC_DEBUG("app resp 0x%x\n", resp);

	return resp;
}

void sd_card_power_on(uint32_t card,uint32_t flag)
{
	uint32_t detect;
	uint32_t reg;

	storage_clear_bit(STORAGE_CTRL,bSTORAGE_CTRL_ENABLE_INTERRUPT);

	/* clear any pending interrupts */
	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	detect = atxx_sd_read_reg(STORAGE_CDETECT);
	/*rSTORAGE_PWREN = 0xffff;*/
	if(flag) {	   /*rSTORAGE_PWREN |= (1 << card);*/
		reg =  atxx_sd_read_reg(STORAGE_PWREN)|(1 << card);
		atxx_sd_write_reg(STORAGE_PWREN, reg);
	} else {
		/*rSTORAGE_PWREN &= (~(1 << card));*/
		reg =  atxx_sd_read_reg(STORAGE_PWREN)&(~(1 << card));
		atxx_sd_write_reg(STORAGE_PWREN, reg);
	}

	ATXX_MMC_DEBUG("app command err status 0 0x%x\n", detect);

	return;
}

int sd_set_clock(int card,uint32_t clock)
{
	int clkdiv = 0,clksrc = 0;
	uint32_t reg;
	uint32_t command = 0,ret;
	uint32_t timeout = 10000;
	uint32_t timeouttop = 10000;

	clkdiv = (int)(STORAGE_CLOCK_IN / (2 * clock));

	/*rSTORAGE_CLKENA = 0x00000000;*/
	atxx_sd_write_reg(STORAGE_CLKENA, 0x00000000);
	storage_set_cmd_bit(command,bSTORAGE_CMD_START_CMD);
	storage_set_cmd_bit(command,bSTORAGE_CMD_UPDATE_CLK_REGS_ONLY);
	storage_set_cmd_bit(command,bSTORAGE_CMD_WAIT_PRVDATA_COMPLETE);
	ATXX_MMC_DEBUG("clock 0x%x\n", command);

	/*untill update clock register success, most timeout time is 100s*/
	while(timeouttop--) {
		atxx_sd_write_reg(STORAGE_CMD, command);
		timeout = 1000000;
		while(timeout--) {
			/*check if start_command bit of CMD is cleared*/
			reg = atxx_sd_read_reg(STORAGE_CMD);
			if((reg & bSTORAGE_CMD_START_CMD) == 0) {
				ATXX_MMC_DEBUG("clock cmd 0x%x\n", reg);
				ret = 1;
				break;
			}

			/*check if hle bit of RINTSTS is set*/
			reg = atxx_sd_read_reg(STORAGE_RINTSTS);
			if(reg & bSTORAGE_INTSTS_HLE) {
				ATXX_MMC_DEBUG("clock intst 0x%x\n", reg);
				ret = 0;
				break;
			}

			udelay(1);
		}

		if(ret)
			break;

		udelay(1);
	}

	/*rSTORAGE_CLKDIV = clkdiv;*/
	atxx_sd_write_reg(STORAGE_CLKDIV, clkdiv);

	storage_set_cmd_bit(command,bSTORAGE_CMD_START_CMD);
	storage_set_cmd_bit(command,bSTORAGE_CMD_UPDATE_CLK_REGS_ONLY);
	storage_set_cmd_bit(command,bSTORAGE_CMD_WAIT_PRVDATA_COMPLETE);
	ATXX_MMC_DEBUG("clock  0x%x\n", command);

	/*untill update clock register success*/
	timeouttop = 10000;
	while(timeouttop--) {
		atxx_sd_write_reg(STORAGE_CMD, command);
		timeout = 1000000;
		while(timeout--) {
			/*check if start_command bit of CMD is cleared*/
			reg = atxx_sd_read_reg(STORAGE_CMD);
			if((reg & bSTORAGE_CMD_START_CMD) == 0)	{
				ret = 1;
				break;
			}

			/*check if hle bit of RINTSTS is set*/
			reg = atxx_sd_read_reg(STORAGE_RINTSTS);
			if(reg & bSTORAGE_INTSTS_HLE) {
				ret = 0;
				break;
			}

			udelay(1);

		}

		if(ret)
			break;

		udelay(1);
	}

	/*rSTORAGE_CLKSRC = clksrc;*/
	atxx_sd_write_reg(STORAGE_CLKSRC, clksrc);
	storage_set_cmd_bit(command,bSTORAGE_CMD_START_CMD);
	storage_set_cmd_bit(command,bSTORAGE_CMD_UPDATE_CLK_REGS_ONLY);
	storage_set_cmd_bit(command,bSTORAGE_CMD_WAIT_PRVDATA_COMPLETE);

	/*untill update clock register success*/
	timeouttop = 10000;
	while(timeouttop--) {
		atxx_sd_write_reg(STORAGE_CMD, command);
		timeout = 1000000;
		while(timeout--) {
			/*check if start_command bit of CMD is cleared*/
			reg = atxx_sd_read_reg(STORAGE_CMD);
			if((reg & bSTORAGE_CMD_START_CMD) == 0)	{
				ret = 1;
				break;
			}

			/*check if hle bit of RINTSTS is set*/
			reg = atxx_sd_read_reg(STORAGE_RINTSTS);
			if(reg & bSTORAGE_INTSTS_HLE) {
				ret = 0;
				break;
			}

			udelay(1);

		}

		if(ret)
			break;

		udelay(1);
	}

	storage_set_cmd_bit(command,bSTORAGE_CMD_START_CMD);
	storage_set_cmd_bit(command,bSTORAGE_CMD_UPDATE_CLK_REGS_ONLY);
	storage_set_cmd_bit(command,bSTORAGE_CMD_WAIT_PRVDATA_COMPLETE);

	/*untill update clock register success*/
	timeouttop = 10000;
	while(timeouttop--) {
		atxx_sd_write_reg(STORAGE_CMD, command);
		timeout = 1000000;
		while(timeout--) {
			/*check if start_command bit of CMD is cleared*/
			reg = atxx_sd_read_reg(STORAGE_CMD);
			if((reg & bSTORAGE_CMD_START_CMD) == 0)	{
				ret = 1;
				break;
			}

			/*check if hle bit of RINTSTS is set*/
			reg = atxx_sd_read_reg(STORAGE_RINTSTS);
			if(reg & bSTORAGE_INTSTS_HLE) {
				ret = 0;
				break;
			}

			udelay(1);

		}
		if(ret)
			break;

		udelay(1);
	}

	/*rSTORAGE_CLKENA = 0x00000001;*/
	atxx_sd_write_reg(STORAGE_CLKENA, 0x00010001);
	storage_set_cmd_bit(command,bSTORAGE_CMD_START_CMD);
	storage_set_cmd_bit(command,bSTORAGE_CMD_UPDATE_CLK_REGS_ONLY);
	storage_set_cmd_bit(command,bSTORAGE_CMD_WAIT_PRVDATA_COMPLETE);

	/*untill update clock register success*/
	timeouttop = 10000;
	while(timeouttop--) {
		atxx_sd_write_reg(STORAGE_CMD, command);
		timeout = 1000000;
		while(timeout--) {
			/*check if start_command bit of CMD is cleared*/
			reg = atxx_sd_read_reg(STORAGE_CMD);
			if((reg & bSTORAGE_CMD_START_CMD) == 0)	{
				ret = 1;
				break;
			}

			/*check if hle bit of RINTSTS is set*/
			reg = atxx_sd_read_reg(STORAGE_RINTSTS);
			if(reg & bSTORAGE_INTSTS_HLE) {
				ret = 0;
				break;
			}

			udelay(1);

		}

		if(ret)
			break;

		udelay(1);
	}

	ATXX_MMC_DEBUG("left set clock():\n");

	return 1;
}

void sd_controller_init(void)
{
	uint32_t reg;
	uint32_t timeout = 1000000;

	reg = atxx_sd_read_reg(STORAGE_CTRL);
	reg |= 0x00000002;
	/*rSTORAGE_CTRL = reg;*/
	atxx_sd_write_reg(STORAGE_CTRL, reg);

	while(timeout--) {
		reg = atxx_sd_read_reg(STORAGE_CTRL);
		if(reg & 0x00000002){
                        ;
                } else {
                         break;
                }

		udelay(1);
	}

	reg = atxx_sd_read_reg(STORAGE_CTRL);
	reg |= 0x00000001;
	/*rSTORAGE_CTRL = reg;*/
	atxx_sd_write_reg(STORAGE_CTRL, reg);

	timeout = 1000000;
	while(timeout--) {
		reg = atxx_sd_read_reg(STORAGE_CTRL);
		if(reg & 0x00000001){
                        ;
                } else {
                         break;
                }

		udelay(1);
	}

	storage_clear_bit(STORAGE_CTRL,bSTORAGE_CTRL_ENABLE_INTERRUPT);

	/*change clock source assignment*/
	sd_set_clock(0, (400 * KHZ));

	storage_set_resptimeout(0x64);

	/* set data timeout */
	/*storage_set_datatimeout(0x100);*/
	storage_set_datatimeout(0xfffff);

	/* set debounce count as 25ms at 100M clock*/
	storage_set_debouncecount(25 * 1000 * 25);

	/*storage_set_rxwmark(STORAGE_FIFO_DEPTH/2 - 1);*/

	/*storage_set_txwmark(STORAGE_FIFO_DEPTH/2 - 1);*/
	/*storage_set_txwmark(STORAGE_FIFO_DEPTH - 1);*/
	storage_set_txwmark(0);

	storage_set_rxwmark(0);

	storage_clear_bit(STORAGE_CTRL,bSTORAGE_CTRL_ENABLE_INTERRUPT);

	/* clear any pending interrupts */
	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	/*rSTORAGE_PWREN = 0x0001;*/
	atxx_sd_write_reg(STORAGE_PWREN, 0x0001);

	return;
}

/*reset the card*/
void sd_init_card(uint32_t card)
{
	uint32_t resp[4],ret;
	sd_rca = 0;
	uint32_t c_size=0;
	uint8_t c_size_mult=0;
	uint8_t read_bl_len=0;
	uint8_t read_bl_len_per_sector=0;
	uint32_t timeout = 1000000;

	ATXX_MMC_DEBUG("sd_controller_init **augusta**\n");
	sd_controller_init();

	ATXX_MMC_DEBUG("sd card detect 0x%x\n", atxx_sd_read_reg(STORAGE_CDETECT));
	ATXX_MMC_DEBUG("sd write protect 0x%x\n", atxx_sd_read_reg(STORAGE_WRTPRT));

	sd_set_clock(0, (400 * KHZ));
	/*send  CMD0*/
	ATXX_MMC_DEBUG("send command 0\n");
	sd_base_command(card,CMD0,0,resp);
	udelay(10);

	/*send  ACMD41*/
	while(timeout--) {
		ATXX_MMC_DEBUG("send application command 41\n");
		sd_ocr = sd_application_command(card,ACMD41,0x00ff8000);
		if(sd_ocr == 0)	{
			printf("init sd card failed!!!\n");		
			return;
		} else if((sd_ocr & 0x00ff8000) == 0) {
			printf("sd card voltage range error!!!\n");
			return;
		}

		if(sd_ocr & 0x80000000)
			break;

		udelay(1);
	}

	sd_ocr &= 0x7fffffff;

	/*send  CMD2*/
	ATXX_MMC_DEBUG("send command 2\n");
	ret = sd_base_command(card,CMD2,0,resp);
	if(ret)	{
		printf("CMD2 init sd card failed!!!\n");
		return;
	}

	sd_cid[0] = resp[0];
	sd_cid[1] = resp[1];
	sd_cid[2] = resp[2];
	sd_cid[3] = resp[3];

	ATXX_MMC_DEBUG("sd_cid[0] 0x%x\n", sd_cid[0]);
	ATXX_MMC_DEBUG("sd_cid[1] 0x%x\n", sd_cid[1]);
	ATXX_MMC_DEBUG("sd_cid[2] 0x%x\n", sd_cid[2]);
	ATXX_MMC_DEBUG("sd_cid[3] 0x%x\n", sd_cid[3]);
	udelay(10);

	/*send  CMD3*/
	ATXX_MMC_DEBUG("send command 3\n");
	ret = sd_base_command(card,CMD3,0,resp);
	if(ret)	{
		printf("\n\rCMD3 init sd card failed!!!");
		return;
	}

	udelay(10);

	/*send  CMD9*/
	ATXX_MMC_DEBUG("send command 9\n");
	ret = sd_base_command(card,CMD9,0,resp);
	if(ret)	{
		printf("CMD3 init sd card failed!!!\n");
		return;
	}

	sd_csd[0] = resp[0];
	sd_csd[1] = resp[1];
	sd_csd[2] = resp[2];
	sd_csd[3] = resp[3];

	c_size = ((resp[1] >> 30)& 0x3)+((resp[2]& 0x3ff) << 2);
	c_size_mult = (resp[1] >> 15)& 0x7;
	read_bl_len = (resp[2] >> 16)& 0xf;

	read_bl_len_per_sector=(1<<read_bl_len)/STORAGE_BLOCK_SIZE; 

	mmc_blk_dev.lba = (c_size+1)*(1<<(c_size_mult+2))*read_bl_len_per_sector;

	ATXX_MMC_DEBUG("C_SIZE= 0x%x\n", c_size);
	ATXX_MMC_DEBUG("C_SIZE_MULT= 0x%x\n", c_size_mult);
	ATXX_MMC_DEBUG("READ_BL_LEN= 0x%x\n", read_bl_len);
	ATXX_MMC_DEBUG("READ_BL_LEN_PER_SECTOR= 0x%x\n", read_bl_len_per_sector);
	ATXX_MMC_DEBUG("mmc_blk_dev.lba= 0x%x\n", (uint32_t)mmc_blk_dev.lba);

	ATXX_MMC_DEBUG("rsd_csd[0] 0x%x\n", sd_csd[0]);
	ATXX_MMC_DEBUG("rsd_csd[1] 0x%x\n", sd_csd[1]);
	ATXX_MMC_DEBUG("rsd_csd[2] 0x%x\n", sd_csd[2]);
	ATXX_MMC_DEBUG("rsd_csd[3] 0x%x\n", sd_csd[3]);

	udelay(10);

	/*send  CMD7*/
	ATXX_MMC_DEBUG("send command 7\n");
	ret = sd_base_command(card,CMD7,sd_rca,resp);
	if(ret)	{
		printf("CMD7 init sd card failed!!!\n");
		return;
	}

	udelay(10);

	ATXX_MMC_DEBUG("set bus width\n");
	sd_set_buswidth(0,4);

	ATXX_MMC_DEBUG("get scr regster\n");
	sd_get_scr_register();
	ATXX_MMC_DEBUG("get status regster\n");
	sd_get_status_register();
	sd_set_clock(0, (25 * MHZ));

	return;
}

/* be sure of size of data LE fifo depth */
void sd_read_fifo(uint64_t *buf,uint32_t *len)
{
	volatile uint64_t *addr;
	int i;

	uint32_t rest;
	addr = (volatile uint64_t *)(sd_controller_base_addr + 0x100);
	*len = (atxx_sd_read_reg(STORAGE_STATUS) >> 17) & 0x1FFF;
	rest = *len;
	for(i=0; i<rest; i++) {
		buf[i] = addr[i];
	}

	return;
}

/* be sure of size of data LE fifo depth */
void sd_write_fifo(uint64_t *data,uint32_t size,uint32_t *len)
{
	volatile uint64_t *addr;
	int i;

	uint32_t rest;

	addr = (volatile uint64_t *)(sd_controller_base_addr + 0x100);
	rest = STORAGE_FIFO_DEPTH - ((atxx_sd_read_reg(STORAGE_STATUS) >> 17) & 0x1FFF);

	*len = (size >= rest)? rest : size;
	rest = *len;
	for(i=0; i<rest; i++) {
		addr[i] = data[i];
	}

	return;
}

void sd_read_card(uint32_t card,uint32_t addr)
{
	uint32_t resp[4],command,status;
	uint32_t size = 0,len;
	uint64_t *data = 0;
	int ret,i;
	uint32_t timeout = 1000000;

	resp[0] = 0;
	resp[1] = 0;
	resp[2] = 0;
	resp[3] = 0;

	for(i = 0;i < SD_BUF_SIZE;i++) {
                        buf[i] = 0;
        }

	/*i = ret;*/
	data = (uint64_t *)buf;

	address = addr;

	status = atxx_sd_read_reg(STORAGE_STATUS);
	while(timeout--) {
		if(status & bSTORAGE_STATUS_DATA_BUSY) {
                                ;
                } else {
                                break;
                }

                status = atxx_sd_read_reg(STORAGE_STATUS);
                udelay(1);
	}

	/*send  CMD17*/
	command = command_base_build(card,CMD17,COMMAND_BASE);
	ATXX_MMC_DEBUG("\n\rcommand 0x%x", command);

	/*rSTORAGE_BLKSIZ = STORAGE_BLOCK_SIZE;*/
	/*rSTORAGE_BYTCNT = STORAGE_BLOCK_SIZE;*/
	/*rSTORAGE_CMDARG = address;*/
	atxx_sd_write_reg(STORAGE_BLKSIZ, STORAGE_BLOCK_SIZE);
	atxx_sd_write_reg(STORAGE_BYTCNT, STORAGE_BLOCK_SIZE);
	atxx_sd_write_reg(STORAGE_CMDARG, address);

	/*rSTORAGE_CMD = command | 0x200;*/
	atxx_sd_write_reg(STORAGE_CMD, (command | 0x200));

	/* wait for data */
	timeout = 0xefffffff;
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & (bSTORAGE_INTSTS_DCRC | bSTORAGE_INTSTS_DRTO | bSTORAGE_INTSTS_SBE | \
                             bSTORAGE_INTSTS_EBE)) {
			ret = STORAGE_COMMAND_DCRC;
			atxx_sd_write_reg(STORAGE_RINTSTS, (bSTORAGE_INTSTS_DCRC | bSTORAGE_INTSTS_DRTO |\ 
                                                            bSTORAGE_INTSTS_SBE | bSTORAGE_INTSTS_EBE));
			ATXX_MMC_DEBUG("data error 0x%x\n", status);
			/*return;*/
		}

                /*bSTORAGE_INTSTS_RXDR*/
		if(status & bSTORAGE_INTSTS_HTO) {
			/*rSTORAGE_RINTSTS = bSTORAGE_INTSTS_HTO;*/
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_HTO);
			sd_read_fifo(&data[size],&len);
			size += len;
		}

		if(status & bSTORAGE_INTSTS_DTO) {
			ATXX_MMC_DEBUG("** size 0x%x\n", size);
			sd_read_fifo(&data[size],&len);
			size += len;
			ret = STORAGE_COMMAND_DTO;
			ATXX_MMC_DEBUG("&& size 0x%x\n", size);
			ATXX_MMC_DEBUG("\n\rcommand bSTORAGE_INTSTS_DTO\n");
			break;
		}

                /*bSTORAGE_INTSTS_RXDR*/
		if(status & bSTORAGE_INTSTS_RXDR) {
			ATXX_MMC_DEBUG("\n\r#:%x\n", size);
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_RXDR);
			/*if((0x40 - size) >= 16)*/
				sd_read_fifo(&data[size],&len);
				size += len;
		}

		udelay(1);
	}

	ATXX_MMC_DEBUG("read data break 0 0x%x\n", status);

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
	resp[0] = atxx_sd_read_reg(STORAGE_RESP0);

	ATXX_MMC_DEBUG("response 0 0x%x\n", resp[0]);

	status = atxx_sd_read_reg(STORAGE_STATUS);
	ATXX_MMC_DEBUG("fifo status 0x%x\n", status);
	ATXX_MMC_DEBUG("read data size:%x\n", size);

	for(i = 0;i < 512;i++) {
		if((i % 16) == 0) {
                        ATXX_MMC_DEBUG("\n\r");
                }

                ATXX_MMC_DEBUG("buf[%d]=%x", i, buf[i]);
	}

	return;
}

void sd_write_card(uint32_t card,uint32_t addr)
{
	uint32_t resp[4],command;
	uint32_t status,size = 0,len;
	uint64_t *data;
	int ret = STORAGE_COMMAND_CD,i;
	uint32_t timeout;

	resp[0] = 0;
	resp[1] = 0;                                                
	resp[2] = 0;
	resp[3] = 0;

	/*reset controller*/
	address = addr;
	for(i = 0;i < SD_BUF_SIZE;i++) {
                        buf[i] = (uint8_t)i;
        }

	data = (uint64_t *)buf;
	i = ret;

	/*send  CMD24*/
	command = command_base_build(card,CMD24,COMMAND_BASE);
	ATXX_MMC_DEBUG("\n\rcommand 0x%x", command);
	/*rSTORAGE_BLKSIZ = STORAGE_BLOCK_SIZE;*/
	atxx_sd_write_reg(STORAGE_BLKSIZ, STORAGE_BLOCK_SIZE);
	atxx_sd_write_reg(STORAGE_BYTCNT, STORAGE_BLOCK_SIZE);
	atxx_sd_write_reg(STORAGE_CMDARG, address);

	sd_write_fifo(&data[size],(64 - size),&len);
	size += len;

	/*rSTORAGE_CMD = command | 0x200 | 0x1000;*/
	atxx_sd_write_reg(STORAGE_CMD, (command | 0x200));

	/* wait for data */
	timeout = 10000000;
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & bSTORAGE_INTSTS_DCRC) {
			ret = STORAGE_COMMAND_DCRC;
			atxx_sd_write_reg(STORAGE_RINTSTS,bSTORAGE_INTSTS_DCRC);
			ATXX_MMC_DEBUG("command STORAGE_COMMAND_DCRC 0x%x\n", status);
			/*return;*/
		}

		if(status & bSTORAGE_INTSTS_DRTO) {
			ret = bSTORAGE_INTSTS_DRTO;
			atxx_sd_write_reg(STORAGE_RINTSTS,0xffffffff);
			ATXX_MMC_DEBUG("command bSTORAGE_INTSTS_DRTO 0x%x\n", status);
			return;
		}

		if(status & bSTORAGE_INTSTS_EBE) {
			ret = bSTORAGE_INTSTS_EBE;
			atxx_sd_write_reg(STORAGE_RINTSTS,0xffffffff);
			ATXX_MMC_DEBUG("command bSTORAGE_INTSTS_EBE 0x%x\n", status);
			return;
		}

		if(status & bSTORAGE_INTSTS_HTO) {
			sd_write_fifo(&data[size],(64 - size),&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS,bSTORAGE_INTSTS_HTO);
		}

		if(status & bSTORAGE_INTSTS_TXDR) {
			uint32_t rest;
			rest = ((atxx_sd_read_reg(STORAGE_STATUS) >> 17) & 0x1FFF);
			ATXX_MMC_DEBUG("est 0x%x\n", rest);
			sd_write_fifo(&data[size],(64 - size),&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS,bSTORAGE_INTSTS_TXDR);
		}

		if(status & bSTORAGE_INTSTS_DTO) {
			ret = STORAGE_COMMAND_DTO;
			ATXX_MMC_DEBUG("command STORAGE_COMMAND_DTO 0x%x\n", status);
			break;
		}

		udelay(1);
	}

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	resp[0] = atxx_sd_read_reg(STORAGE_RESP0);

	ATXX_MMC_DEBUG("response 0 0x%x\n", resp[0]);
	ATXX_MMC_DEBUG("write data 0x%x\n", size);
	status = atxx_sd_read_reg(STORAGE_STATUS);
	ATXX_MMC_DEBUG("fifo status 0x%x\n", status);
	status = atxx_sd_read_reg(STORAGE_RINTSTS);
	ATXX_MMC_DEBUG("rSTORAGE_RINTSTS status 0x%x\n", status);

	status = atxx_sd_read_reg(STORAGE_STATUS);
	timeout = 5000000;
	while(timeout--) {
		if(status & bSTORAGE_STATUS_DATA_BUSY) {
                                ;
                } else {
                                break;
                }

                status = atxx_sd_read_reg(STORAGE_STATUS);
                udelay(1);
	}

	return;
}

void sd_erase_card(uint32_t card,uint32_t addr)
{
	uint32_t resp[4],command,status;
	int ret,size;
	uint32_t timeout;

	resp[0] = 0;
	resp[1] = 0;
	resp[2] = 0;
	resp[3] = 0;

	address = addr;
	size = 512;

	/*Sets the address of the first write block to be rased*/
	command = command_base_build(card,CMD32,COMMAND_BASE);
	ret = storage_send_base_command(command,address,resp);
	if(ret != STORAGE_COMMAND_DTO) {
		printf("send CMD32 error ret 0x%x\n", ret);
		return;
	}

	ATXX_MMC_DEBUG("CMD32 resp 0x%x\n", resp[0]);
	/*Sets the address of the last write block to be rased*/
	command = command_base_build(card,CMD33,COMMAND_BASE);
	ret = storage_send_base_command(command,(address + size),resp);
	if(ret != STORAGE_COMMAND_DTO) {
		printf("send CMD33 error ret 0x%x\n", ret);
		return;
	}

	ATXX_MMC_DEBUG("CMD33 resp 0x%x\n", resp[0]);
	/*Erases all previously selected write blocks*/
	command = command_base_build(card,CMD38,COMMAND_BASE);
	ret = storage_send_base_command(command,0,resp);
	if(ret != STORAGE_COMMAND_DTO)
	{
		printf("send CMD38 error ret 0x%x\n", ret);
		return;
	}

	ATXX_MMC_DEBUG("CMD38 resp 0x%x\n", resp[0]);

	status = atxx_sd_read_reg(STORAGE_STATUS);
	timeout = 5000000;
	while(timeout--) {
		if(status & bSTORAGE_STATUS_DATA_BUSY);
		else
			break;
		status = atxx_sd_read_reg(STORAGE_STATUS);
		udelay(1);
	}

	return;
}

void sd_set_buswidth(uint32_t card,int width)
{


	if(width == 4) {
		/*rSTORAGE_CTYPE = 0x00000001;*/
		atxx_sd_write_reg(STORAGE_CTYPE, 0x00000001);
		sd_application_command(card,CMD6,2);
	} else {
		/*rSTORAGE_CTYPE = 0x00000000;*/
		atxx_sd_write_reg(STORAGE_CTYPE, 0x00000000);
		sd_application_command(card,CMD6,0);
	}

	return;
}

void sd_select_card(uint32_t card)
{
	uint32_t resp[4],command,arg = 0;
	int ret;

	resp[0] = 0;
	resp[1] = 0;
	resp[2] = 0;
	resp[3] = 0;

	arg = (sd_rca << 16) & 0xffff0000;
	command = command_base_build(card,CMD7,COMMAND_BASE);
	ATXX_MMC_DEBUG("select card command 0x%x\n", command);

	ret = storage_send_base_command(command,arg,resp);

	if(ret != STORAGE_COMMAND_DTO) {
		return;
	}

	return;
}

void sd_get_status(uint32_t card)
{
	uint32_t resp,status;

	resp = sd_application_command(card,CMD13,0);

	status = (resp >> 9) & 0xf;
	ATXX_MMC_DEBUG("status register 0x%x\n", resp);
	ATXX_MMC_DEBUG("%s\n", sd_state[status]);

	return;
}

void sd_set_blocksize(int card,uint32_t size)
{
	uint32_t block = 0,resp[4];

	block = (size & 0xf) + ((size >> 4) & 0xf) * 10 + ((size >> 8) & 0xf) * 100 + ((size >> 12) & 0xf) * 1000;
	if((block == 0) || (block > 512)) {
                        block = 512;
        }

	sd_base_command(card,CMD16,block,resp);

	return;
}

void sd_card_detect(void)
{
	/*uint32_t detect,wrtprot,temp,state;*/
	/*int i,time;*/

	/* clear any pending interrupts */
	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	ATXX_MMC_DEBUG("Read before card detect register 0x%x\n", cd_detect);
	ATXX_MMC_DEBUG("Read before write protect register 0x%x\n", cd_wrprotect);	
	cd_detect = atxx_sd_read_reg(STORAGE_CDETECT);
	cd_wrprotect = atxx_sd_read_reg(STORAGE_WRTPRT);

	ATXX_MMC_DEBUG("Read sd card detect register 0x%x\n", cd_detect);
	ATXX_MMC_DEBUG("Read sd write protect register 0x%x\n", cd_wrprotect);

#if 0
	for(time = 0;time < 10;) {
		state = atxx_sd_read_reg(STORAGE_RINTSTS);
		ATXX_MMC_DEBUG("sd card state register 0x%x\n", state);
		time++;
		if(state & bSTORAGE_INTSTS_CDT)	{
			atxx_sd_write_reg(STORAGE_RINTSTS , bSTORAGE_INTSTS_CDT);
			detect = atxx_sd_read_reg(STORAGE_CDETECT);

			ATXX_MMC_DEBUG("card detected register 0x%x\n", detect);
			temp = (cd_detect & (~detect));

			for(i = 0; i< STORAGE_MAX_CARD_NUMBER;i++) {
				if(temp & (1 << i)) {
                                        ATXX_MMC_DEBUG("card detected, slot is 0x%x\n", i);
                                }
                        }

                        temp = ((~cd_detect) & detect);

                        for(i = 0; i< STORAGE_MAX_CARD_NUMBER;i++) {
                                if(temp & (1 << i))
                                        ATXX_MMC_DEBUG("card removed, slot is 0x%x\n", i);
                        }

                        cd_detect = detect;

                        wrtprot = atxx_sd_read_reg(STORAGE_WRTPRT);

                        ATXX_MMC_DEBUG("sd write protect register 0x%x \n", i);
                        temp = (cd_wrprotect & (~wrtprot));

                        for(i = 0; i< STORAGE_MAX_CARD_NUMBER;i++) {
                                if(temp & (1 << i))
                                        ATXX_MMC_DEBUG("card locked, slot is 0x%x\n", i);
                        }

                        temp = ((~cd_wrprotect) & wrtprot);

                        for(i = 0; i< STORAGE_MAX_CARD_NUMBER;i++) {
                                if(temp & (1 << i))
                                        ATXX_MMC_DEBUG("card locked, slot is 0x%x\n", i);
                        }

                        cd_wrprotect = wrtprot;

                        time++;
                }
        }

        return;
#endif

}

void sd_multiblock_erase(int card, uint32_t addr, uint32_t blk_cnts)
{
	uint32_t resp[4],command,status;
	int ret,size;
	uint32_t timeout;

	resp[0] = 0;
	resp[1] = 0;
	resp[2] = 0;
	resp[3] = 0;

	ATXX_MMC_DEBUG("blk_cnts=0x%x\n", blk_cnts);

	address = addr;
	size = (STORAGE_BLOCK_SIZE * blk_cnts);

	/*Sets the address of the first write block to be rased*/
	command = command_base_build(card,CMD32,COMMAND_BASE);
	ret = storage_send_base_command(command,address,resp);
	if(ret != STORAGE_COMMAND_DTO) {
		printf("send CMD32 error ret 0x%x\n", ret);
		return;
	}

	ATXX_MMC_DEBUG("CMD32 resp 0x%x\n", resp[0]);

	/*Sets the address of the last write block to be rased*/
	command = command_base_build(card,CMD33,COMMAND_BASE);
	ret = storage_send_base_command(command,(address + size),resp);
	if(ret != STORAGE_COMMAND_DTO) {
		printf("send CMD33 error ret 0x%x\n", ret);
		return;
	}

	ATXX_MMC_DEBUG("CMD32 resp 0x%x\n", resp[0]);

	/*Erases all previously selected write blocks*/
	command = command_base_build(card,CMD38,COMMAND_BASE);
	ret = storage_send_base_command(command,0,resp);
	if(ret != STORAGE_COMMAND_DTO) {
		printf("send CMD38 error ret 0x%x\n", ret);
		return;
	}

	ATXX_MMC_DEBUG("CMD32 resp 0x%x\n", resp[0]);

	status = atxx_sd_read_reg(STORAGE_STATUS);
	timeout = 0xefffffff;
	while(timeout--) {
		if(status & bSTORAGE_STATUS_DATA_BUSY);
		else
			break;

		status = atxx_sd_read_reg(STORAGE_STATUS);
		udelay(1);
	}

	return;
}

void sd_multiblock_read(int card, uint32_t addr, uint32_t blk_cnts, uint8_t *pdata)
{
	uint32_t command,status;
	uint32_t size = 0,len,number;
	uint64_t *data;
	int ret = 0;
	uint32_t timeout;

	data = (uint64_t *)pdata;
	number = blk_cnts * STORAGE_BLOCK_SIZE;

	/*send  CMD18*/
	command = command_base_build(card,CMD18,COMMAND_BASE);
	atxx_sd_write_reg(STORAGE_BLKSIZ, STORAGE_BLOCK_SIZE);
	atxx_sd_write_reg(STORAGE_BYTCNT, number);
	atxx_sd_write_reg(STORAGE_CMDARG, addr);

	/*rSTORAGE_CMD = command | 0x200;*/
	atxx_sd_write_reg(STORAGE_CMD, command | 0x200);

	/* wait for data */
	timeout = 0xefffffff;
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & (bSTORAGE_INTSTS_DCRC | bSTORAGE_INTSTS_DRTO | bSTORAGE_INTSTS_SBE | \
                             bSTORAGE_INTSTS_EBE))
		{
			ret = STORAGE_COMMAND_DCRC;
			atxx_sd_write_reg(STORAGE_RINTSTS, (bSTORAGE_INTSTS_DCRC | bSTORAGE_INTSTS_DRTO |\
                                                             bSTORAGE_INTSTS_SBE | bSTORAGE_INTSTS_EBE));
			/*return;*/
		}

                 /*bSTORAGE_INTSTS_HTO*/
		if(status & bSTORAGE_INTSTS_HTO) {
			sd_read_fifo(&data[size],&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_HTO);
		}

                /*bSTORAGE_INTSTS_RXDR*/
                if(status & bSTORAGE_INTSTS_RXDR) {
			sd_read_fifo(&data[size],&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_RXDR);
		}

		if(status & bSTORAGE_INTSTS_DTO) {
			sd_read_fifo(&data[size],&len);
			size += len;
			break;
		}

		if(size >= (number / 8))
			break;


		udelay(1);
	}

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
	size *= 8;
	command = command_base_build(card,CMD12,COMMAND_BASE);
	atxx_sd_write_reg(STORAGE_CMDARG, 0);

	atxx_sd_write_reg(STORAGE_CMD, command);
	udelay(10);

	if(ret)
		ATXX_MMC_DEBUG("\n\r");

	timeout = 0xefffffff;
	while(timeout--) {
		sd_read_fifo(&data[size],&len);
		size += len;
		if(len == 0)
			break;
		udelay(1);
	}

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	return;
}

void sd_multiblock_write(int card, uint32_t addr, uint32_t blk_cnts, uint8_t *pdata)
{
	uint32_t command;
	uint32_t status,size = 0,len,total,number;
	uint64_t *data;
	int ret = STORAGE_COMMAND_CD;
	uint32_t timeout;

	data = (uint64_t *)pdata;
	number = blk_cnts * STORAGE_BLOCK_SIZE;

	for(len = 0;len < number;len++)	{
		*pdata = (uint8_t)len;
		pdata++;
	}

	total = number / 8;

	/*send  CMD25*/
	command = command_base_build(card,CMD25,COMMAND_BASE);
	atxx_sd_write_reg(STORAGE_BLKSIZ, STORAGE_BLOCK_SIZE);
	atxx_sd_write_reg(STORAGE_BYTCNT, number);
	atxx_sd_write_reg(STORAGE_CMDARG, addr);

	sd_write_fifo(&data[size],(total - size),&len);
	size += len;

	atxx_sd_write_reg(STORAGE_CMD, ( command | 0x200));


	/* wait for data */
	timeout = 0xefffffff;
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & bSTORAGE_INTSTS_DCRC) {
			ret = STORAGE_COMMAND_DCRC;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_DCRC);
			/*return;*/
		}

		if(status & bSTORAGE_INTSTS_DRTO) {
			ret = bSTORAGE_INTSTS_DRTO;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_DRTO);
			/*return;*/
		}

		if(status & bSTORAGE_INTSTS_EBE) {
			ret = bSTORAGE_INTSTS_EBE;
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			/*return;*/
		}

		if(status & bSTORAGE_INTSTS_HTO) {
			sd_write_fifo(&data[size],(total - size),&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_HTO);
		}

		if(status & bSTORAGE_INTSTS_TXDR) {
			sd_write_fifo(&data[size],(total - size),&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_TXDR);
		}

		if(status & bSTORAGE_INTSTS_DTO) {
			break;
		}

		udelay(1);
	}

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
	command = command_base_build(card,CMD12,COMMAND_BASE);
	atxx_sd_write_reg(STORAGE_CMDARG, 0);

	atxx_sd_write_reg(STORAGE_CMD, command);
	atxx_sd_write_reg(STORAGE_CMD, command);
	udelay(10);

	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);

	if(ret)
		ATXX_MMC_DEBUG("\n\r");

	status = atxx_sd_read_reg(STORAGE_STATUS);
	timeout = 0xefffffff;
	while(timeout--) {
		if(status & bSTORAGE_STATUS_DATA_BUSY);
		else
			break;
		status = atxx_sd_read_reg(STORAGE_STATUS);
		udelay(1);
	}

	return;
}

void sd_get_scr_register(void)
{
	uint32_t resp[4],command,status,size = 0,len,blksize;
	uint64_t *data;
	int ret,i;
	uint8_t temp;
	uint32_t timeout;

	for(i = 0;i < 8;i++){
                 sd_scr[i] = 0;
        }

	ret = sd_base_command(0,CMD55,sd_rca,resp);
	if(ret)
	{
		ATXX_MMC_DEBUG("rcommand CMD55 send error\n");
		return;
	}

	blksize = atxx_sd_read_reg(STORAGE_BLKSIZ);
	command = command_base_build(0,51,COMMAND_APPLICATION);
	ATXX_MMC_DEBUG("rACMD51 0x%x\n", command);
	data = (uint64_t *)sd_scr;

	/*now send application command*/
	/*rSTORAGE_BLKSIZ = 8;*/
	atxx_sd_write_reg(STORAGE_BLKSIZ, 9);
	atxx_sd_write_reg(STORAGE_BYTCNT, 8);
	atxx_sd_write_reg(STORAGE_CMDARG, 0);
	atxx_sd_write_reg(STORAGE_CMD, (command | 0x200));

	/* wait for data */
	timeout=0xefffffff;
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & (bSTORAGE_INTSTS_DCRC | bSTORAGE_INTSTS_DRTO | bSTORAGE_INTSTS_SBE |\
                              bSTORAGE_INTSTS_EBE)) {
			ret = STORAGE_COMMAND_DCRC;
			atxx_sd_write_reg(STORAGE_RINTSTS, (bSTORAGE_INTSTS_DCRC | bSTORAGE_INTSTS_DRTO |\
                                                             bSTORAGE_INTSTS_SBE | bSTORAGE_INTSTS_EBE));
			ATXX_MMC_DEBUG("data error 0x%x\n", status);
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			return;
		}

                /*bSTORAGE_INTSTS_HTO*/
		if(status & bSTORAGE_INTSTS_HTO) {
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_HTO);
			sd_read_fifo(&data[size],&len);
			size += len;
		}

                /*bSTORAGE_INTSTS_RXDR*/
                if(status & bSTORAGE_INTSTS_RXDR) {
			status = atxx_sd_read_reg(STORAGE_STATUS);
			sd_read_fifo(&data[size],&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_RXDR);
		}

		if(status & bSTORAGE_INTSTS_DTO) {
			sd_read_fifo(&data[size],&len);
			size += len;
			ret = STORAGE_COMMAND_DTO;
			break;
		}

		udelay(1);
	}

	ATXX_MMC_DEBUG("read register status 0x%x\n", status);
	ATXX_MMC_DEBUG("read register size 0x%x\n", size);
	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
	/*rSTORAGE_BLKSIZ = blksize;*/
	atxx_sd_write_reg(STORAGE_BLKSIZ, blksize);

	for(i = 0;i < 4;i++) {
		temp = sd_scr[i];
		sd_scr[i] = sd_scr[7 - i];
		sd_scr[7 - i] = temp;
	}

	ATXX_MMC_DEBUG("scr register:\n\r");
	for(i = 0;i <8;i++) {
		ATXX_MMC_DEBUG("sd_scr[%d]=0x%x\n", i, sd_scr[i]);
	}

	return;

}

void sd_get_status_register(void)
{
	uint32_t resp[4],command,status,size = 0,len,blksize;
	uint64_t *data;
	int ret,i;
	uint8_t temp;
	uint32_t timeout;

	for(i = 0; i < 64;i++)
		sd_status[i] = 0;

	ret = sd_base_command(0,CMD55,sd_rca,resp);
	if(ret)	{
		printf("command CMD55 send error\n");
		return;
	}
	/*blksize = rSTORAGE_BLKSIZ;*/
	blksize = atxx_sd_read_reg(STORAGE_BLKSIZ);
	command = command_base_build(0,13,COMMAND_APPLICATION);
	ATXX_MMC_DEBUG("ACMD13 0x%x\n", command);
	data = (uint64_t *)sd_status;

	/*now send application command*/
	atxx_sd_write_reg(STORAGE_BLKSIZ, 64);
	atxx_sd_write_reg(STORAGE_BYTCNT, 64);
	atxx_sd_write_reg(STORAGE_CMDARG, 0);
	atxx_sd_write_reg(STORAGE_CMD, (command | 0x200));

	/* wait for data */
	timeout = 0xefffffff;
	while(timeout--) {
		status = atxx_sd_read_reg(STORAGE_RINTSTS);
		if(status & (bSTORAGE_INTSTS_DRTO | bSTORAGE_INTSTS_SBE | bSTORAGE_INTSTS_EBE))	{
			ret = bSTORAGE_INTSTS_DRTO;
			atxx_sd_write_reg(STORAGE_RINTSTS, (bSTORAGE_INTSTS_DCRC | bSTORAGE_INTSTS_DRTO | bSTORAGE_INTSTS_SBE | bSTORAGE_INTSTS_EBE));
			ATXX_MMC_DEBUG("data error 0x%x\n", status);
			atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
			return;
		}

		if(status & (bSTORAGE_INTSTS_DCRC)) {
			ret = STORAGE_COMMAND_DCRC;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_DCRC);
			ATXX_MMC_DEBUG("data error 0x%x\n", status);
		}

                /*bSTORAGE_INTSTS_HTO*/
		if(status & bSTORAGE_INTSTS_HTO) {
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_HTO);
			sd_read_fifo(&data[size],&len);
			size += len;
		}

                /*bSTORAGE_INTSTS_RXDR*/
		if(status & bSTORAGE_INTSTS_RXDR) {
			status = atxx_sd_read_reg(STORAGE_STATUS);
			sd_read_fifo(&data[size],&len);
			size += len;
			atxx_sd_write_reg(STORAGE_RINTSTS, bSTORAGE_INTSTS_RXDR);
		}

		if(status & bSTORAGE_INTSTS_DTO) {
			sd_read_fifo(&data[size],&len);
			size += len;
			ret = STORAGE_COMMAND_DTO;
			break;
		}

		udelay(1);
	}

	ATXX_MMC_DEBUG("read register status 0x%x\n", status);
	ATXX_MMC_DEBUG("read register size 0x%x\n", size);
	atxx_sd_write_reg(STORAGE_RINTSTS, 0xffffffff);
	atxx_sd_write_reg(STORAGE_BLKSIZ, blksize);

	for(i = 0;i < 32;i++) {
		temp = sd_status[i];
		sd_status[i] = sd_status[63 - i];
		sd_status[63 - i] = temp;
	}


	ATXX_MMC_DEBUG("sd status register:\n");
	for(i = 0;i < 64;i++) {
		if((i % 16) == 0)
			ATXX_MMC_DEBUG("\n");

		ATXX_MMC_DEBUG("sd_status[%d]=0x%x\n", i, sd_status[i]);
	}

	return;

}

static const int8_t sd_speed[64][16] =
{
	"reserved",   "reserved",   "reserved",  "reserved",
	"100kbit/s",  "1Mbit/s",    "10Mbit/s",  "100Mbit/s",
	"120kbit/s",  "1.2Mbit/s",  "12Mbit/s",  "120Mbit/s",
	"130kbit/s",  "1.3Mbit/s",  "13Mbit/s",  "130Mbit/s",
	"150kbit/s",  "1.5Mbit/s",  "15Mbit/s",  "150Mbit/s",
	"200kbit/s",  "2Mbit/s",    "20Mbit/s",  "200Mbit/s",
	"250kbit/s",  "2.5Mbit/s",  "25Mbit/s",  "250Mbit/s",
	"300kbit/s",  "3Mbit/s",    "30Mbit/s",  "300Mbit/s",
	"350kbit/s",  "3.5Mbit/s",  "35Mbit/s",  "350Mbit/s",
	"400kbit/s",  "4Mbit/s",    "40Mbit/s",  "400Mbit/s",
	"450kbit/s",  "4.5Mbit/s",  "45Mbit/s",  "450Mbit/s",
	"500kbit/s",  "5Mbit/s",    "50Mbit/s",  "500Mbit/s",
	"550kbit/s",  "5.5Mbit/s",  "55Mbit/s",  "550Mbit/s",
	"600kbit/s",  "6Mbit/s",    "60Mbit/s",  "600Mbit/s",
	"700kbit/s",  "7Mbit/s",    "70Mbit/s",  "700Mbit/s",
	"800kbit/s",  "8Mbit/s",    "80Mbit/s",  "800Mbit/s",
};

void sd_get_card_info(void)
{
	uint8_t cid_pnm[6];
	uint8_t speed,size,multi;
	uint16_t type,csize;
	uint32_t mem;

	/*ocr register*/
	ATXX_MMC_DEBUG("OCR register: 0x%x\n", sd_ocr);

	/*product name of cid*/
	cid_pnm[0] = (uint8_t) (sd_cid[3] & 0xff);
	cid_pnm[1] = (uint8_t) ((sd_cid[2] & 0xff000000) >> 24);
	cid_pnm[2] = (uint8_t) ((sd_cid[2] & 0xff0000) >> 16);
	cid_pnm[3] = (uint8_t) ((sd_cid[2] & 0xff00) >> 8);
	cid_pnm[4] = (uint8_t) ((sd_cid[2] & 0xff));
	cid_pnm[5] = 0;
	ATXX_MMC_DEBUG("product name:");
	ATXX_MMC_DEBUG("%s\n", cid_pnm);

	/*CSD*/
	speed = (uint8_t)(sd_csd[3] & 0xff);
	ATXX_MMC_DEBUG("max transfer speed:");
	ATXX_MMC_DEBUG("%s\n", sd_speed[((speed & 0x78) >> 3) * 4 + (speed & 0x7)]);

	size =  (uint8_t)((sd_csd[2] & 0xf0000) >> 16);
	ATXX_MMC_DEBUG("max read block size: ");
	if(size == 9)
		ATXX_MMC_DEBUG("512 Bytes\n");
	else if(size == 10)
		ATXX_MMC_DEBUG("1024 Bytes\n");
	else if(size == 11)
		ATXX_MMC_DEBUG("2048 Bytes\n");
	else
		ATXX_MMC_DEBUG("reserverd 0x%x\n", size);

	size =  (uint8_t)((sd_csd[0] & 0x3c00000) >> 22);
	ATXX_MMC_DEBUG("max write block size: ");

	if(size == 9)
		ATXX_MMC_DEBUG("512 Bytes\n");
	else if(size == 10)
		ATXX_MMC_DEBUG("1024 Bytes\n");
	else if(size == 11)
		ATXX_MMC_DEBUG("2048 Bytes\n");
	else
		ATXX_MMC_DEBUG("reserverd 0x%x\n", size);

	size =  (uint8_t)((sd_csd[2] & 0xf0000) >> 16);
	csize = (uint16_t)(((sd_csd[1] & 0xc0000000) >> 30) | ((sd_csd[2] & 0x3ff) << 2));
	multi = (uint8_t)((sd_csd[1] & 0x38000) >> 15);
	mem = ((csize + 1) * (1 << (multi + 2)) * (1 << size));
	ATXX_MMC_DEBUG("reserverd size 0x%x\n", mem);


	/*SCR*/
	speed = sd_scr[7] & 0xf;
	if(speed & 1)
		ATXX_MMC_DEBUG("card support 1bit bus width\n");
	if(speed & 4)
		ATXX_MMC_DEBUG("card support 4bit bus width\n");

	/*SD status*/
	speed = ((sd_status[63] & 0x30) >> 6);
	if(speed == 0)
		ATXX_MMC_DEBUG("current card bus width is 1bit\n");
	else if(speed == 2)
		ATXX_MMC_DEBUG("current card bus width is 4bit\n");
	else
		ATXX_MMC_DEBUG("reserverd 0x%x\n", speed);

	type = ((sd_status[61] << 8) | sd_status[60]);
	if(type == 0)
		ATXX_MMC_DEBUG("current card is RD/WR card\n");

	else if(type == 1)
		ATXX_MMC_DEBUG("current card is SD ROM card\n");

	mem = ((sd_status[56]) | (sd_status[57] << 8) | (sd_status[58] << 16) | (sd_status[59] << 24));
	mem *= (multi * (1 << size));
	ATXX_MMC_DEBUG("protected area size 0x%x\n", mem);

	/*RCA*/
	ATXX_MMC_DEBUG("RCA register 0x%x\n", sd_rca);

	return;
}

unsigned long mmc_block_read(int dev_num, unsigned long blknr, lbaint_t blkcnt,void *dst)
{
	uint32_t addr=0;

	ATXX_MMC_DEBUG("mmc_block_read():blknr=0x%x,blkcnt=0x%x\n", (uint32_t)blknr, (uint32_t)blkcnt);
	addr = blknr*STORAGE_BLOCK_SIZE;
	sd_multiblock_read(dev_num, addr, blkcnt, (uint8_t *)dst);	
	return blkcnt; 
}


block_dev_desc_t *mmc_get_dev(int dev)
{
	ATXX_MMC_DEBUG("augusta mmc_get_dev():&mmc_blk_dev=0x%x\n", (uint32_t)&mmc_blk_dev);
	return (block_dev_desc_t *) &mmc_blk_dev;
}

int mmc_legacy_init(int verbose)
{
	int ret = -1;

	ret = pmu_power_control(PPS_SD, PS_ON);
	if(0 == ret) {
		ATXX_MMC_DEBUG("mmc_legacy_init: pmu_power_control is OK!\n");
	}
	else 
		printf("mmc_legacy_init: pmu_power_control is fail!\n");

	sd_card_detect();

	if(0 == cd_detect) {
		sd_init_card(0);
	}
	else
	{
		ATXX_MMC_DEBUG("There is no SD card!\n");
		return -1;
	}

	mmc_blk_dev.if_type = IF_TYPE_MMC;
	mmc_blk_dev.part_type = PART_TYPE_DOS;
	mmc_blk_dev.dev = 0;
	mmc_blk_dev.lun = 0;
	mmc_blk_dev.type = 0;

	mmc_blk_dev.blksz = STORAGE_BLOCK_SIZE;
	/*mmc_blk_dev.lba = 0x400000; get from CSD info */
	mmc_blk_dev.removable = 0;
	mmc_blk_dev.block_read = mmc_block_read;
	ret = fat_register_device(&mmc_blk_dev, 1);
	if(0 == ret) {
		ATXX_MMC_DEBUG("mmc_legacy_init: fat_register_device is OK!\n");
	}
	else 
		printf("mmc_legacy_init: fat_register_device is fail!\n");

	return 0;
}



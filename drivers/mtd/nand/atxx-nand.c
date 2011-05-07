/*------------------------------------------------------------------------------
 * (c) Copyright, Augusta Technology, Inc., 2006-present.
 * (c) Copyright, Augusta Technology USA, Inc., 2006-present.
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
#include <common.h>
#include <linux/types.h>
#include <linux/err.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <nand.h>
#include <malloc.h>
#include <asm/arch-atxx/delay.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/regs_base.h>

#include "atxx-nand.h"

#define CONFIG_ATXX_NAND_DMA

#ifdef CONFIG_ATXX_NAND_DEBUG
static int bad_scan = 1;
#endif

static int ecc_number = CONFIG_NAND_ECC;
static int oob_size = CONFIG_NAND_OOBSIZE;
static int size_per_sector = 512;
static int bus_width = 0;
struct mtd_info *mtd_bak;

/**********************************/

static  uint32_t atxx_nd_read_reg(uint32_t reg)
{
        return readl(ATXX_NAND_BASE + reg);
}

static  void atxx_nd_write_reg(uint32_t reg, uint32_t val)
{
        writel(val, ATXX_NAND_BASE + reg);
}

 void ATXX_ND_LOG(void)
{
	/* dump registers */
	printf(	"NFC_PARA0:%08x, NFC_TIMING:%08x\n, NFC_CMD:%08x, \
			NFC_STATUS:%08x\n, NFC_CCFG:%08x, NFC_SOFT_PIN:%08x\n\
			NFC_CFG0:%08x, NFC_CFG1:%08x\n, NFC_CFG2:%08x,\
			NFC_FADDR0:%08x\n, NFC_FADDR1:%08x, NFC_FIFO_CFG:%08x\n\
			NFC_INT_CODE:%08x, NFC_INT_MASK:%08x\n, NFC_DMA_CFG:%08x,\
			NFC_DMA_CTRL:%08x\n, NFC_DMA_STAT:%08x\
			NFC_DEBUG0:%08x\n, NFC_DEBUG1:%08x, NFC_DEBUG2:%08x\n",
			atxx_nd_read_reg(REG_NFC_PARA0), atxx_nd_read_reg(REG_NFC_TIMING), 
			atxx_nd_read_reg(REG_NFC_CMD), atxx_nd_read_reg(REG_NFC_STATUS), 
			atxx_nd_read_reg(REG_NFC_CCFG), atxx_nd_read_reg(REG_NFC_SOFT_PIN), 
			atxx_nd_read_reg(REG_NFC_CFG0), atxx_nd_read_reg(REG_NFC_CFG1), 
			atxx_nd_read_reg(REG_NFC_CFG2), atxx_nd_read_reg(REG_NFC_FADDR0), 
			atxx_nd_read_reg(REG_NFC_FADDR1), atxx_nd_read_reg(REG_NFC_FIFO_CFG),
			atxx_nd_read_reg(REG_NFC_INT_CODE), atxx_nd_read_reg(REG_NFC_INT_MASK), 
			atxx_nd_read_reg(REG_NFC_DMA_CFG), atxx_nd_read_reg(REG_NFC_DMA_CTRL),
			atxx_nd_read_reg(REG_NFC_DMA_STAT), atxx_nd_read_reg(REG_NFC_DEBUG0), 
			atxx_nd_read_reg(REG_NFC_DEBUG1), atxx_nd_read_reg(REG_NFC_DEBUG2));
}

static  void NFC_RESET(void)
{
	atxx_nd_write_reg(REG_NFC_CFG2, atxx_nd_read_reg(REG_NFC_CFG2) & ~NFC_CFG2_RESET);
	udelay(10);
	atxx_nd_write_reg(REG_NFC_CFG2, atxx_nd_read_reg(REG_NFC_CFG2) | NFC_CFG2_RESET);
	udelay(10);
}

static  void WAIT_CMD_END(void)
{
	int i = 0;
	do {
		if (!(atxx_nd_read_reg(REG_NFC_INT_CODE) & 0x1)) {
			udelay(1);
		} else {
			atxx_nd_write_reg(REG_NFC_INT_CODE, 0x1);
			return;
		}
	} while (i++ < ATXX_ND_TIMEOUT);

	printf("wait command end timeout\n");
	ATXX_ND_LOG();
}

static  void NFC_WAIT_FIFO(void)
{
	int i = 0;
	do {
		if (atxx_nd_read_reg(REG_NFC_FIFO_STATUS) & NFC_FIFO_FULL_BIT) {
			udelay(1);
		} else {
			return;
		}
	} while (i++ < ATXX_ND_TIMEOUT);

	printf(
			"wait fifo not full timeout\n");
	ATXX_ND_LOG();
}

static  void NFC_WAIT_FIFO_EMPTY(void)
{
	int i = 0;
	do {
		if (!(atxx_nd_read_reg(REG_NFC_FIFO_STATUS) & NFC_FIFO_EMPTY_BIT)) {
			udelay(1);
		} else {
			return;
		}
	} while (i++ < ATXX_ND_TIMEOUT);

	printf("wait fifo emptyl timeout\n");
	ATXX_ND_LOG();
}

static  void WAIT_ENCODE_READY(void)
{
	int i = 0;
	do {
		if (!(atxx_nd_read_reg(REG_NFC_INT_CODE) & NFC_INT_ENCODE_MARK)) {
			udelay(1);
		} else {
			atxx_nd_write_reg(REG_NFC_INT_CODE, NFC_INT_ENCODE_MARK);
			return;
		}
	} while (i++ < ATXX_ND_TIMEOUT);

	printf( "wait ECC ready timeout\n");
	ATXX_ND_LOG();
}

static  void WAIT_DECODE_READY(void)
{
	int i = 0;
	do {
		if (!(atxx_nd_read_reg(REG_NFC_INT_CODE) & NFC_INT_DECODE_MARK)) {
			udelay(1);
		} else {
			atxx_nd_write_reg(REG_NFC_INT_CODE, NFC_INT_DECODE_MARK);
			return;
		}
	} while (i++ < ATXX_ND_TIMEOUT);

	printf( "wait ECC ready timeout\n");
	ATXX_ND_LOG();
}

static  void WAIT_DECODE_CODE_READY(int code_word)
{
	int i = 0;
	do {
		if (!(atxx_nd_read_reg(REG_NFC_INT_CODE) & (1 << (8 + code_word)))) {
			udelay(1);
		} else {
			atxx_nd_write_reg(REG_NFC_INT_CODE, 1 << (8 + code_word));
			return;
		}
	} while (i++ < ATXX_ND_TIMEOUT);

	printf( "wait ECC ready timeout\n");
	ATXX_ND_LOG();
}

static  void WAIT_DMA_FINI(void)
{
	uint32_t i = 0;
	do {
		if (!(atxx_nd_read_reg(REG_NFC_INT_CODE) & NFC_INT_DMA_FINI)) {
			udelay(1);
		} else {
			atxx_nd_write_reg(REG_NFC_INT_CODE, NFC_INT_DMA_FINI);
			return;
		}
	} while (i++ < 0xffffffful);
}

static  void nfc_send_readid_cmd(void)
{
	uint32_t cmd;
	atxx_nd_write_reg(REG_NFC_FADDR0, ID_DATA_ADDR);
	cmd = (NFC_CMD_READ_ID << NFC_CMD_CMD_SHIFT) |
		(4 << NFC_CMD_DATA_LEN_SHIFT) |
		(1 << NFC_CMD_ADDR_LEN_SHIFT) | NFC_CMD_ENABLE;
	atxx_nd_write_reg(REG_NFC_CMD, cmd);
	WAIT_CMD_END();
}

static  void nfc_send_readstatus_cmd(void)
{
	uint32_t cmd;
	cmd = (NFC_CMD_READ_STATUS << NFC_CMD_CMD_SHIFT) |
		(1 << NFC_CMD_DATA_LEN_SHIFT) |
		(0 << NFC_CMD_ADDR_LEN_SHIFT) | NFC_CMD_ENABLE;
	atxx_nd_write_reg(REG_NFC_CMD, cmd);
	WAIT_CMD_END();
}

static  void nfc_send_reset_cmd(void)
{
	atxx_nd_write_reg(REG_NFC_CMD, (NFC_CMD_RESET << NFC_CMD_CMD_SHIFT) 
			| NFC_CMD_ENABLE);
	WAIT_CMD_END();
}

static  void nfc_config_big_page(int width, int addr)
{
	int nd_addr;

	nd_addr = addr & 0xffff0000;
	nd_addr |= (addr & 0xffff) / 2;

	if (width == NAND_BUSWIDTH_16) {
		atxx_nd_write_reg(REG_NFC_FADDR0, nd_addr);
		atxx_nd_write_reg(REG_NFC_BADDR, nd_addr);
	} else {
		atxx_nd_write_reg(REG_NFC_FADDR0, addr);
	}
}


/******************** cmd for read operation **********************/
static  void nfc_config_small_page_read(int width, int page)
{
	atxx_nd_write_reg(REG_NFC_PARA0, 
			atxx_nd_read_reg(REG_NFC_PARA0) | NFC_PARAR0_OLD);
	atxx_nd_write_reg(REG_NFC_CFG0, 
			atxx_nd_read_reg(REG_NFC_CFG0) & 0xffffff00);
	atxx_nd_write_reg(REG_NFC_CCFG, 
			NAND_CMD_READ1 | (NAND_CMD_READOOB << 16));

	if (width == NAND_BUSWIDTH_16) {
		atxx_nd_write_reg(REG_NFC_FADDR0, page << 8);
		atxx_nd_write_reg(REG_NFC_BADDR, page << 8);
	} else {
		atxx_nd_write_reg(REG_NFC_FADDR0, page << 8);
	}
}

static  void nfc_send_readoob_cmd(u32 data_len, u32 addr_len)
{
	uint32_t cmd;

	cmd = (NFC_CMD_READ << NFC_CMD_CMD_SHIFT)
		| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
		| (data_len << NFC_CMD_DATA_LEN_SHIFT)
		| NFC_CMD_WAIT_BUSY | NFC_CMD_ENABLE;

	atxx_nd_write_reg(REG_NFC_CMD, cmd);
	WAIT_CMD_END();
}

static  void nfc_send_readoob_udf_cmd(u32 data_len, u32 addr_len)
{
	uint32_t cmd;

	cmd = (NFC_CMD_UDFB_READ << NFC_CMD_CMD_SHIFT)
		| (data_len << NFC_CMD_DATA_LEN_SHIFT)
		| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
		| NFC_CMD_WAIT_BUSY | NFC_CMD_INT | NFC_CMD_ENABLE;

	atxx_nd_write_reg(REG_NFC_CMD, cmd);
	WAIT_CMD_END();
}

static  void nfc_send_readpage_cmd(u32 data_len, u32 addr_len)
{
	uint32_t cmd;

	cmd = (NFC_CMD_READ << NFC_CMD_CMD_SHIFT)
		| (data_len << NFC_CMD_DATA_LEN_SHIFT)
		| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
		| NFC_CMD_WAIT_BUSY | NFC_CMD_ENABLE;

	atxx_nd_write_reg(REG_NFC_CMD, cmd);
	WAIT_CMD_END();
}

static  void nfc_send_readpage_udf_cmd(u32 data_len, u32 addr_len, u32 ecc_enable)
{
	uint32_t cmd;

	atxx_nd_write_reg(REG_NFC_CCFG, (NAND_CMD_RNDOUTSTART << 8) | NAND_CMD_RNDOUT);
	atxx_nd_write_reg(REG_NFC_FADDR0, 0);
	atxx_nd_write_reg(REG_NFC_BADDR, 0);

	if (ecc_enable) {
		cmd = (NFC_CMD_UDFA_READ << NFC_CMD_CMD_SHIFT)
			| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
			| (data_len << NFC_CMD_DATA_LEN_SHIFT)
			| NFC_CMD_ECC | NFC_CMD_ENABLE;
	} else {
		cmd = (NFC_CMD_UDFA_READ << NFC_CMD_CMD_SHIFT)
			| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
			| (data_len << NFC_CMD_DATA_LEN_SHIFT)
			| NFC_CMD_ENABLE;
	}
	atxx_nd_write_reg(REG_NFC_CMD, cmd);
}

static  void nfc_send_readpage_ecc_cmd(u32 data_len, u32 addr_len)
{
	uint32_t cmd;

	cmd = (NFC_CMD_READ << NFC_CMD_CMD_SHIFT)
		| (data_len << NFC_CMD_DATA_LEN_SHIFT)
		| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
		| NFC_CMD_WAIT_BUSY | NFC_CMD_ECC | NFC_CMD_ENABLE;

	atxx_nd_write_reg(REG_NFC_CMD, cmd);
	WAIT_CMD_END();
}

/******************** cmd for write operation **********************/
static  void nfc_config_small_page_write(int width, int page)
{
	int reg_data;

	reg_data = NAND_CMD_SEQIN | (NAND_CMD_PAGEPROG << 8)
		| (NAND_CMD_READOOB << 16);
	atxx_nd_write_reg(REG_NFC_CCFG, reg_data);

	if (width == NAND_BUSWIDTH_16) {
		atxx_nd_write_reg(REG_NFC_FADDR0, (page << 8) / 2);
		atxx_nd_write_reg(REG_NFC_BADDR, (page << 8) / 2);
	} else {
		atxx_nd_write_reg(REG_NFC_FADDR0, page << 8);
	}
}

static  void nfc_send_write_cmd(u32 data_len, u32 addr_len)
{
	uint32_t cmd;
	cmd = (NFC_CMD_PAGE_PROGRAM << NFC_CMD_CMD_SHIFT)
		| (data_len << NFC_CMD_DATA_LEN_SHIFT)
		| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
		| NFC_CMD_WAIT_BUSY | NFC_CMD_INT | NFC_CMD_ENABLE;

	atxx_nd_write_reg(REG_NFC_CMD, cmd);
}

static  void nfc_send_write_ecc_cmd(u32 data_len, u32 addr_len)
{
	uint32_t cmd;
	cmd = (NFC_CMD_PAGE_PROGRAM << NFC_CMD_CMD_SHIFT)
		| (data_len << NFC_CMD_DATA_LEN_SHIFT)
		| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
		| NFC_CMD_WAIT_BUSY | NFC_CMD_ECC | NFC_CMD_INT | NFC_CMD_ENABLE;

	atxx_nd_write_reg(REG_NFC_CMD, cmd);
}

static  void nfc_send_write_udf_cmd(u32 data_len, u32 addr_len)
{
	uint32_t cmd;
	cmd = (NFC_CMD_SPECIAL_UDFA_PROG_BUFFER << NFC_CMD_CMD_SHIFT)
		| (data_len << NFC_CMD_DATA_LEN_SHIFT)
		| (addr_len << NFC_CMD_ADDR_LEN_SHIFT)
		| NFC_CMD_WAIT_BUSY | NFC_CMD_ENABLE;

	atxx_nd_write_reg(REG_NFC_CMD, cmd);
	WAIT_CMD_END();
}

#if 0
/*
 * Define partitions for flash device
 * This only for default partition.
 * u can set real partitions from cmdline
 */
 #include <linux/mtd/partitions.h>
static struct mtd_partition partition_info[] = {
	{
	 .name = "boot",
	 .offset = 0,
	 .size = 32 * SZ_1M,
	 /* .mask_flags: MTD_WRITEABLE, *//* force read-only */
	 },
	{
	 .name = "sysfs",
	 .offset = MTDPART_OFS_APPEND,
	 .size = MTDPART_SIZ_FULL }
};
#endif
/***********************************************************
*   Nand dma and interrupt interface
***********************************************************/


#ifdef CONFIG_ATXX_NAND_DMA
static void atxx_nd_dma_write(struct nand_chip *chip, uint8_t * addr,
		int len, int wait_ecc)
{
	uint32_t m_dma;

	/*
	* NFC DMA require source address and destination address
	* to be 64 bits align
	*/
	m_dma = (uint32_t)addr;
	flush_cache(0, 0);

	NFC_WAIT_FIFO_EMPTY();
	atxx_nd_write_reg(REG_NFC_DMA_SAR, m_dma);
	atxx_nd_write_reg(REG_NFC_DMA_DAR, NFC_FIFO_ADDR);
	atxx_nd_write_reg(REG_NFC_DMA_CTRL, 
			(len << NFC_DMA_LEN) | NFC_DMA_RW | NFC_DMA_ENABLE);

	WAIT_DMA_FINI();

	if (wait_ecc)
		WAIT_ENCODE_READY();

}

static void atxx_nd_dma_read(struct nand_chip *chip, uint8_t * addr,
		int read_buf_off, int len)
{
	uint32_t m_dma;

	m_dma = (uint32_t)addr;
	flush_cache(0, 0);
	atxx_nd_write_reg(REG_NFC_DMA_SAR, NFC_READ_BUF_ADDR + read_buf_off);
	atxx_nd_write_reg(REG_NFC_DMA_DAR, m_dma);
	atxx_nd_write_reg(REG_NFC_DMA_CTRL, 
			((len << NFC_DMA_LEN) | NFC_DMA_ENABLE) & (~NFC_DMA_RW));
	WAIT_DMA_FINI();

}
#endif

/***********************************************************
*   Nand base operation
***********************************************************/
/* atxx_nd_ready -return R/nB
 *
 * @mtd:	MTD device structure
 * there are 4 cs/RB in atxx NFC 
 * returns 0 if the nand is busy, 1 if it is ready
 */
static int atxx_nd_ready(struct mtd_info *mtd)
{
	int rb_bit, para0;

	para0 = atxx_nd_read_reg(REG_NFC_PARA0);

	if (para0 & NFC_PARA0_BANK_CE0)
		rb_bit = NFC_STATUS_RBB0;
	else if (para0 & NFC_PARA0_BANK_CE1)
		rb_bit = NFC_STATUS_RBB1;
	else if (para0 & NFC_PARA0_BANK_CE2)
		rb_bit = NFC_STATUS_RBB2;
	else
		rb_bit = NFC_STATUS_RBB3;

	return (atxx_nd_read_reg(REG_NFC_STATUS) & rb_bit) ? 1 : 0;
}

/**
 * atxx_nd_select_chip - control -CE line
 * @mtd:	MTD device structure
 * @chip:	chipnumber to select, -1 for deselect
 */
static void atxx_nd_select_chip(struct mtd_info *mtd, int chip)
{
	int para0;

	switch (chip) {
#if (BOARD == ad8071)
	case 0:
		/* select bank 0 chip */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE1 |
			       NFC_PARA0_BANK_CE2 | NFC_PARA0_BANK_CE3);
		para0 |= NFC_PARA0_BANK_CE0;
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
	case 1:
		/* select bank 1 chip */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE0 |
			       NFC_PARA0_BANK_CE1 | NFC_PARA0_BANK_CE3);
		para0 |= NFC_PARA0_BANK_CE2;
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
	default:
		/* de-select all */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE0 | NFC_PARA0_BANK_CE1
			       | NFC_PARA0_BANK_CE2 | NFC_PARA0_BANK_CE3);
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
#else
	case 0:
		/* select bank 0 chip */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE1 |
			       NFC_PARA0_BANK_CE2 | NFC_PARA0_BANK_CE3);
		para0 |= NFC_PARA0_BANK_CE0;
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
	case 1:
		/* select bank 1 chip */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE0 |
			       NFC_PARA0_BANK_CE2 | NFC_PARA0_BANK_CE3);
		para0 |= NFC_PARA0_BANK_CE1;
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
	case 2:
		/* select bank 2 chip */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE0 |
			       NFC_PARA0_BANK_CE1 | NFC_PARA0_BANK_CE3);
		para0 |= NFC_PARA0_BANK_CE2;
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
	case 3:
		/* select bank 3 chip */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE0 | NFC_PARA0_BANK_CE1 |
			       NFC_PARA0_BANK_CE2);
		para0 |= NFC_PARA0_BANK_CE3;
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
	case -1:
		/* de-select all */
		para0 = atxx_nd_read_reg(REG_NFC_PARA0);
		para0 &= ~(NFC_PARA0_BANK_CE0 | NFC_PARA0_BANK_CE1
			       | NFC_PARA0_BANK_CE2 | NFC_PARA0_BANK_CE3);
		atxx_nd_write_reg(REG_NFC_PARA0, para0);
		break;
#endif
	}
}

static int atxx_nd_wait(struct mtd_info *mtd, struct nand_chip *chip)
{
	uint32_t status;

	nfc_send_readstatus_cmd();
	status = atxx_nd_read_reg(REG_NFC_STATUS) & 0xff;
	atxx_nd_debug("status:%x\n", status);

	return status;
}

/***********************************************************
*   Nand read operation
***********************************************************/
/**
 * atxx_nd_read_oob_std - [REPLACABLE] the most common OOB data read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @page:	page number to read
 * @sndcmd:	flag whether to issue read command or not
 */
static int atxx_nd_read_oob_std(struct mtd_info *mtd, struct nand_chip
		*chip, int page, int sndcmd)
{
	int addr_len;

	//atxx_nd_debug("atxx_nd_read_oob_std - page:%08x\n", page);
	if (mtd->writesize <= 512) {	/* small page */
		nfc_config_small_page_read(bus_width, page);
		if (chip->chipsize > (32 << 20)) {
			addr_len = 4;
		} else {
			addr_len = 3;
		}
		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readoob_udf_cmd(mtd->oobsize / 2, addr_len);
		} else {
			nfc_send_readoob_udf_cmd(mtd->oobsize, addr_len);
		}
	} else {
		/* Write NFC_FADDR0, NFC_FADDR1 with desired page address */
		nfc_config_big_page(bus_width, mtd->writesize | (page << 16));

		if (chip->chipsize > (128 << 20)) {
			addr_len = 5;
			atxx_nd_write_reg(REG_NFC_FADDR1, page >> 16);
		} else {
			addr_len = 4;
		}
		/* read oob */
		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readoob_cmd(mtd->oobsize / 2, addr_len);
		} else {
			nfc_send_readoob_cmd(mtd->oobsize, addr_len);
		}
	}

	memcpy((void *)chip->oob_poi,
			(void *)(NFC_READ_BUF_ADDR + mtd->writesize),
			mtd->oobsize);

	return 0;
}

/**
 * atxx_nd_read_page_raw - [Intern] atxx page read function(raw mode)
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 */
static int atxx_nd_read_page_raw(struct mtd_info *mtd, struct
		nand_chip *chip, uint8_t * buf, int page)
{
	int addr_len;
	//struct atxx_nd *hw = (struct atxx_nd *)chip->priv;

	atxx_nd_debug("atxx_nd_read_page_raw - page:%08x\n", page);

	if (mtd->writesize <= 512) {	/* small page */
		nfc_config_small_page_read(bus_width, page);
		if (chip->chipsize > (32 << 20)) {
			addr_len = 4;
		} else {
			addr_len = 3;
		}
		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readoob_udf_cmd(mtd->oobsize / 2, addr_len);
		} else {
			nfc_send_readoob_udf_cmd(mtd->oobsize, addr_len);
		}

		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readpage_cmd(mtd->writesize / 2, addr_len);
		} else {
			nfc_send_readpage_cmd(mtd->writesize, addr_len);
		}
	} else {
		/* Write NFC_FADDR0, NFC_FADDR1 with desired page address */
		nfc_config_big_page(bus_width, mtd->writesize | (page << 16));

		if (chip->chipsize > (128 << 20)) {
			addr_len = 5;
			atxx_nd_write_reg(REG_NFC_FADDR1, page >> 16);
		} else {
			addr_len = 4;
		}

		/* read oob */
		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readoob_cmd(mtd->oobsize / 2, addr_len);
		} else {
			nfc_send_readoob_cmd(mtd->oobsize, addr_len);
		}

		/* read data, like 2048 bytes */
		/* Write UDFA cmd1 = 0x05, UDFA cmd2 = 0xE0 */

		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readpage_udf_cmd(mtd->writesize / 2, 2, 0);
		} else {
			nfc_send_readpage_udf_cmd(mtd->writesize, 2, 0);
		}

		WAIT_CMD_END();
	}

	/* for verify, we can get data in readbuf, (buf=null) */
	if (buf) {
#ifndef CONFIG_ATXX_NAND_DMA
		memcpy((uint8_t *) buf, (uint8_t *) NFC_READ_BUF,
		       mtd->writesize);
#else
		atxx_nd_dma_read(chip, buf, 0, mtd->writesize);
#endif
		memcpy(chip->oob_poi,
		       (uint8_t *) NFC_READ_BUF + mtd->writesize, mtd->oobsize);
	}

	return 0;
}

/**
 * atxx_nd_read_page_hwecc - [Intern] atxx page read function(hardware ecc)
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 */
static int atxx_nd_read_page_hwecc(struct mtd_info *mtd, struct
		nand_chip *chip, uint8_t * buf, int page)
{
	int addr_len, status1, status2;
#ifdef CONFIG_ATXX_NAND_DEBUG
	uint8_t ecc_buf[3 * NAND_MAX_OOBSIZE + 10];
	int ecc_len;
	uint8_t *oob;
	int i;
#endif

	atxx_nd_debug("atxx_nd_read_page_hwecc - page:%08x,buf:%08x\n", 
				page, (int)buf);

	if (mtd->writesize <= 512) {	/* small page */
		nfc_config_small_page_read(bus_width, page);
		if (chip->chipsize > (32 << 20)) {
			addr_len = 4;
		} else {
			addr_len = 3;
		}

		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readoob_udf_cmd(mtd->oobsize / 2, addr_len);
		} else {
			nfc_send_readoob_udf_cmd(mtd->oobsize, addr_len);
		}

		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readpage_ecc_cmd(mtd->writesize / 2, addr_len);
		} else {
			nfc_send_readpage_ecc_cmd(mtd->writesize, addr_len);
		}

		/* ecc status */
		status1 = atxx_nd_read_reg(REG_NFC_ECCSTATUS1);
		if (status1 & NFC_ECC_UNCORRECT1)
			mtd->ecc_stats.failed++;
		else
			mtd->ecc_stats.corrected +=
				((status1 & NFC_ECC_CORRECTED_MASK1)
				 >> NFC_ECC_CORRECTED_SHIFT1);

		/* for verify, we can get data in readbuf, (buf=null) */
		if (buf) {
#ifndef CONFIG_ATXX_NAND_DMA
			memcpy((uint8_t *) buf, (uint8_t *) NFC_READ_BUF,
					mtd->writesize);
#else
			atxx_nd_dma_read(chip, buf, 0, mtd->writesize);
#endif
			memcpy(chip->oob_poi,
				(uint8_t *) NFC_READ_BUF + mtd->writesize,
				mtd->oobsize);
		}
	} else {
		/* clear RSD ready for each code */
		atxx_nd_write_reg(REG_NFC_INT_CODE, 0xff00);
		nfc_config_big_page(bus_width, mtd->writesize | (page << 16));

		if (chip->chipsize > (128 << 20)) {
			addr_len = 5;
			atxx_nd_write_reg(REG_NFC_FADDR1, page >> 16);
		} else {
			addr_len = 4;
		}

		/* read oob */
		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readoob_cmd(mtd->oobsize / 2, addr_len);
		} else {
			nfc_send_readoob_cmd(mtd->oobsize, addr_len);
		}
#ifdef CONFIG_ATXX_NAND_DEBUG
		ecc_len = 0;
		oob = (uint8_t *) NFC_READ_BUF + mtd->writesize;
		for (i = 0; i < mtd->oobsize; i++) {
			ecc_len += sprintf((char *)(ecc_buf + ecc_len), "%02x ",
					oob[i]);
		}
		ecc_buf[ecc_len] = '\0';
		atxx_nd_debug("page:%08x, oob data:%s\n", page, ecc_buf);

		oob = (uint8_t *) NFC_READ_BUF + mtd->writesize;
		atxx_nd_debug("page:%08x, oob data:", page);

		for (i = 0; i < mtd->oobsize; i++) {
			printf("%02x ",oob[i]);
		}
		printf("\n");
#endif
		/* read data, like 2048 bytes */
		/* Write UDFA cmd1 = 0x05, UDFA cmd2 = 0xE0 */
		if (bus_width == NAND_BUSWIDTH_16) {
			nfc_send_readpage_udf_cmd(mtd->writesize / 2, 2, 1);
		} else {
			nfc_send_readpage_udf_cmd(mtd->writesize, 2, 1);
		}

		if (buf) {
#ifndef CONFIG_ATXX_NAND_DMA
			WAIT_CMD_END();
			memcpy((uint8_t *) buf, (uint8_t *) NFC_READ_BUF,
					mtd->writesize);
#else
			/*using code word check only for 4k pagesize 
			  to improve read speed */
			if (mtd->writesize == 4096) {
				WAIT_DECODE_CODE_READY(4);
				atxx_nd_dma_read(chip, buf, 0, 0x800);

				WAIT_CMD_END();
				atxx_nd_dma_read(chip, buf + 0x800, 0x800,
						0x800);

			} else {
				WAIT_CMD_END();
				atxx_nd_dma_read(chip, buf, 0, mtd->writesize);
			}
#endif
			memcpy(chip->oob_poi,
					(uint8_t *) NFC_READ_BUF + mtd->writesize,
					mtd->oobsize);
		} else {
			WAIT_CMD_END();
		}
	}

	status1 = atxx_nd_read_reg(REG_NFC_ECCSTATUS1);
	status2 = atxx_nd_read_reg(REG_NFC_ECCSTATUS2);
	if (mtd->writesize == 4096) {	
		/* ecc status */
		if ((status1 & (NFC_ECC_UNCORRECT1 | NFC_ECC_UNCORRECT2 |
				NFC_ECC_UNCORRECT3 | NFC_ECC_UNCORRECT4)) |
			(status2 & (NFC_ECC_UNCORRECT1 | NFC_ECC_UNCORRECT2 |
				    NFC_ECC_UNCORRECT3 | NFC_ECC_UNCORRECT4))) {
			atxx_nd_err("nand ecc error, page:%08x, ecc status1:%08x"
					"ecc status2:%08x\n",
					page, status1, status2);
			mtd->ecc_stats.failed++;
		} else
			mtd->ecc_stats.corrected +=
				((status1 & NFC_ECC_CORRECTED_MASK1)
				 >> NFC_ECC_CORRECTED_SHIFT1) +
				((status1 & NFC_ECC_CORRECTED_MASK2)
				 >> NFC_ECC_CORRECTED_SHIFT2) +
				((status1 & NFC_ECC_CORRECTED_MASK3)
				 >> NFC_ECC_CORRECTED_SHIFT3) +
				((status1 & NFC_ECC_CORRECTED_MASK4)
				 >> NFC_ECC_CORRECTED_SHIFT4) + 
				((status2 & NFC_ECC_CORRECTED_MASK1)
				 >> NFC_ECC_CORRECTED_SHIFT1) +
				((status2 & NFC_ECC_CORRECTED_MASK2)
				 >> NFC_ECC_CORRECTED_SHIFT2) +
				((status2 & NFC_ECC_CORRECTED_MASK3)
				 >> NFC_ECC_CORRECTED_SHIFT3) +
				((status2 & NFC_ECC_CORRECTED_MASK4)
				 >> NFC_ECC_CORRECTED_SHIFT4);
	} else if (mtd->writesize == 2048) {	
		/* ecc status */
		if (status1 & (NFC_ECC_UNCORRECT1 | NFC_ECC_UNCORRECT2 |
				NFC_ECC_UNCORRECT3 | NFC_ECC_UNCORRECT4)) {
			atxx_nd_err("nand ecc error, page:%08x, ecc status:%08x\n",
					page, status1);
			mtd->ecc_stats.failed++;
		} else
			mtd->ecc_stats.corrected +=
				((status1 & NFC_ECC_CORRECTED_MASK1)
				 >> NFC_ECC_CORRECTED_SHIFT1) +
				((status1 & NFC_ECC_CORRECTED_MASK2)
				 >> NFC_ECC_CORRECTED_SHIFT2) +
				((status1 & NFC_ECC_CORRECTED_MASK3)
				 >> NFC_ECC_CORRECTED_SHIFT3) +
				((status1 & NFC_ECC_CORRECTED_MASK4)
				 >> NFC_ECC_CORRECTED_SHIFT4);
	} else {
		if (status1 & NFC_ECC_UNCORRECT1) {
			atxx_nd_err("nand ecc error, page:%08x, ecc status:%08x\n",
					page, status1);
			mtd->ecc_stats.failed++;
		} else
			mtd->ecc_stats.corrected +=
				((status1 & NFC_ECC_CORRECTED_MASK1)
				 >> NFC_ECC_CORRECTED_SHIFT1);
	}

	return 0;
}

/***********************************************************
*   Nand write operation
***********************************************************/
/**
 * atxx_nd_write_oob_std - [REPLACABLE]atxx OOB data write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @page:	page number to write
 */
static int atxx_nd_write_oob_std(struct mtd_info *mtd, struct nand_chip
				     *chip, int page)
{
	int *p = (int *)chip->oob_poi;
	int addr_len, reg_data;
	int i;
	int ret;
	
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	char buf[NAND_MAX_OOBSIZE];
#endif

	atxx_nd_debug("atxx_nd_write_oob_std - page:%08x\n", page);

	if (mtd->writesize <= 512) {
		nfc_config_small_page_write(bus_width, page);
		if (chip->chipsize > (32 << 20))
			addr_len = 4;
		else
			addr_len = 3;

		/* 0x50 command */
		reg_data = (NFC_CMD_UDF3_CYCLE << NFC_CMD_CMD_SHIFT)
		    | NFC_CMD_ENABLE;
		atxx_nd_write_reg(REG_NFC_CMD, reg_data);
		WAIT_CMD_END();
	} else {
		nfc_config_big_page(bus_width, mtd->writesize | (page << 16));
		if (chip->chipsize > (128 << 20)) {
			addr_len = 5;
			atxx_nd_write_reg(REG_NFC_FADDR1, page >> 16);
		} else {
			addr_len = 4;
		}
	}

	/* page program */
	if (bus_width == NAND_BUSWIDTH_16) {
		nfc_send_write_cmd(mtd->oobsize / 2, addr_len);
	} else {
		nfc_send_write_cmd(mtd->oobsize, addr_len);
	}
	
	/* write data to fifo and wait command finished */
	for (i = 0; i < mtd->oobsize / 4; i++) {
		NFC_WAIT_FIFO();
		atxx_nd_write_reg(REG_NFC_FIFO, p[i]);
	}
	WAIT_CMD_END();

	nfc_send_readstatus_cmd();

	if (!(atxx_nd_read_reg(REG_NFC_STATUS) & 0x40)) {
		printf("ERR: nand write oob ready before status ok.");
	}

	ret = (atxx_nd_read_reg(REG_NFC_STATUS) & NAND_STATUS_FAIL) ? -EIO : 0;
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	if (ret == 0) {
		chip->oob_poi = (uint8_t *)buf;
		atxx_nd_read_oob_std(mtd, chip, page, 0);
		for (i = 0; i < mtd->oobsize / 4; i++) {
			if (p[i] != *(uint32_t *) (buf + 4 * i)) {
				printf("nand write oob verify"
					" failed, page:%08x, i:%d, w:%08x, r:%08x\n",
					   page, i, p[i],
					   *(uint32_t *) (buf + 4 * i));
				ret = -EIO;
				break;
			}
		}

		if (i < mtd->oobsize / 4) {
			printf("OOB data comparision----------------\n");

			for (i = 0; i < mtd->oobsize / 4; i += 4) {
				printf("w:%08x %08x %08x %08x\n",
					   p[i], p[i + 1], p[i + 2], p[i + 3]);
				printf("R:%08x %08x %08x %08x\n",
					   *(uint32_t *) (buf + 4 * i),
					   *(uint32_t *) (buf + 4 * (i + 1)),
					   *(uint32_t *) (buf + 4 * (i + 2)),
					   *(uint32_t *) (buf + 4 * (i + 3)));
			}
		}
		chip->oob_poi = (uint8_t *)p;
	} else
		printf("nand write oob failed, page:%08x\n", page);
#endif
	return ret;
}
#if 0
/**
 * atxx_nd_write_page_raw - {REPLACABLE] raw page write function for atxx
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 */
static void atxx_nd_write_page_raw(struct mtd_info *mtd, struct
		nand_chip *chip, const uint8_t * buf,
		uint32_t page)
{
	int i;
	int addr_len;
	struct atxx_nd *hw = (struct atxx_nd *)chip->priv;
	uint32_t *p = (uint32_t *) buf;

	atxx_nd_debug("atxx_nd_write_page_raw - page:%08x\n", page);

	atxx_nd_irq_enable(NFC_INT_CMD_MASK);

	if (mtd->writesize <= 512) {	/* small page */
		nfc_config_small_page_write(bus_width, page);
		if (chip->chipsize > (32 << 20))
			addr_len = 4;
		else
			addr_len = 3;
	} else {
		/* setup column and page address */
		nfc_config_big_page(bus_width, page << 16);
		if (chip->chipsize > (128 << 20)) {
			addr_len = 5;
			atxx_nd_write_reg(REG_NFC_FADDR1, page >> 16);
		} else {
			addr_len = 4;
		}
	}

	/* send comand for writing page data and spare */
	if (bus_width == NAND_BUSWIDTH_16) {
		nfc_send_write_cmd((mtd->writesize + mtd->oobsize) / 2, addr_len);
	} else {
		nfc_send_write_cmd(mtd->writesize + mtd->oobsize, addr_len);
	}

	/* write page data and oob, wait command finished */
#ifndef CONFIG_ATXX_NAND_DMA
	for (i = 0; i < mtd->writesize / 4; i++) {
		NFC_WAIT_FIFO();
		atxx_nd_write_reg(REG_NFC_FIFO, p[i]);
	}
#else
	/* write one page by one time */
	atxx_nd_dma_write(chip, (void *)buf, mtd->writesize, 0);
#endif

	p = (uint32_t *) chip->oob_poi;
	for (i = 0; i < mtd->oobsize / 4; i++) {
		NFC_WAIT_FIFO();
		atxx_nd_write_reg(REG_NFC_FIFO, p[i]);
	}

	if (!wait_for_completion_timeout(&hw->cmd_completion,
				msecs_to_jiffies(NFC_TIMEOUT_JIFFIS)))
	{
		printf(
				"wait cmd finish timeout!");
	}

	atxx_nd_irq_disable(NFC_INT_CMD_MASK);
}
#endif
/**
 * atxx_nd_write_page_hwecc - 
 * {REPLACABLE] hardware ecc based page write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 */
static void atxx_nd_write_page_hwecc(struct mtd_info *mtd, struct
		nand_chip *chip, const uint8_t * buf,
		uint32_t page)
{
	int i, j;
	int addr_len;

	unsigned int *p = (unsigned int *) buf;
	unsigned char *oob = chip->oob_poi;
	unsigned char *ecc = (uint8_t *) NFC_READ_BUF + mtd->writesize + mtd->oobsize;
	unsigned int *eccpos = chip->ecc.layout->eccpos;

	atxx_nd_debug("atxx_nd_write_page_hwecc - page:%08x\n", page);

	if (mtd->writesize <= 512) {	/* small page */
		nfc_config_small_page_write(bus_width, page);
		if (chip->chipsize > (32 << 20))
			addr_len = 4;
		else
			addr_len = 3;
	} else {
		/* setup column and page address */
		nfc_config_big_page(bus_width, page << 16);

		if (chip->chipsize > (128 << 20)) {
			addr_len = 5;
			atxx_nd_write_reg(REG_NFC_FADDR1, page >> 16);
		} else {
			addr_len = 4;
		}
	}

	if (bus_width == NAND_BUSWIDTH_16) {
		nfc_send_write_ecc_cmd((mtd->writesize + mtd->oobsize) / 2, addr_len);
	} else {
		nfc_send_write_ecc_cmd(mtd->writesize + mtd->oobsize, addr_len);
	}

	/* write page data and oob */
#ifndef CONFIG_ATXX_NAND_DMA
	/* 512 bytes every step */
	for (i = 0; i < chip->ecc.steps; i++) {
		for (j = 0; j < chip->ecc.size;) {
			NFC_WAIT_FIFO();
			atxx_nd_write_reg(REG_NFC_FIFO, 
				p[(i * chip->ecc.size + j) / 4]);
			j += 4;
		}
		WAIT_ENCODE_READY();
	}
#else
	atxx_nd_dma_write(chip, (void *)buf, mtd->writesize, 1);
#endif

	/* write spare to fifo */

	for (i = 0; i < chip->ecc.steps; i++) {
		for (j = 0; j < chip->ecc.bytes; j++) {
			oob[eccpos[chip->ecc.bytes * i + j]] =
				ecc[((mtd->oobsize / chip->ecc.steps + 15) / 16)
				* 0x10 * i + j];
		}
	}
#ifdef CONFIG_ATXX_NAND_DEBUG
	printf("page:%08x, written ecc bytes: ", page);
	for (i = 0; i < chip->ecc.steps; i++) {
		for (j = 0; j < chip->ecc.bytes; j++) {
			printf("%02x ", ecc[((mtd->oobsize / chip->ecc.steps + 15) / 16)* 0x10 * i + j]);
		}
	}
	printf("\n");
#endif
	p = (unsigned int *) oob;
	for (i = 0; i < mtd->oobsize / 4; i++) {
		NFC_WAIT_FIFO();
		atxx_nd_write_reg(REG_NFC_FIFO, p[i]);
	}
#ifdef CONFIG_ATXX_NAND_DEBUG
	printf("page:%08x, oob data: ", page);
	for (i = 0; i < mtd->oobsize; i++) {
		printf("%02x ", oob[i]);
	}
	printf("\n");
#endif

	WAIT_CMD_END();

}

/**
 * atxx_nd_write_page - [INTERNAL] write one page
 * @mtd:	MTD device structure
 * @chip:	NAND chip descriptor
 * @buf:	the data to write
 * @page:	page number to write
 * @cached:	cached programming
 */
static int atxx_nd_write_page(struct mtd_info *mtd, struct nand_chip *chip,
				  const uint8_t * buf, int page,
				   int cached, int raw)
{
	int status;

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	int *p = (uint32_t *) buf;
	int i;
	int len = mtd->writesize / 4;
	int *read_data;
	read_data = (int *)NFC_READ_BUF;
#endif

	//if (unlikely(raw))
		/* write page in raw mode(without ecc) */
	//	atxx_nd_write_page_raw(mtd, chip, buf, page);
	//else
		/* write page data with hardware ecc */
		atxx_nd_write_page_hwecc(mtd, chip, buf, page);

	/* success/fail? */
	nfc_send_readstatus_cmd();

	if (!(atxx_nd_read_reg(REG_NFC_STATUS) & 0x40)) {
		printf("ERR: nand write oob ready" "before status ok.");
	}
	status = atxx_nd_read_reg(REG_NFC_STATUS);
	if ((status & NAND_STATUS_FAIL) && (chip->errstat))
		status = chip->errstat(mtd, chip, FL_WRITING, status, page);
	if (status & NAND_STATUS_FAIL) {
		printf("nand write page failed, page:%08x\n", page);
		return -EIO;
	}
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	//if (unlikely(raw))
	//	atxx_nd_read_page_raw(mtd, chip, NULL, page);
	//else
		atxx_nd_read_page_hwecc(mtd, chip, NULL, page);

	for (i = 0; i < len; i++) {
		if (p[i] != read_data[i]) {
			printf("nand write page verify failed"
				   ", page:%08x, i:%d, w:%08x, r:%08x\n",
				   page, i, p[i], read_data[i]);
			printf("Page data comparision----------------\n");
			for (i = 0; i < len; i += 8) {
				printf("w:%08x %08x %08x %08x %08x %08x %08x %08x\n",
					   p[i], p[i + 1], p[i + 2], p[i + 3],
					   p[i + 4], p[i + 5], p[i + 6],
					   p[i + 7]);
				printf("R:%08x %08x %08x %08x %08x %08x %08x %08x\n",
					   read_data[i], read_data[i + 1],
					   read_data[i + 2], read_data[i + 3],
					   read_data[i + 4], read_data[i + 5],
					   read_data[i + 6], read_data[i + 7]);
			}

			return -EIO;
		}
	}
#endif

	return 0;
}

/***********************************************************
*   Nand erase operation
***********************************************************/
/**
* atxx__erease_cmd - NAND standard block erase command function
* @mtd:        MTD device structure
* @page:       the page address of the block which will be erased
*
* Standard erase command for NAND chips
*/
static void atxx_nd_erase(struct mtd_info *mtd, int page)
{
	struct nand_chip *chip = mtd->priv;
	int cmd;

	atxx_nd_debug("atxx_erase - page:%08x\n", page);

	if (mtd->writesize <= 512) {
		if (chip->chipsize > (32 << 20)) {
			atxx_nd_write_reg(REG_NFC_FADDR0, page);

			cmd = (NFC_CMD_BLOCK_ERASE << NFC_CMD_CMD_SHIFT) |
				(0 << NFC_CMD_DATA_LEN_SHIFT) |
				(3 << NFC_CMD_ADDR_LEN_SHIFT) |
				NFC_CMD_WAIT_BUSY | NFC_CMD_ENABLE;
			atxx_nd_write_reg(REG_NFC_CMD, cmd);
			WAIT_CMD_END();
		} else {
			atxx_nd_write_reg(REG_NFC_FADDR0, page);
			cmd = (NFC_CMD_BLOCK_ERASE << NFC_CMD_CMD_SHIFT) |
				(0 << NFC_CMD_DATA_LEN_SHIFT) |
				(2 << NFC_CMD_ADDR_LEN_SHIFT) |
				NFC_CMD_WAIT_BUSY | NFC_CMD_ENABLE;
			atxx_nd_write_reg(REG_NFC_CMD, cmd);

			WAIT_CMD_END();
		}
	} else {
		if (chip->chipsize > (128 << 20)) {
			atxx_nd_write_reg(REG_NFC_FADDR0, page);
			cmd = (NFC_CMD_BLOCK_ERASE << NFC_CMD_CMD_SHIFT) |
				(0 << NFC_CMD_DATA_LEN_SHIFT) |
				(3 << NFC_CMD_ADDR_LEN_SHIFT) |
				NFC_CMD_WAIT_BUSY | NFC_CMD_ENABLE;
			atxx_nd_write_reg(REG_NFC_CMD, cmd);

			WAIT_CMD_END();
		} else {
			atxx_nd_write_reg(REG_NFC_FADDR0, page);
			cmd = (NFC_CMD_BLOCK_ERASE << NFC_CMD_CMD_SHIFT) |
				(0 << NFC_CMD_DATA_LEN_SHIFT) |
				(2 << NFC_CMD_ADDR_LEN_SHIFT) |
				NFC_CMD_WAIT_BUSY | NFC_CMD_ENABLE;
			atxx_nd_write_reg(REG_NFC_CMD, cmd);

			WAIT_CMD_END();
		}
	}

}


/***********************************************************
*   Nand initialize operation
***********************************************************/
void atxx_nd_read_ids(int * first_id_byte,
		int * second_id_byte, int * fourth_id_byte)
{
	uint32_t *p = (uint32_t *) ((uint8_t *) NFC_READ_BUF + ID_DATA_ADDR);

	nfc_send_readid_cmd();

	/* we are interested in 1st, 2nd and 4th id bytes */
	*first_id_byte = *p & 0xff;
	*second_id_byte = (*p >> 8) & 0xff;
	*fourth_id_byte = (*p >> 24) & 0xff;

	atxx_nd_debug("ID:%08x\n", *p);
}

/*******************************************
*  Bit0~3           PAGE_SIZE           BLOCK_SIZE                               
*  0000 :            8192                  8192KB
*  0001              4096                   256KB                       
*  0010              4096                   512KB                       
*  0100              4096                  1024KB                        
*  1000              2048                   256KB                       
*  0011              2048                   128KB                       
*  0110              2048                   512KB                       
*  1100              2048                    64KB                       
*  1010              2048                  1024KB
*  1001              4096                  2048KB
*  0101              4096                  4096KB
*  0111              8192                  2048KB
*  1011              8192                  4096KB
*  1101              8192                   256KB                        
*  1110              8192                   512KB                        
*  1111              8192                  1024KB                        
*
*  bit 4~6           ecc bit             oob_size_per_sector    sector size
*  000                 4                      16                     512
*  001                10                      26                     512
*  010                 8                      22                     512
*  100                12                      30                    1024
*  111                22                      56                    1024
*  110                 0                      16                     512
*  101                20                      52                    1024
*  011                24                      64                    1024
*
*  bit 7             Bus Width
*  0                    8
*  1                    16 
*  
********************************************/
static uint32_t atxx_nd_identify(struct mtd_info *mtd)
{  
	uint32_t io_data, value, rt = 0;
	 
	io_data = atxx_nd_read_reg(REG_NFC_SOFT_PIN2);
	printf("nand io-data: 0x%08x\n", io_data);

	/*identify pagesize/blocksize.*/
	value = io_data & SIZE_BITS;
	switch (value)
	{
		case 0:
			mtd->writesize = PAGE_SIZE_8K;
			mtd->erasesize = 8192 * 1024;
			break;
		case 1:
			mtd->writesize = PAGE_SIZE_4K;
			mtd->erasesize = 256 * 1024;
			break;
		case 2:
			mtd->writesize = PAGE_SIZE_4K;
			mtd->erasesize = 512 * 1024;
			break;
		case 3:
			mtd->writesize = PAGE_SIZE_2K;
			mtd->erasesize = 128 * 1024;
			break;
		case 4:
			mtd->writesize = PAGE_SIZE_4K;
			mtd->erasesize = 1024 * 1024;
			break;
		case 5:
			mtd->writesize = PAGE_SIZE_4K;
			mtd->erasesize = 4096 * 1024;
			break;
		case 6:
			mtd->writesize = PAGE_SIZE_2K;
			mtd->erasesize = 512 * 1024;
			break;
		case 7:
			mtd->writesize = PAGE_SIZE_8K;
			mtd->erasesize = 2048 * 1024;
			break;
		case 8:
			mtd->writesize = PAGE_SIZE_2K;
			mtd->erasesize = 256 * 1024;
			break;
		case 9:
			mtd->writesize = PAGE_SIZE_4K;
			mtd->erasesize = 2048 * 1024;
			break;
		case 10:
			mtd->writesize = PAGE_SIZE_2K;
			mtd->erasesize = 1024 * 1024;
			break;
		case 11:
			mtd->writesize = PAGE_SIZE_8K;
			mtd->erasesize = 4096 * 1024;
			break;
		case 12:
			mtd->writesize = PAGE_SIZE_2K;
			mtd->erasesize = 64 * 1024;
			break;
		case 13:
			mtd->writesize = PAGE_SIZE_8K;
			mtd->erasesize = 256 * 1024;
			break;
		case 14:
			mtd->writesize = PAGE_SIZE_8K;
			mtd->erasesize = 512 * 1024;
			break;
		case 15:
			mtd->writesize = PAGE_SIZE_8K;
			mtd->erasesize = 1024 * 1024;
			break;
		default:
			break;
	}
		
	/*identify ecc and oob by nand data pin.*/
	value = (io_data & ECC_BITS) >> 4;
	switch (value)
	{
		case 0:
			ecc_number = 4;
			size_per_sector = 512;
			mtd->oobsize = 16 * (mtd->writesize / size_per_sector);
			break;
		case 1:
			ecc_number = 10;
			size_per_sector = 512;
			mtd->oobsize = 26 * (mtd->writesize / size_per_sector);
			break;
		case 2:
			ecc_number = 8;
			size_per_sector = 512;
			mtd->oobsize = 22 * (mtd->writesize / size_per_sector);
			break;
		case 3:
			ecc_number = 24;
			size_per_sector = 1024;
			mtd->oobsize = 24 * (mtd->writesize / size_per_sector);
			break;
		case 4:
			ecc_number = 15;
			size_per_sector = 1024;
			mtd->oobsize = 36 * (mtd->writesize / size_per_sector);
			break;
		case 5:
			ecc_number = 6;
			size_per_sector = 1024;
			mtd->oobsize = 16 * (mtd->writesize / size_per_sector);
			break;
		case 6:
			ecc_number = 0;
			size_per_sector = 512;
			mtd->oobsize = 16 * (mtd->writesize / size_per_sector);
			break;
		case 7:
			ecc_number = 1;
			size_per_sector = 1024;
			mtd->oobsize = 16 * (mtd->writesize / size_per_sector);
			break;
		default:
			break;
	}

	/*identify buswidth by nand data pin.*/
	value = (io_data & BUS_BITS) >> 7;
	switch (value)
	{
		case 0:
			bus_width = 0;
			break;
		case 1:
			bus_width = NAND_BUSWIDTH_16;
			break;
		default:
			break;
	}
	
	return rt;
}  

static void atxx_nd_set_eccmask(void)
{
	if(size_per_sector == 512) {
	switch (ecc_number) {
	case 4:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0x731A62BB);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0x28C952BB);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0x0000F768);
	break;
	case 8:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0x90D35B12);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0x3A2164E8);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0x4BEC2ACF);
		atxx_nd_write_reg(REG_NFC_RSE_MSK3, 0x6641028A);
		atxx_nd_write_reg(REG_NFC_RSE_MSK4, 0xBBDE209A);
	break;
	case 10:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0x40B8ADB7);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0xC7071A00);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0x8CA0D551);
		atxx_nd_write_reg(REG_NFC_RSE_MSK3, 0x9ADD989A);
		atxx_nd_write_reg(REG_NFC_RSE_MSK4, 0x0B3A4270);
		atxx_nd_write_reg(REG_NFC_RSE_MSK5, 0xF624A164);
		atxx_nd_write_reg(REG_NFC_RSE_MSK6, 0x000000CD);
		break;
	case 12:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0xC3AC74BE);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0xA4FA44BA);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0x8E0D5317);
		atxx_nd_write_reg(REG_NFC_RSE_MSK3, 0xFC9AF48F);
		atxx_nd_write_reg(REG_NFC_RSE_MSK4, 0x932BEE9B);
		atxx_nd_write_reg(REG_NFC_RSE_MSK5, 0x7829A12B);
		atxx_nd_write_reg(REG_NFC_RSE_MSK6, 0x51101FF7);
		atxx_nd_write_reg(REG_NFC_RSE_MSK7, 0x00005279);
		break;
	default:
		printf(
		 "ATXX NFC does not support this ecc:%d\n",
				 ecc_number);
		break;
		}
	} else {
	switch (ecc_number) {
	case 12:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0xBB55817D);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0x96652FD9);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0x9C0AB380);
		atxx_nd_write_reg(REG_NFC_RSE_MSK3, 0x1C183452);
		atxx_nd_write_reg(REG_NFC_RSE_MSK4, 0x5C812C35);
		atxx_nd_write_reg(REG_NFC_RSE_MSK5, 0xF05A99D1);
		atxx_nd_write_reg(REG_NFC_RSE_MSK6, 0x74BC85C9);
		atxx_nd_write_reg(REG_NFC_RSE_MSK7, 0x00000CB7);
		break;
	case 20:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0x14D09A6A);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0xC4CD9056);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0xD4E75487);
		atxx_nd_write_reg(REG_NFC_RSE_MSK3, 0x1FC38B7E);
		atxx_nd_write_reg(REG_NFC_RSE_MSK4, 0xA5D61845);
		atxx_nd_write_reg(REG_NFC_RSE_MSK5, 0xA258FAB3);
		atxx_nd_write_reg(REG_NFC_RSE_MSK6, 0x758DC42D);
		atxx_nd_write_reg(REG_NFC_RSE_MSK7, 0x559D44E5);
		atxx_nd_write_reg(REG_NFC_RSE_MSK8, 0xCFF0957A);
		atxx_nd_write_reg(REG_NFC_RSE_MSK9, 0xCE5A9E67);
		atxx_nd_write_reg(REG_NFC_RSE_MSK10,0xA13ED6B3);
		atxx_nd_write_reg(REG_NFC_RSE_MSK11,0xB94E8815);
		atxx_nd_write_reg(REG_NFC_RSE_MSK12,0xFFFF47B0);
		break;
	case 22:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0xC9D1A6E1);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0xDE8CBDF1);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0xCC4A362A);
		atxx_nd_write_reg(REG_NFC_RSE_MSK3, 0xFCCBA97C);
		atxx_nd_write_reg(REG_NFC_RSE_MSK4, 0xE5107C7C);
		atxx_nd_write_reg(REG_NFC_RSE_MSK5, 0x27453FC5);
		atxx_nd_write_reg(REG_NFC_RSE_MSK6, 0x7C254C32);
		atxx_nd_write_reg(REG_NFC_RSE_MSK7, 0x41D956AF);
		atxx_nd_write_reg(REG_NFC_RSE_MSK8, 0xF6E6DA51);
		atxx_nd_write_reg(REG_NFC_RSE_MSK9, 0xE4CEB35E);
		atxx_nd_write_reg(REG_NFC_RSE_MSK10,0xC731FF3A);
		atxx_nd_write_reg(REG_NFC_RSE_MSK11,0x953E282F);
		atxx_nd_write_reg(REG_NFC_RSE_MSK12,0xB79CBA54);
		atxx_nd_write_reg(REG_NFC_RSE_MSK13,0xFF462220);
		atxx_nd_write_reg(REG_NFC_RSE_MSK14,0xFFFFFFFF);
		break;
	case 24:
		atxx_nd_write_reg(REG_NFC_RSE_MSK0, 0x3FCE9F6B);
		atxx_nd_write_reg(REG_NFC_RSE_MSK1, 0x6BE36897);
		atxx_nd_write_reg(REG_NFC_RSE_MSK2, 0x3030B956);
		atxx_nd_write_reg(REG_NFC_RSE_MSK3, 0x3CFE3C1C);
		atxx_nd_write_reg(REG_NFC_RSE_MSK4, 0x2982FA4F);
		atxx_nd_write_reg(REG_NFC_RSE_MSK5, 0xAB02BD6D);
		atxx_nd_write_reg(REG_NFC_RSE_MSK6, 0xCA9A8C9E);
		atxx_nd_write_reg(REG_NFC_RSE_MSK7, 0xDD80D630);
		atxx_nd_write_reg(REG_NFC_RSE_MSK8, 0x969DB66A);
		atxx_nd_write_reg(REG_NFC_RSE_MSK9, 0x30F88E60);
		atxx_nd_write_reg(REG_NFC_RSE_MSK10,0xFBFC8B28);
		atxx_nd_write_reg(REG_NFC_RSE_MSK11,0x94B86945);
		atxx_nd_write_reg(REG_NFC_RSE_MSK12,0x795F25E0);
		atxx_nd_write_reg(REG_NFC_RSE_MSK13,0xAE1CBC27);
		atxx_nd_write_reg(REG_NFC_RSE_MSK14,0x5648E21E);
		break;
	default:
		printf(
		 "ATXX NFC does not support this ecc:%d\n",
				 ecc_number);
		break;
		}
	}

	atxx_nd_write_reg(REG_NFC_RSD_MSK0, atxx_nd_read_reg(REG_NFC_RSE_MSK0));
	atxx_nd_write_reg(REG_NFC_RSD_MSK1, atxx_nd_read_reg(REG_NFC_RSE_MSK1));
	atxx_nd_write_reg(REG_NFC_RSD_MSK2, atxx_nd_read_reg(REG_NFC_RSE_MSK2));
	atxx_nd_write_reg(REG_NFC_RSD_MSK3, atxx_nd_read_reg(REG_NFC_RSE_MSK3));
	atxx_nd_write_reg(REG_NFC_RSD_MSK4, atxx_nd_read_reg(REG_NFC_RSE_MSK4));
	atxx_nd_write_reg(REG_NFC_RSD_MSK5, atxx_nd_read_reg(REG_NFC_RSE_MSK5));
	atxx_nd_write_reg(REG_NFC_RSD_MSK6, atxx_nd_read_reg(REG_NFC_RSE_MSK6));
	atxx_nd_write_reg(REG_NFC_RSD_MSK7, atxx_nd_read_reg(REG_NFC_RSE_MSK7));
	atxx_nd_write_reg(REG_NFC_RSD_MSK8, atxx_nd_read_reg(REG_NFC_RSE_MSK8));
	atxx_nd_write_reg(REG_NFC_RSD_MSK9, atxx_nd_read_reg(REG_NFC_RSE_MSK9));
	atxx_nd_write_reg(REG_NFC_RSD_MSK10,atxx_nd_read_reg(REG_NFC_RSE_MSK10));
	atxx_nd_write_reg(REG_NFC_RSD_MSK11,atxx_nd_read_reg(REG_NFC_RSE_MSK11));
	atxx_nd_write_reg(REG_NFC_RSD_MSK12,atxx_nd_read_reg(REG_NFC_RSE_MSK12));
	atxx_nd_write_reg(REG_NFC_RSD_MSK13,atxx_nd_read_reg(REG_NFC_RSE_MSK13));
	atxx_nd_write_reg(REG_NFC_RSD_MSK14,atxx_nd_read_reg(REG_NFC_RSE_MSK14));
}

/*
 * Get the flash and manufacturer id and lookup if the type is supported
 */
static struct nand_flash_dev *atxx_nd_get_flash_type(struct mtd_info *mtd, 
			struct nand_chip *chip, int *maf_id)
{
	struct nand_flash_dev *type = NULL;
	int i, dev_id, maf_idx, ext_id, ext_id_bak;
	uint32_t rt;


	/* Select the device */
	atxx_nd_select_chip(mtd, 0);
	atxx_nd_read_ids(maf_id, &dev_id, &ext_id);
	ext_id_bak = ext_id;

	printf("maf_id 0x%x, dev_id 0x%x, ext_id 0x%x\n", *maf_id, dev_id, ext_id);
	/* Lookup the flash id */
	for (i = 0; nand_flash_ids[i].name != NULL; i++) {
		if (dev_id == nand_flash_ids[i].id) {
			type = &nand_flash_ids[i];
			break;
		}
	}

	if (!type)
		return ERR_PTR(-ENODEV);

	if (!mtd->name)
		mtd->name = type->name;

	chip->chipsize = type->chipsize << 20;

	/* Newer devices have all the information in additional id bytes */
	if (!type->pagesize) {
		mtd->writesize = 1024 << (ext_id & 0x3);
		ext_id >>= 2;
		/* Calc oobsize */
		mtd->oobsize = (8 << (ext_id & 0x01)) * (mtd->writesize >> 9);
		ext_id >>= 2;
		/* Calc blocksize. Blocksize is multiples of 64KiB */
		mtd->erasesize = (64 * 1024) << (ext_id & 0x03);
		ext_id >>= 2;
		/* Get buswidth information */
		bus_width = (ext_id & 0x01) ? NAND_BUSWIDTH_16 : 0;

	} else {
		/*
		 * Old devices have chip data hardcoded in the device id table
		 */
		mtd->erasesize = type->erasesize;
		mtd->writesize = type->pagesize;
		mtd->oobsize = mtd->writesize / 32;
		bus_width = type->options & NAND_BUSWIDTH_16;
	}

	/* Try to identify manufacturer */
	for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++) {
		if (nand_manuf_ids[maf_idx].id == *maf_id)
			break;
	}

	/*
	* we have used nand data pin to identify nand
	* because lots of new flash do not following the ID format.
	*/
	rt = atxx_nd_identify(mtd);
	if(rt == 1)
		return ERR_PTR(-ENODEV);
	/* Calculate the address shift from the page size */
	chip->page_shift = ffs(mtd->writesize) - 1;
	/* Convert chipsize to number of pages per chip -1. */
	chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;

	chip->bbt_erase_shift = chip->phys_erase_shift =
	    ffs(mtd->erasesize) - 1;
	chip->chip_shift = ffs(chip->chipsize) - 1;

	/* Set the bad block position */
	chip->badblockpos = mtd->writesize > 512 ?
	    NAND_LARGE_BADBLOCK_POS : NAND_SMALL_BADBLOCK_POS;

	/* Get chip options, preserve non chip based options */
	chip->options &= ~NAND_CHIPOPTIONS_MSK;
	chip->options |= type->options & NAND_CHIPOPTIONS_MSK;

	/*
	 * Set chip as a default. Board drivers can override it, if necessary
	 */
	chip->options |= NAND_NO_AUTOINCR;

	/* Check if chip is a not a samsung device. Do not clear the
	 * options for chips which are not having an extended id.
	 */
	if (*maf_id != NAND_MFR_SAMSUNG && !type->pagesize)
		chip->options &= ~NAND_SAMSUNG_LP_OPTIONS;

	printf("NAND device: Manufacturer ID:"
		   " 0x%02x, Chip ID: 0x%02x (%s %s %d)\n", *maf_id, dev_id,
		   nand_manuf_ids[maf_idx].name, type->name, mtd->writesize);

	return type;
}


static int atxx_nd_config_init(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	uint32_t reg_data;
	struct atxx_nd *hw = (struct atxx_nd *)chip->priv;
	int i, j, ecc_point;

	/*
	 * Setting ecc structure and layout
	 */
	atxx_nd_write_reg(REG_NFC_ECC_CFG, NFC_ECC_EN_RESET | 
				NFC_ECC_DE_RESET | ecc_number);

	chip->ecc.size = size_per_sector;
	if(ecc_number) {
		/* (2*(ecc_number * 10bit)/8) byte + 1byte/0byte */
		chip->ecc.bytes = ((ecc_number * 10) + 3)/4;
	}
	else {
		chip->ecc.bytes  = 0x08;
	}

	chip->ecc.steps = mtd->writesize / chip->ecc.size;
	chip->ecc.total = chip->ecc.steps * chip->ecc.bytes;

	/*we need 3 bytes for badblock in the beginning of oob*/
	if ((chip->ecc.bytes + 3) > mtd->oobsize/chip->ecc.steps) {
		printf("Not enough oob space for ecc!!!\n");
		chip->select_chip(mtd, -1);
		return ENOSPC;
	}

	reg_data = 0xffff0000 | ((mtd->oobsize/chip->ecc.steps - 1) << 8) 
			| (mtd->oobsize/chip->ecc.steps - chip->ecc.bytes);
	atxx_nd_write_reg(REG_NFC_ECC_CBC1, reg_data);
	atxx_nd_write_reg(REG_NFC_ECC_CBC2, 0xffffffff);

	atxx_nd_set_eccmask();

	hw->atxx_ecclayout.eccbytes = chip->ecc.total;

	for (i = 0; i < chip->ecc.steps; i++) {
		ecc_point = (mtd->oobsize/chip->ecc.steps) * (i+1) 
				- chip->ecc.bytes;
		for (j = 0; j < chip->ecc.bytes; j++) {
			hw->atxx_ecclayout.eccpos[i*chip->ecc.bytes + j] = 
				ecc_point + j;
		}
	}

	/*
	* Nand bad block is marked in 6th byte for 512 pagesize,
	* and first byte for large pagesize.
	*/
	if (mtd->writesize == PAGE_SIZE_512) {
		hw->atxx_ecclayout.oobfree[0].offset = 0;
		hw->atxx_ecclayout.oobfree[0].length = 5;
	} else {
		hw->atxx_ecclayout.oobfree[0].offset = 3;
		hw->atxx_ecclayout.oobfree[0].length =
			mtd->oobsize/chip->ecc.steps - chip->ecc.bytes - 3;
	}

	for (i = 1; i < chip->ecc.steps; i++) {
		hw->atxx_ecclayout.oobfree[i].offset = 
				i * mtd->oobsize/chip->ecc.steps;
		hw->atxx_ecclayout.oobfree[i].length = 
				mtd->oobsize/chip->ecc.steps - chip->ecc.bytes;
	}

	chip->ecc.layout = &hw->atxx_ecclayout;

	/* propagate ecc.layout to mtd_info */
	mtd->ecclayout = chip->ecc.layout;

#ifdef CONFIG_ATXX_NAND_DEBUG
	printf("REG_NFC_ECC_CFG:%02x, REG_NFC_ECC_CBC1:%02x\n",  
		atxx_nd_read_reg(REG_NFC_ECC_CFG), atxx_nd_read_reg(REG_NFC_ECC_CBC1));
	printf("ecc.size:0x%02x, bytes:0x%02x, steps:0x%02x, total:0x%02x\n", 
		chip->ecc.size, chip->ecc.bytes, chip->ecc.steps, chip->ecc.total);
	for (i = 0; i < chip->ecc.total; i++) {	
		printf(" %02x\n", mtd->ecclayout->eccpos[i]);
	}
	printf("mtd->ecclayout=%x\n", mtd->ecclayout);
	for (i = 0; i < chip->ecc.steps; i++) {
		printf("offset:%02x, length:%02x\n", 	
		mtd->ecclayout->oobfree[i].offset,
		mtd->ecclayout->oobfree[i].length);
	}
#endif
	/*
	 * The number of bytes available for a client to place data into
	 * the out of band area
	 */
	chip->ecc.layout->oobavail = 0;
	for (i = 0; i < chip->ecc.steps; i++)
		chip->ecc.layout->oobavail +=
		    chip->ecc.layout->oobfree[i].length;

	/* setup page size */
	switch (mtd->writesize) {
	case 8192:
		reg_data = atxx_nd_read_reg(REG_NFC_PARA0);
		reg_data &= ~(0xf << NFC_PARA0_PAGE_SIZE_SHIFT);
		reg_data |= (3 << NFC_PARA0_PAGE_SIZE_SHIFT);
		reg_data &= ~(1 << 16);
		atxx_nd_write_reg(REG_NFC_PARA0, reg_data);
		break;
	case 4096:
		reg_data = atxx_nd_read_reg(REG_NFC_PARA0);
		reg_data &= ~(0xf << NFC_PARA0_PAGE_SIZE_SHIFT);
		reg_data |= (2 << NFC_PARA0_PAGE_SIZE_SHIFT);
		reg_data &= ~(1 << 16);
		atxx_nd_write_reg(REG_NFC_PARA0, reg_data);
		break;
	case 2048:
		reg_data = atxx_nd_read_reg(REG_NFC_PARA0);
		reg_data &= ~(0xf << NFC_PARA0_PAGE_SIZE_SHIFT);
		reg_data |= (0x01 << NFC_PARA0_PAGE_SIZE_SHIFT);
		reg_data &= ~(1 << 16);
		atxx_nd_write_reg(REG_NFC_PARA0, reg_data);
		break;
	case 512:
		reg_data = atxx_nd_read_reg(REG_NFC_PARA0);
		reg_data &= ~(0xf << NFC_PARA0_PAGE_SIZE_SHIFT);
		reg_data |= (1 << 16);
		reg_data &= ~0xF0;
		atxx_nd_write_reg(REG_NFC_PARA0, reg_data);
	default:
		break;
	}

	if (size_per_sector == 1024)
		atxx_nd_write_reg(REG_NFC_PARA0, atxx_nd_read_reg(REG_NFC_PARA0) | 0x80);

	if (bus_width == NAND_BUSWIDTH_16) {
		reg_data = atxx_nd_read_reg(REG_NFC_PARA0);
		reg_data |= NFC_PARA0_DEVICE_BUS;
		atxx_nd_write_reg(REG_NFC_PARA0, reg_data);
	}


	/*oob check baseaddr should set to pagesize only to make sure there is enough space.*/
	if ((mtd->oobsize / chip->ecc.steps) > 32)
		reg_data = mtd->writesize | (0x40 << 16);
	else
		reg_data = (mtd->writesize + mtd->oobsize) | ((mtd->oobsize /chip->ecc.steps) << 16);
	atxx_nd_write_reg(REG_NFC_PARA1, reg_data);

	return 0;
}




/**
 * atxx_nd_scan - [NAND Interface] Scan for the NAND device
 * @mtd:	MTD device structure
 * @maxchips:	Number of chips to scan for
 *
 * This fills out all the uninitialized function pointers
 * with the defaults.
 * The flash ID is read and the mtd/chip structures are
 * filled with the appropriate values.
 * The mtd->owner field must be set to the module of the caller
 *
 */
int atxx_nd_scan(struct mtd_info *mtd, int maxchips)
{
	int i, nand_maf_id = 0;
	struct nand_chip *chip = mtd->priv;
	struct nand_flash_dev *type;

	mtd_bak = mtd;
	
	/* Set the default functions */
	nand_set_defaults(chip, 0);

	/*reset all the nand chip*/
	for (i = 0; i < maxchips; i++) {
		chip->select_chip(mtd, i);
		nfc_send_reset_cmd();
		mdelay(100);
	}
	chip->select_chip(mtd, -1);

	/* Read the flash type */
	type = atxx_nd_get_flash_type(mtd, chip, &nand_maf_id);

	if (IS_ERR(type)) {
		printf("No NAND device found!!!\n");
		chip->select_chip(mtd, -1);
		return PTR_ERR(type);
	}

	/* Check for a chip array */
	for (i = 1; i < maxchips; i++) {
		int first_id, second_id, fourth_id;
		chip->select_chip(mtd, i);
		atxx_nd_read_ids(&first_id, &second_id, &fourth_id);

		/* Read manufacturer and device IDs */
		if (nand_maf_id != first_id || type->id != second_id)
			break;
	}
	if (i > 1)
		printf("%d NAND chips detected\n", i);

	/* Store the number of chips and calc total size for mtd */
	chip->numchips = i;
	mtd->size = i * chip->chipsize;
	
	atxx_nd_config_init(mtd);
	
	/* atxx oob read/write function in full hardware mode */
	chip->ecc.read_oob = atxx_nd_read_oob_std;
	chip->ecc.write_oob = atxx_nd_write_oob_std;
	chip->ecc.read_page = atxx_nd_read_page_hwecc;
	chip->ecc.read_page_raw = atxx_nd_read_page_raw;
	/* chip->ecc.read_subpage = ? TODO ; */

	/* we do not support subpage yet */
	chip->subpagesize = mtd->writesize;

	/* Initialize state */
	chip->state = FL_READY;

	/* De-select the device */
	chip->select_chip(mtd, -1);

	/* Invalidate the pagebuffer reference */
	chip->pagebuf = -1;

	/* setup buffer */
	chip->buffers = malloc(sizeof(*chip->buffers));
	if (!chip->buffers)
		return -ENOMEM;

	/* Set the internal oob buffer location, just after the page data */
	chip->oob_poi = chip->buffers->databuf + mtd->writesize;

	/* set mtd fields */
	mtd_set_defaults(mtd);

	chip->options = NAND_NO_AUTOINCR | NAND_NO_READRDY;

	/* Check, if we should skip the bad block table scan */
	if (chip->options & NAND_SKIP_BBTSCAN)
		return 0;
	
#ifdef CONFIG_ATXX_NAND_DEBUG
	if (bad_scan != 0)
		/* Build bad block table */
		return chip->scan_bbt(mtd);
	else
		return 0;
#endif
	return chip->scan_bbt(mtd);

}

static void atxx_nd_set_timing(void)
{
	uint32_t reg_data;
	uint8_t read_hold_time, read_setup_time;
	uint8_t write_hold_time, write_setup_time;
        struct clk *clk_app;
	unsigned long rate;

        clk_app = clk_get("app");	
	rate = clk_get_rate(clk_app);

	/* timing params according to clk_app */
	if (rate >= 156 * 1000 * 1000) {
		read_hold_time = read_setup_time = 3;
		write_hold_time = write_setup_time = 4;
	} else if ((rate < 156 * 1024 * 1024)
		   && (rate >= 104 * 1000 * 1000)) {
		read_hold_time = 3;
		read_setup_time = 3;
		write_hold_time = 4;
		write_setup_time = 4;
	} else {
		read_hold_time = read_setup_time = 3;
		write_hold_time = write_setup_time = 4;
	}

	reg_data = (0xff << 16) | (read_hold_time << NFC_READ_HOLD_TIME_SHIFT)
	    | (read_setup_time << NFC_READ_SETUP_TIME_SHIFT)
	    | (write_hold_time << NFC_WRITE_HOLD_TIME_SHIFT)
	    | (write_setup_time << NFC_WRITE_SETUP_TIME_SHIFT);
	atxx_nd_write_reg(REG_NFC_TIMING, reg_data);

}

static void atxx_nd_controller_init(void)
{
	uint32_t reg_data;

	/* reset nand flash controller */
	NFC_RESET();

	reg_data = atxx_nd_read_reg(REG_NFC_PARA0);
	reg_data &= ~(NFC_PARA0_WP | NFC_PARAR0_MANUAL_CE);
	atxx_nd_write_reg(REG_NFC_PARA0, reg_data);

	/* timing params according to clk_app */
	atxx_nd_set_timing();

	/* setup fifo low/high level */
	reg_data = NFC_FIFO_LOW_LEV | (NFC_FIFO_HIGH_LEV <<
					   NFC_FIFO_CFG_HIGH_SHIFT);
	atxx_nd_write_reg(REG_NFC_FIFO_CFG, reg_data);

	/* disable all interrupts */
	reg_data = atxx_nd_read_reg(REG_NFC_INT_MASK);
	reg_data &= ~(NFC_INT_FIFO_HIGH_MASK | NFC_INT_FIFO_LOW_MASK
			  | NFC_INT_CMD_MASK | NFC_INT_ECCE_RDY_MASK
			  | NFC_INT_ECCD_RDY_MASK|NFC_INT_DMA_FINI);
	atxx_nd_write_reg(REG_NFC_INT_MASK, reg_data);

	/* set DMA burst_size to 8 */
	atxx_nd_write_reg(REG_NFC_DMA_CFG, 8);

	/* reset nand flash */
	nfc_send_reset_cmd();
	mdelay(100);		/*reset time, max 100ms */
}

/*******************************************************************************
 *  Nand driver interfaces 
 *******************************************************************************/
int board_nand_init(struct nand_chip *this)
{
	this->priv = (void *)malloc(sizeof(struct atxx_nd));

	/* replace hardware dependent functions */
	this->chip_delay = 10;	/*udelay */
	this->dev_ready = atxx_nd_ready;
	this->select_chip = atxx_nd_select_chip;
	this->waitfunc = atxx_nd_wait;
	this->write_page = atxx_nd_write_page;

	this->erase_cmd = atxx_nd_erase;
	/* do atxx nfc initi */
	atxx_nd_controller_init();
	return 0;
}

int board_nand_reinit(uint8_t flag)
{
	return 0;
}


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
 *
 * Filename:  atxx_nand.h
 * Abstract:  headfile of the atxx nand controller driver
 * Author  :  baohua.song
 ------------------------------------------------------------------------------*/
#ifndef ATXX_ND_H
#define ATXX_ND_H

#define ATXX_ND_TIMEOUT 50000	/* 50ms */
#define NFC_TIMEOUT_JIFFIS   		3000
#define ADDR_WAIT_BUSY 			0x40

#define PAGE_SIZE_512			0x200
#define PAGE_SIZE_2K			0x800
#define PAGE_SIZE_4K			0x1000
#define PAGE_SIZE_8K			0x2000
//#define CONFIG_ATXX_NAND_DEBUG
#ifdef  CONFIG_ATXX_NAND_DEBUG
#define atxx_nd_debug(fmt, args...) \
	printf( fmt, ##args)
#define atxx_nd_err(fmt, args...) \
	printf( fmt, ##args)
#else
#define atxx_nd_debug(fmt,arg...) do { } while (0)
#define atxx_nd_err(fmt,arg...) do { } while (0)
#endif

/**********************************/
/* NFC registers */
#define REG_NFC_PARA0           (0x6000)
#define REG_NFC_TIMING          (0x6004)
#define REG_NFC_CFG0            (0x6008)
#define REG_NFC_CFG1            (0x600c)
#define REG_NFC_CFG2            (0x6010)
#define REG_NFC_BADDR           (0x6014)
#define REG_NFC_FADDR0          (0x6018)
#define REG_NFC_FADDR1          (0x601c)
#define REG_NFC_CMD             (0x6020)
#define REG_NFC_CCFG            (0x6024)
#define REG_NFC_STATUS          (0x6028)
#define REG_NFC_ECC_CFG         (0x602c)
#define REG_NFC_ECC_CBC1        (0x6030)
#define REG_NFC_ECC_CBC2        (0x6034)
#define REG_NFC_ECCSTATUS1      (0x6038)
#define REG_NFC_ECCSTATUS2      (0x603c)
#define REG_NFC_FIFO_CFG        (0x6040)
#define REG_NFC_FIFO_STATUS     (0x6044)
#define REG_NFC_FIFO            (0x6048)
#define REG_NFC_SOFT_PIN        (0x6050)
#define REG_NFC_SOFT_PIN2       (0x60c0) 
#define REG_NFC_PARA1           (0x6054)
#define REG_NFC_INT_MASK        (0x605c)
#define REG_NFC_INT_CODE        (0x6058)
#define REG_NFC_DMA_CFG         (0x6060)
#define REG_NFC_DMA_CTRL        (0x6064)
#define REG_NFC_DMA_SAR         (0x6068)
#define REG_NFC_DMA_DAR         (0x606C)
#define REG_NFC_DMA_STAT        (0x6070)
#define REG_NFC_DEBUG0          (0x6100)
#define REG_NFC_DEBUG1          (0x6104)
#define REG_NFC_DEBUG2          (0x6108)
#define REG_NFC_RSE_MSK0        (0x6080)
#define REG_NFC_RSE_MSK1        (0x6084)
#define REG_NFC_RSE_MSK2        (0x6088)
#define REG_NFC_RSE_MSK3        (0x608c)
#define REG_NFC_RSD_MSK0        (0x6090)
#define REG_NFC_RSD_MSK1        (0x6094)
#define REG_NFC_RSD_MSK2        (0x6098)
#define REG_NFC_RSD_MSK3        (0x609c)
#define REG_NFC_RSE_MSK4        (0x60a0)
#define REG_NFC_RSE_MSK5        (0x60a4)
#define REG_NFC_RSE_MSK6        (0x60a8)
#define REG_NFC_RSE_MSK7        (0x60ac)
#define REG_NFC_RSD_MSK4        (0x60b0)
#define REG_NFC_RSD_MSK5        (0x60b4)
#define REG_NFC_RSD_MSK6        (0x60b8)
#define REG_NFC_RSD_MSK7        (0x60bc)
#define REG_NFC_RSE_MSK8        (0x60C8)
#define REG_NFC_RSD_MSK8        (0x60D0)


/* NFC params register */
#define NFC_PARA0_DEVICE_BUS    	(1<<8)
#define NFC_PARA0_BANK_CE0    		(1<<9)
#define NFC_PARA0_BANK_CE1    		(1<<10)
#define NFC_PARA0_BANK_CE2    		(1<<12)
#define NFC_PARA0_BANK_CE3    		(1<<13)
#define NFC_PARAR0_STD_CMD    		(0<<14)
#define NFC_PARAR0_SW_PIN_CMD 		(1<<14)
#define NFC_PARAR0_OLD        		(1<<16)
#define NFC_PARA0_WP          		(1<<17)
#define NFC_PARAR0_MANUAL_CE   		(1<<20)
#define NFC_PARA0_PAGE_SIZE_SHIFT   	4

/* NFC timing register */
#define NFC_READ_HOLD_TIME_SHIFT      	12
#define NFC_READ_SETUP_TIME_SHIFT     	8
#define NFC_WRITE_HOLD_TIME_SHIFT     	4
#define NFC_WRITE_SETUP_TIME_SHIFT    	0

/* command config */
#define NFC_CFG2_RESET 			(1<<31)

/* NFC_FADDR registers address */
#define NFC_FADDR1_CEB0  		(1<<28)
#define NFC_FADDR1_CEB1  		(1<<29)
#define NFC_FADDR1_CEB2  		(1<<30)
#define NFC_FADDR1_CEB3  		(1<<31)

/* NFC_CMD register */
#define NFC_CMD_CMD_MASK  		0xff000000
#define NFC_CMD_CMD_SHIFT 		24
#define NFC_CMD_ADDR_LEN_MASK   	0xf0
#define NFC_CMD_ADDR_LEN_SHIFT  	4
#define NFC_CMD_DATA_LEN_MASK   	0x3fff00
#define NFC_CMD_DATA_LEN_SHIFT  	8
#define NFC_CMD_ECC        		(1<<23)
#define NFC_CMD_ENABLE     		0x01
#define NFC_CMD_INT        		0x02
#define NFC_CMD_WAIT_BUSY  		0x04
/* command codes */
#define NFC_CMD_READ             	0x01
#define NFC_CMD_PAGE_PROGRAM     	0x02
#define NFC_CMD_BLOCK_ERASE      	0x03
#define NFC_CMD_READ_ID          	0x04
#define NFC_CMD_READ_STATUS      	0x05
#define NFC_CMD_UDFA_READ        	0x06
#define NFC_CMD_UDFA_PROG        	0x07
#define NFC_CMD_SPECIAL_PROG_OPENMODE                   0x08
#define NFC_CMD_SPECIAL_UDFA_PROG_BUFFER                0x09
#define NFC_CMD_SPECIAL_UDFA_PROG_OPENMODE_BUFFER       0x0a
#define NFC_CMD_SPECIAL_UDFA_PROG_OPENMODE_FIFO         0x0b
#define NFC_CMD_UDFB_READ        	0x16
#define NFC_CMD_UDFB_PROG        	0x17
#define NFC_CMD_RESET            	0x0f
#define NFC_CMD_UDF1_CYCLE       	0x20
#define NFC_CMD_UDF2_CYCLE       	0x21
#define NFC_CMD_UDF3_CYCLE       	0x22
#define NFC_CMD_UDF4_CYCLE       	0x23
#define NFC_CMD_ADDR_CYCLE       	0x24
#define NFC_CMD_DATA_IN          	0x25
#define NFC_CMD_DATA_OUT         	0x26

/* NFC status register */
#define NFC_STATUS_RBB0     		(1<<24)
#define NFC_STATUS_RBB1     		(1<<25)
#define NFC_STATUS_RBB2     		(1<<26)
#define NFC_STATUS_RBB3     		(1<<27)

/* NFC ecc registers */
#define NFC_ECC_RSE_CK_RDY  		(0x01<<31)
#define NFC_ECC_EN_RESET    		(1<<16)
#define NFC_ECC_DE_RESET    		(1<<17)
#define NFC_ECC_PER_PAGE    		(1<<8)

#define NFC_ECC_UNCORRECT1  		(0x01<<1)
#define NFC_ECC_UNCORRECT2  		(0x01<<9)
#define NFC_ECC_UNCORRECT3  		(0x01<<17)
#define NFC_ECC_UNCORRECT4  		(0x01<<25)
#define NFC_ECC_CORRECTED_SHIFT1   	4
#define NFC_ECC_CORRECTED_MASK1    	0xf0
#define NFC_ECC_CORRECTED_SHIFT2   	12
#define NFC_ECC_CORRECTED_MASK2    	0xf000
#define NFC_ECC_CORRECTED_SHIFT3   	20
#define NFC_ECC_CORRECTED_MASK3    	0xf00000
#define NFC_ECC_CORRECTED_SHIFT4   	28
#define NFC_ECC_CORRECTED_MASK4    	0xf0000000

/* NFC FIFO registers */
#define NFC_FIFO_CFG_LOW_SHIFT  	0
#define NFC_FIFO_CFG_HIGH_SHIFT 	8
#define NFC_FIFO_LOW_LEV  		3
#define NFC_FIFO_HIGH_LEV 		12
#define NFC_FIFO_ADDR (ATXX_NAND_BASE + 0x6048)
#define NFC_FIFO_FULL_BIT   		0x02
#define NFC_FIFO_EMPTY_BIT  		0x01

/* NFC ATXX_NAND_SOFT_PIN register */
#define NFC_SOFT_PIN_RE 		(1<<20)
#define NFC_SOFT_PIN_WE 		(1<<21)
#define NFC_SOFT_PIN_WP 		(1<<22)
#define NFC_SOFT_PIN_RB0 		(1<<24)
#define NFC_SOFT_PIN_RB1 		(1<<25)
#define NFC_SOFT_PIN_RB2 		(1<<26)
#define NFC_SOFT_PIN_RB3 		(1<<27)
#define NFC_SOFT_PIN_CE0 		(1<<18)
#define NFC_SOFT_PIN_CE1 		(1<<19)
#define NFC_SOFT_PIN_CE2 		(1<<28)
#define NFC_SOFT_PIN_CE3 		(1<<29)
#define NFC_SOFT_PIN_ALE 		(1<<17)
#define NFC_SOFT_PIN_CLE 		(1<<16)
#define NFC_SOFT_PIN_OUT_EN 		(1<<31)
#define NFC_SOFT_OUT_IO   		0xff00
#define NFC_SOFT_OUT_SHIFT 		8
#define NFC_SOFT_IN_IO    		0xff

/* param1 */
#define OOB_PER_512_BYTE    		16
#define RSE_BASE        		0

/* NFC interrupt registers */
#define NFC_INT_ECCD_RDY_MASK      	0x20
#define NFC_INT_ECCE_RDY_MASK      	0x8
#define NFC_INT_FIFO_HIGH_MASK  	0x4
#define NFC_INT_FIFO_LOW_MASK   	0x2
#define NFC_INT_CMD_MASK        	0x1
#define NFC_INT_DMA_FINI          	0x10
#define NFC_INT_FIFO_HIGH_MARK  	0x4
#define NFC_INT_FIFO_LOW_MARK   	0x2
#define NFC_INT_CMD_MARK        	0x1
#define NFC_INT_ENCODE_MARK        	0x8
#define NFC_INT_DECODE_MARK        	0x20

/* DMA command control */
#define NFC_DMA_ENABLE 			0x01
#define NFC_DMA_LEN       		8
#define NFC_DMA_RW       		0x02
#define NFC_DMA_RUNNING       		(0x1<<31)

/* NFC read buffer base address */
#define NFC_READ_BUF_ADDR  ATXX_NAND_BASE
#define NFC_READ_BUF ((volatile u32 *)(ATXX_NAND_BASE))

#define ID_DATA_ADDR  		0x1000
#define ECC_DATA_ADDR  		0x1080

#define SIZE_BITS		0x0f     /*bit 0~4 to identify pagesize and blocksize.*/
#define ECC_BITS		0x70     /*bit 5~7 to identify ecc number*/
#define BUS_BITS		0x80     /*bit 8 to identify buswidth.*/

struct atxx_nd {
	struct device *dev;
	struct mtd_info *at_mtd;
	struct nand_ecclayout atxx_ecclayout;	
	struct resource *ioarea;
	struct clk *clk;
};

/*
 * NAND Flash Manufacturer ID Codes
 */
#define NAND_MFR_TOSHIBA	0x98
#define NAND_MFR_SAMSUNG	0xec
#define NAND_MFR_FUJITSU	0x04
#define NAND_MFR_NATIONAL	0x8f
#define NAND_MFR_RENESAS	0x07
#define NAND_MFR_STMICRO	0x20
#define NAND_MFR_HYNIX		0xad
#define NAND_MFR_MICRON		0x2c
#define NAND_MFR_AMD		0x01

#endif


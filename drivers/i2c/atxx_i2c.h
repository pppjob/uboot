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
#ifndef __ATXX_I2C_H_
#define __ATXX_I2C_H_

#include <asm/io.h>
#include <common.h>
#include <linux/types.h>
#include <asm/arch-atxx/regs_base.h>

#define I2C_BASE_ADDR                           (ATXX_I2C_BASE)
#define rI2C_CON                                (I2C_BASE_ADDR+0x00)
#define rI2C_TAR                                (I2C_BASE_ADDR+0x04)
#define rI2C_SAR                                (I2C_BASE_ADDR+0x08)
#define rI2C_HS_MADDR                           (I2C_BASE_ADDR+0x0C)
#define rI2C_DATA_CMD                           (I2C_BASE_ADDR+0x10)
#define rI2C_SS_SCL_HCNT                        (I2C_BASE_ADDR+0x14)
#define rI2C_SS_SCL_LCNT                        (I2C_BASE_ADDR+0x18)
#define rI2C_FS_SCL_HCNT                        (I2C_BASE_ADDR+0x1C)
#define rI2C_FS_SCL_LCNT                        (I2C_BASE_ADDR+0x20)
#define rI2C_HS_SCL_HCNT                        (I2C_BASE_ADDR+0x24)
#define rI2C_HS_SCL_LCNT                        (I2C_BASE_ADDR+0x28)
#define rI2C_INTR_STAT                          (I2C_BASE_ADDR+0x2C)
#define rI2C_INTR_MASK                          (I2C_BASE_ADDR+0x30)
#define rI2C_RAW_INTR_STAT                      (I2C_BASE_ADDR+0x34)
#define rI2C_RX_TL                              (I2C_BASE_ADDR+0x38)
#define rI2C_TX_TL                              (I2C_BASE_ADDR+0x3C)
#define rI2C_CLR_INTR                           (I2C_BASE_ADDR+0x40)
#define rI2C_CLR_RX_UNDER                       (I2C_BASE_ADDR+0x44)
#define rI2C_CLR_RX_OVER                        (I2C_BASE_ADDR+0x48)
#define rI2C_CLR_TX_OVER                        (I2C_BASE_ADDR+0x4C)
#define rI2C_CLR_RD_REQ                         (I2C_BASE_ADDR+0x50)
#define rI2C_CLR_TX_ABRT                        (I2C_BASE_ADDR+0x54)
#define rI2C_CLR_RX_DONE                        (I2C_BASE_ADDR+0x58)
#define rI2C_CLR_ACTIVITY                       (I2C_BASE_ADDR+0x5C)
#define rI2C_CLR_STOP_DET                       (I2C_BASE_ADDR+0x60)
#define rI2C_CLR_START_DET                      (I2C_BASE_ADDR+0x64)
#define rI2C_CLR_GEN_CALL                       (I2C_BASE_ADDR+0x68)
#define rI2C_ENABLE                             (I2C_BASE_ADDR+0x6C)
#define rI2C_STATUS                             (I2C_BASE_ADDR+0x70)
#define rI2C_TXFLR                              (I2C_BASE_ADDR+0x74)
#define rI2C_RXFLR                              (I2C_BASE_ADDR+0x78)
#define rI2C_TX_ABRT_SOURCE                     (I2C_BASE_ADDR+0x80)
#define rI2C_SLV_DATA_NACK_ONLY                 (I2C_BASE_ADDR+0x84)
#define rI2C_DMA_CR                             (I2C_BASE_ADDR+0x88)
#define rI2C_DMA_TDLR                           (I2C_BASE_ADDR+0x8C)
#define rI2C_DMA_RDLR                           (I2C_BASE_ADDR+0x90)
#define rI2C_SDA_SETUP                          (I2C_BASE_ADDR+0x94)
#define rI2C_ACK_GENERAL_CALL                   (I2C_BASE_ADDR+0x98)
#define rI2C_ENABLE_STATUS                      (I2C_BASE_ADDR+0x9C)
#define rI2C_COMP_PARAM_1                       (I2C_BASE_ADDR+0xF4)
#define rI2C_COMP_VERSION                       (I2C_BASE_ADDR+0xF8)
#define rI2C_COMP_TYPE                          (I2C_BASE_ADDR+0xFC)


#define I2C_CLK_FREQ				52000000
#define SS_FREQ					100000		
#define FS_FREQ					400000		
#define HS_FREQ					3400000		

#define SS_SCL_HIGH         	(I2C_CLK_FREQ/SS_FREQ/2-40)
#define SS_SCL_LOW          	(I2C_CLK_FREQ/SS_FREQ/2+40)
#define FS_SCL_HIGH         	(I2C_CLK_FREQ/FS_FREQ/2-20)
#define FS_SCL_LOW          	(I2C_CLK_FREQ/FS_FREQ/2+20)
#define HS_SCL_HIGH		(I2C_CLK_FREQ/HS_FREQ/2-2)
#define HS_SCL_LOW		(I2C_CLK_FREQ/HS_FREQ/2+2)

#define SDA_SETUP_DELAY		(I2C_CLK_FREQ/1000000*1)	
/* timeout waiting for the controller to respond */
#define I2C_MAX_TIMEOUT 	3000000 	/*3ms*/
#define MAX_BUFFER_LEN 		128

#endif /*__ATXX_I2C_H_*/

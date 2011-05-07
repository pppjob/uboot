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
#include <asm/io.h>
#include <common.h>
#include <linux/types.h>
#include <i2c.h>
#include "atxx_i2c.h"

#define I2CCTL	(ATXX_TOPCTL_BASE+0x28)

//#define DEBUG
#ifdef DEBUG
#define DPRINTF(args...)  printf(args)
#else
#define DPRINTF(args...) 
#endif

extern  void udelay(unsigned long n);

typedef enum _bool{FALSE, TRUE} bool;

enum {
	I2C_M_WR, 
	I2C_M_RD
};

static u8 i2c_tx_rx_byte_max;
int i2c_debug;

static void i2c_ctl_init(u8 init_convalue, u32 speedmode)
{
	u8 mylw, i2c_tf_tl, i2c_rf_tl;
	u32 reg_mid;

	DPRINTF("i2c_ctl_init\n");
	writel(0, rI2C_ENABLE);
	reg_mid = readl(rI2C_DMA_CR)&0xfffffffd;
	/*disables DMA transmit and receive*/
	writel(reg_mid, rI2C_DMA_CR); 	
	writel(0, rI2C_INTR_MASK);

	/*disables and clears all interrupts*/	
	writel(0, rI2C_CLR_INTR);  
	writel(init_convalue, rI2C_CON);

	writel(SS_SCL_HIGH, rI2C_SS_SCL_HCNT);
	writel(SS_SCL_LOW, rI2C_SS_SCL_LCNT);
	writel(FS_SCL_HIGH, rI2C_FS_SCL_HCNT);
	writel(FS_SCL_LOW, rI2C_FS_SCL_LCNT);
	writel(HS_SCL_HIGH, rI2C_HS_SCL_HCNT);
	writel(HS_SCL_LOW, rI2C_HS_SCL_LCNT);
	writel(SDA_SETUP_DELAY, rI2C_SDA_SETUP);

	reg_mid = readl(rI2C_COMP_PARAM_1);
	mylw = (((reg_mid>>16)&0xFF)+1)>>1;
	i2c_tf_tl = mylw;
	writel(mylw, rI2C_TX_TL);
	mylw = (((reg_mid>>8)&0xFF)+1)>>1;
	i2c_rf_tl = mylw + 1;
	writel(mylw, rI2C_RX_TL);

	reg_mid = readl(rI2C_CON);
	reg_mid &= ~0x06;
	reg_mid |= (speedmode << 1);
	writel(reg_mid, rI2C_CON);     /*1: standard; 2: fast; 3: high*/
#ifdef CONFIG_PM_AT2600
        writel(0x63, rI2C_CON);
#endif
	i2c_tx_rx_byte_max = i2c_tf_tl < i2c_rf_tl ? i2c_tf_tl : i2c_rf_tl;
	DPRINTF("i2c_tx_rx_byte_max is %d\n", i2c_tx_rx_byte_max);
}

static void i2c_enable(void)
{
	writel(1, rI2C_ENABLE);
}
static bool i2c_check(uchar chip, uint addr, int alen, int buflen)
{
	if (alen > 2 || alen <= 0) {
		DPRINTF("controller don't support %d byte address\n", alen);
		return FALSE;
	}
	
	if (buflen > i2c_tx_rx_byte_max || buflen <= 0) {
		DPRINTF("improper read/write size: %d\n", buflen);
		return FALSE;
	}

	return TRUE;
}

static bool i2c_is_busy(void)
{
	if( ( readl(rI2C_STATUS)&0x21)!=0 )
	return TRUE;
	else
	return FALSE;
}

static bool wait_for_bus_idle(void)
{
	int timeout = 0;

	udelay(10);
	while(timeout < I2C_MAX_TIMEOUT){
		if(i2c_is_busy()){
			udelay(5);
			timeout += 5;
		} else {
			break;
		}
	};

	if (timeout < I2C_MAX_TIMEOUT){
		return TRUE;
	}else {
		DPRINTF("%s, timeout!\n",__FUNCTION__);
		return FALSE;	
	}
}

static bool i2c_is_tfnf(void)
{
	if( ( readl(rI2C_STATUS)&0x2)!=0 )
		return TRUE;
	else
		return FALSE;
}

static bool i2c_is_tfe(void)
{
	if( ( readl(rI2C_STATUS)&0x4)!=0 )
		return TRUE;
	else
		return FALSE;
}


static bool i2c_is_rff(void)
{
	if( ( readl(rI2C_STATUS)&0x10)!=0 )
		return TRUE;
	else
		return FALSE;
}

static bool i2c_is_rfne(void)
{
	if( ( readl(rI2C_STATUS)&0x8)!=0 )
		return TRUE;
	else
		return FALSE;
}
static bool i2c_is_tx_abort(void)
{
	if( (readl(rI2C_RAW_INTR_STAT)&0x40) !=0 )
		return TRUE;
	else 
		return FALSE;
}

static bool i2c_is_tx_empty(void)
{
	if( (readl(rI2C_RAW_INTR_STAT)&0x10)!=0)
		return TRUE;
	else
		return FALSE;
}

static bool i2c_is_tx_over(void)
{
	if( (readl(rI2C_RAW_INTR_STAT)&0x8)!=0)
		return TRUE;
	else
		return FALSE;
}

static bool i2c_is_rx_full(void)
{
	if( (readl(rI2C_RAW_INTR_STAT)&0x4)!=0)
		return TRUE;
	else
		return FALSE;
}	

static bool i2c_is_rx_over(void)
{
	if( (readl(rI2C_RAW_INTR_STAT)&0x2)!=0)
		return TRUE;
	else
		return FALSE;
}	

static bool i2c_is_rx_under(void)
{
	if((readl(rI2C_RAW_INTR_STAT)&0x1)!=0)
		return TRUE;
	else
		return FALSE;
}

static int i2c_transfer(u8 cmd_type, uchar *buf, int len)
{
	int i, ret;
	u32 timeout = 0;
	u8	buf_to_xfer, buf_to_read;

	DPRINTF("i2c_xfer,CMD %d, len %d\n",cmd_type, len );
	if( (I2C_M_RD == cmd_type)&& (len > i2c_tx_rx_byte_max) ) {
		DPRINTF("I2C read return bytes exceed the maximum in an INTR XFER\n");
		return -1;
	}

	wait_for_bus_idle();
	switch(cmd_type){
	case I2C_M_WR: 
		DPRINTF("I2C_M_WR P1 %s\n", __FUNCTION__);
		i = 0;
		buf_to_xfer = len;
		do {
			while ((i < len) && (i2c_is_tfnf())) {
				writel(buf[i], rI2C_DATA_CMD);
				i++;
				buf_to_xfer--;
			}
			DPRINTF("I2C_M_WR P2 %s\n", __FUNCTION__);	
			if(i == len) {
				wait_for_bus_idle();
				if(i2c_is_tx_abort()){
					DPRINTF("abort cause is %x\n", readl(rI2C_TX_ABRT_SOURCE));
					readl(rI2C_CLR_TX_ABRT);
					return -1;
				}
			} else if(!i2c_is_tfnf()){
				while(timeout < I2C_MAX_TIMEOUT){
					if(i2c_is_tx_empty())
						break;
					else {
						udelay(5);
						timeout += 5;
					}
				};
				if (timeout < I2C_MAX_TIMEOUT)
					continue;/* continue write TX FIFO */
				else {
					DPRINTF("%s, timeout!\n",__FUNCTION__);
					return -1;	
				}
			}else
				continue; 
	
		} while( buf_to_xfer > 0);
		DPRINTF("I2C_M_WR P3 %s\n", __FUNCTION__);		
		ret = i;
		break;
	case I2C_M_RD:	
		DPRINTF("I2C_M_RD P1 %s\n", __FUNCTION__);
#ifndef CONFIG_PM_AT2600
		for(i = 0; i < len; i++){
			writel(0x100, rI2C_DATA_CMD);
		}
#endif
		i = 0;
		buf_to_read = len;
		do {
			timeout = 0;
			while ((i < len) && (timeout < I2C_MAX_TIMEOUT)) {
				if (i2c_is_rfne()){
					buf[i] = readl(rI2C_DATA_CMD);
					i++;
					buf_to_read--;
				} else {
					udelay(5);
					timeout += 5;
				}
				
			}
			if(i == len) {
				if(i2c_is_tx_abort() 
					&& ((readl(rI2C_TX_ABRT_SOURCE)& 0xfffe) != 0)){
					DPRINTF("abort cause is 0x%x\n", readl(rI2C_TX_ABRT_SOURCE));
					readl(rI2C_CLR_TX_ABRT);
					return FALSE;
				} else {
					DPRINTF("I2C_M_RD P2 %s, timeout is %d\n", __FUNCTION__, timeout);
				}	
			} else if(!i2c_is_rfne()){
				timeout = 0;
				while(timeout < I2C_MAX_TIMEOUT){
					if(i2c_is_rx_full()){
						break;
					}else {
						udelay(5);
						timeout +=5;
					}
				};
				if (i2c_is_rfne())
					continue; /* continue read RX FIFO */
				else{
					DPRINTF("%s, timeout!\n",__FUNCTION__);
					return -1;
				}
			} else {
				DPRINTF("%s, read %d, status %x\n", __FUNCTION__, i, readl(rI2C_STATUS));
				continue;
			}
		}while( buf_to_read > 0);
		DPRINTF("I2C_M_RD P3 %s\n", __FUNCTION__);
		ret = i;
		break;
	}
	return ret;
}


void i2c_init(int speed, int slaveaddr)
{
	return;
}

/*
 * Probe the given I2C chip address.  Returns 0 if a chip responded,
 * not 0 on failure. not implement;
 */
int i2c_probe(u8 chip)
{
	return -1;
}

int i2c_read(uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int ret;
	u8 xaddr[2];
	DPRINTF("%s chip: 0x%02x addr: 0x%04x alen: %d len: %d\n",__FUNCTION__, chip, addr, alen, len); 

	if (alen == 2){
		xaddr[0] = (addr>>8)& 0xff;
		xaddr[1] = addr & 0xff;
	}else if (alen == 1){
		xaddr[0] = addr & 0xff;
	}

	writel(chip, rI2C_TAR);
#ifdef CONFIG_PM_AT2600
        writel(xaddr[0], rI2C_DATA_CMD);
        writel(0x100, rI2C_DATA_CMD);
#else
	ret = i2c_transfer(I2C_M_WR, xaddr, alen);
	if(ret!= alen){
		DPRINTF("\n%s, Start not acked\n", __FUNCTION__);
		return -1;
	}
#endif
	ret = i2c_transfer(I2C_M_RD, buffer, len);
	if(ret!= len){
		DPRINTF("\n%s, Read not finished yet\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

int i2c_write(uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int ret;
	u8 xaddr[MAX_BUFFER_LEN];
	DPRINTF("%s chip: 0x%02x addr: 0x%04x alen: %d len: %d\n",__FUNCTION__, chip, addr, alen, len); 
	
	if (alen == 2){
			xaddr[0] = (addr>>8)& 0xff;
			xaddr[1] = addr & 0xff;
	}else if (alen == 1){
			xaddr[0] = addr & 0xff;
	}	

	memcpy(&xaddr[alen], buffer, len);
	writel(chip, rI2C_TAR);
	ret = i2c_transfer(I2C_M_WR, xaddr, alen+len);
	if(ret!= (len+alen)){
		DPRINTF("\nWrite not finished yet\n", __FUNCTION__);
		return -1;
	}
	return 0;
}


void at6600_i2c_init()
{
	i2c_ctl_init(1, 0x61);	
    	/* PMU only response at delay some flip-flops;*/
	writel(0x7, I2CCTL);
	i2c_enable();
}


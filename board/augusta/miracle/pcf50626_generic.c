/*------------------------------------------------------------------------------
* (c) Copyright, Augusta Technology, Inc., 2006-present.
* (c) Copyright, Augusta Technology USA, Inc., 2006-present.
*
* This software, document, web pages, or material (the "Work") is copyrighted
* by its respective copyright owners.  The Work may be confidential and
* proprietary.	The Work may be further protected by one or more patents and
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
#include <asm/arch-atxx/regs_base.h>
#include <asm/arch-atxx/pmu.h>
#include <asm/arch-atxx/gpio.h>
#include <i2c.h>
#include "pcf50626_regs.h"
#include "power_table.c"

#define PCF50626_ID		0x35
#define PCF50626_ADDR		0x70

//#define DEBUG
#ifdef DEBUG
#define DPRINTF(args...) printf(args)
#else
#define DPRINTF(args...)
#endif

static void pcf50626_write_reg(u8 reg_addr, u8 reg_value)
{
	i2c_reg_write(PCF50626_ADDR, reg_addr, reg_value);
}

static u8 pcf50626_read_reg(u8 reg_addr, u8 *reg_value)
{
	*reg_value = i2c_reg_read(PCF50626_ADDR, reg_addr);
	return *reg_value;
}
/*----------------------------------------------------------------------------*/

/*--------------------power operation-----------------------------------*/

void pcf50626_RFx_power_supply(u32 voltage, power_supply_mode mode, u8 rfx_ctr_reg_start)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( rfx_ctr_reg_start, reg1);
	pcf50626_write_reg( rfx_ctr_reg_start+2, 0x00);
	pcf50626_write_reg( rfx_ctr_reg_start+1, reg3);

}

void pcf50626_Dx_power_supply(u32 voltage, power_supply_mode mode, u8 dcd_ctr_reg_start)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;

	pcf50626_write_reg( dcd_ctr_reg_start, reg1);
	pcf50626_write_reg( dcd_ctr_reg_start+2, 0x00);
	pcf50626_write_reg( dcd_ctr_reg_start+1, reg3);
}

void pcf50626_DCD1_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( DCD1C1, reg1);
	pcf50626_write_reg( DCD1C3, 0x00);
	pcf50626_write_reg( DCD1C4, 0xf);
	pcf50626_write_reg( DCD1C2, reg3);
}

void pcf50626_DCD2_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( DCD2C1, reg1);
	pcf50626_write_reg( DCD2C3, 0x00);
	pcf50626_write_reg( DCD2C4, 0xf);
	pcf50626_write_reg( DCD2C2, reg3);
}

void pcf50626_DCUD_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 2700)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( DCUDC1, reg1);
	pcf50626_write_reg( DCUDC3, 0x00);
	pcf50626_write_reg( DCUDC4, 0xf);
	pcf50626_write_reg( DCUDC2, reg3);
}

void pcf50626_IOVDD_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( IOREGC1, reg1);
	pcf50626_write_reg( IOREGC3, 0x00);
	pcf50626_write_reg( IOREGC2, reg3);
}

void pcf50626_USBREG_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( USBREGC1, reg1);
	pcf50626_write_reg( USBREGC3, 0x00);
	pcf50626_write_reg( USBREGC2, reg3);
}

void pcf50626_HCVDD_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( HCREGC1, reg1);
	pcf50626_write_reg( HCREGC3, 0x00);
	pcf50626_write_reg( HCREGC2, reg3);
}

void pcf50626_LCVDD_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( LCREGC1, reg1);
	pcf50626_write_reg( LCREGC3, 0x00);
	pcf50626_write_reg( LCREGC2, reg3);

}

void pcf50626_USIMVDD_power_supply(u32 voltage, power_supply_mode mode)
{
	u32 reg1, reg3;

	reg1 = (voltage - 600)/25;
	reg3 = mode? 0xe0 : 0x00;
	pcf50626_write_reg( USIMREGC1, reg1);
	pcf50626_write_reg( USIMREGC3, 0x00);
	pcf50626_write_reg( USIMREGC2, reg3);
}


static u8 pcf50626_set_power_supply(ps_setting_t ps, power_supply_mode mode)
{
	DPRINTF("pn = %s; module = %d;"
				"reg = 0x%02x; v = %04d; mode = %s\n",
				ps.ps_name, ps.module, ps.reg_idx, ps.voltage,
				mode ? "on":"off");
	switch (ps.reg_idx) {
	case D1REGC1:
	case D2REGC1:
	case D3REGC1:
	case D4REGC1:
	case D5REGC1:
	case D6REGC1:
	case D7REGC1:
	case D8REGC1:
		pcf50626_Dx_power_supply(ps.voltage, mode, ps.reg_idx);
		break;
	case RF1REGC1:
	case RF2REGC1:
	case RF3REGC1:
	case RF4REGC1:
		pcf50626_RFx_power_supply(ps.voltage, mode, ps.reg_idx);
		break;
	case IOREGC1:
		pcf50626_IOVDD_power_supply(ps.voltage, mode);
		break;
	case USBREGC1:
		pcf50626_USBREG_power_supply(ps.voltage, mode);
		break;
	case USIMREGC1:
		pcf50626_USIMVDD_power_supply(ps.voltage, mode);
		break;
	case LCREGC1:
		pcf50626_LCVDD_power_supply(ps.voltage, mode);
		break;
	case HCREGC1:
		pcf50626_HCVDD_power_supply(ps.voltage, mode);
		break;
	case DCD1C1:
		pcf50626_DCD1_power_supply(ps.voltage, mode);
		break;
	case DCD2C1:
		pcf50626_DCD2_power_supply(ps.voltage, mode);
		break;
	case DCUDC1:
		pcf50626_DCUD_power_supply(ps.voltage, mode);
		break;
	default:
		DPRINTF("%s, Wrong branch\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/**
  * set_default_power_supply - Change power supply to default
  *
  */
int default_power_supply(void)
{
	power_supply_mode	mode;
	u32	i;
	u8	ret = 0;

	/* find power supply first */
	for (i = 0; i < PS_SETTING_COUNT; i++) {
		mode = ps_setting_default[i].default_mode;
		ret |= pcf50626_set_power_supply(ps_setting_default[i], mode);
		udelay(1000);
	}

	return ret;
}

int pmu_power_control(power_supply_component module, power_supply_mode mode)
{
	u32	i;
	u8	ret = 0;

	if (PPS_COMMON == module){
		DPRINTF("%s, operation permitted\n", __FUNCTION__);
		return -1;
	}
	/* find power supply module first */
	for (i = 0; i < PS_SETTING_COUNT; i++) {
		if (ps_setting_default[i].module == module) {
			ret |= pcf50626_set_power_supply(ps_setting_default[i], mode);
			ps_setting_default[i].cur_mode = mode;
			udelay(1000);
		}
	}
	
	return ret;
}

void power_on_detect (void)
{
	u8 reg_val;
	u32 t1, t2;

	/*reset the ON/OFF timeout timer*/
	pcf50626_read_reg (OOCC1, &reg_val);
	reg_val |= PCF50626_OOCC1_TOT_RST;
	pcf50626_write_reg (OOCC1, reg_val);

	pcf50626_read_reg (INT1, &reg_val);
	if ((reg_val & PCF50626_INT1_ONKEYF) == 0)
	{
		return;
	}
	if ((reg_val & PCF50626_INT1_ONKEYR) != 0)
	{
		goto power_off;
	}

	t1 = get_timer (0);
	do {
		t2 = get_timer (0);
		if ((t2 - t1) >= 1000)
			break;
	} while (1);

	pcf50626_read_reg (INT1, &reg_val);
	if ((reg_val & PCF50626_INT1_ONKEYR) != 0)
	{
		goto power_off;
	}

	return;

power_off:
	printf ("\n\rMiss power on,you must press power key more than 1s, turn off!\n");
	pcf50626_write_reg (OOCC1, PCF50626_OOCC1_GO_OFF);
	while (1);
}

int pmu_init(void)
{
	u8 buf;

	/* read a readonly register */
	pcf50626_read_reg (ID, &buf);

	if (buf != PCF50626_ID){
		printf ("\nPMU read ID failed, wrong id value = 0x%x", buf);
		return -1;
	}

	DPRINTF("******* pcf50626 read ID succeed *******\n");

	power_on_detect ();
	default_power_supply();

	return 0;
}

void pmu_power_show()
{
	ps_setting_t *ps;
	u32 i;
	u8 reg1, reg2;

	printf("******* pcf50626 power array begin*******\n");
	for (i = 0; i < PS_SETTING_COUNT; i++){
		ps = &ps_setting_default[i];
		printf("pn = %s; module = %d;"
			"reg = 0x%02x; v = %04d; regout =0x%02x, mode = %s\n",
			ps->ps_name, ps->module, ps->reg_idx, ps->voltage,
			pcf50626_read_reg(ps->reg_idx, &reg1),
			(pcf50626_read_reg(ps->reg_idx+1, &reg2)>>5) ? "on":"off");
	}
	printf("******* pcf50626 power array end*******\n");
}

void set_backlight(u8 dimfreq, u8 ledman)
{
	int ret = 0;
	ret = atxx_request_gpio(GPIO_LCD_BL_DIM);
	if (ret) {
		printf("Failed to request gpio LCD backlight %d!\n", ret);
		return;
	}

	atxx_set_gpio_direction(GPIO_LCD_BL_DIM, 0);
	atxx_gpio_set(GPIO_LCD_BL_DIM, 1);

	atxx_free_gpio(GPIO_LCD_BL_DIM);
}

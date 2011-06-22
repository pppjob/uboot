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
#include <asm/arch-atxx/pm.h>
#include <asm/arch-atxx/aboot.h>
#include <asm/arch-atxx/gpio.h>
#include <i2c.h>
#include "at2600_pm_regs.h"
#include "at2600_pm_generic.h"
#include "power_table.c"

#define AT2600_PM_ID		0x1
#define AT2600_PM_ADDR		0x70

#ifdef DEBUG
#define DPRINTF printf
#else
#define DPRINTF(args...)
#endif

static void at2600_pm_write_reg(u8 reg_addr, u8 reg_value)
{
	i2c_reg_write(AT2600_PM_ADDR, reg_addr, reg_value);
}

static u8 at2600_pm_read_reg(u8 reg_addr, u8 *reg_value)
{
	*reg_value = i2c_reg_read(AT2600_PM_ADDR, reg_addr);
	return *reg_value;
}
/*----------------------------------------------------------------------------*/

/*--------------------power operation-----------------------------------*/


int at2600_pm_sink_led_power_supply(u8 voltage,
		power_supply_mode mode,
		u8 ldo_ctr_reg)
{


    return 0;



}



int at2600_pm_ldo_int_power_supply(u8 voltage,
		power_supply_mode mode,
		u8 ldo_ctr_reg)
{
	u8		ldoregc1;
	
	ldoregc1 = 0;
	
	DPRINTF("voltage = 0x%x, mode = 0x%x,"
			"ldo_ctr_reg = 0x%x\n",
			voltage, mode, ldo_ctr_reg);
	
    /*input parameter check*/
    if ((voltage < LDO_VOL_MIN_IDX) || (voltage > LDO_VOL_MAX_IDX)) {
        DPRINTF("wrong power supply for LDO\n");
        return -EINVAL;
    }

	/*set mode and voltage*/	
    ldoregc1 |= voltage;
    at2600_pm_write_reg(ldo_ctr_reg,ldoregc1);
	DPRINTF("wldoregc1 = 0x%x,rldoregc1 = 0x%x\n\r",ldoregc1,at2600_pm_read_reg(ldo_ctr_reg,&ldoregc1));
	return 0;
}


int at2600_pm_ldo_power_supply(u8 voltage,
		power_supply_mode mode,
		u8 ldo_ctr_reg)
{
	u8		ldoregc1;

	ldoregc1 = 0;
	
	DPRINTF("voltage = 0x%x, mode = 0x%x,"
			"ldo_ctr_reg = 0x%x\n",
			voltage, mode, ldo_ctr_reg);
	
    /*input parameter check*/
    if ((voltage < LDO_VOL_MIN_IDX) || (voltage > LDO_VOL_MAX_IDX)) {
        printf("wrong power supply for LDO\n");
        return -EINVAL;
    }
	
	if((PS_OFF != mode) && (PS_ON != mode)){
        printf("power supply mode invalid\n");
        return -EINVAL;
	}
	
	/*set mode and voltage*/	
	if (mode == PS_OFF) {
	    ldoregc1 |= LDO_POWER_DOWN << LDO_POWER_SHIFT;	

	} else if (mode == PS_ON) {
	    ldoregc1 |= LDO_POWER_ON << LDO_POWER_SHIFT; 
	}
    ldoregc1 |= voltage;
	
    at2600_pm_write_reg(ldo_ctr_reg, ldoregc1);
	
	DPRINTF("wldoregc1 = 0x%x,rldoregc1 = 0x%x\n\r",ldoregc1,at2600_pm_read_reg(ldo_ctr_reg, &ldoregc1));
	return 0;
}



static int at2600_pm_set_power_supply(ps_setting_t ps, power_supply_mode mode)
{
	int	ret = 0;

	DPRINTF("pn = %s; module = %d;"
				"reg = 0x%02x; v = %04d; mode = %s\n",
				ps.ps_name, ps.module, ps.reg_idx, ps.voltage,
				mode ? "on":"off");


	switch (ps.reg_idx) {
	case AT2600_PM_REG_S1V2C1:
	case AT2600_PM_REG_S1V8C1:
	case AT2600_PM_REG_VBUSC1:
	case AT2600_PM_REG_CMRC1:
	case AT2600_PM_REG_D3V0C1:
	case AT2600_PM_REG_SIM1C1:
	case AT2600_PM_REG_SIM2C1:
	case AT2600_PM_REG_SDC1:
	case AT2600_PM_REG_FOCUSC1:
	case AT2600_PM_REG_USBC1:
	case AT2600_PM_REG_ABBC1:
	case AT2600_PM_REG_IMGC1:
	case AT2600_PM_REG_R2V8C1:
	case AT2600_PM_REG_R3V0C1:
	case AT2600_PM_REG_A2V5C1:
		
		ret = at2600_pm_ldo_power_supply(ps.voltage, mode, ps.reg_idx);
		break;


    case AT2600_PM_REG_INTC1:
        ret = at2600_pm_ldo_int_power_supply(ps.voltage, mode, ps.reg_idx);
	    break;

    case AT2600_PM_REG_LED1C1:
	case AT2600_PM_REG_LED2C1:
	case AT2600_PM_REG_VIBRC1:
		ret = at2600_pm_sink_led_power_supply(ps.voltage, mode, ps.reg_idx);
	    break;

	default:
		DPRINTF("Wrong branch\n");
		ret = -EINVAL;
	}

	return ret;
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
		DPRINTF("name=%s  ",ps_setting_default[i].ps_name);
		ret |= at2600_pm_set_power_supply(ps_setting_default[i], mode);
		udelay(1000);
	}
    DPRINTF("\nret=%x\n",ret);
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
			ret |= at2600_pm_set_power_supply(ps_setting_default[i], mode);
			ps_setting_default[i].cur_mode = mode;
			udelay(1000);
		}
	}
	
	return ret;
}

void power_on_detect (void)
{
	u8 reg_val;
	u32 t1, t2, swcfg;
	u8 int1_mask_reg;

	int1_mask_reg = 0;
	if (hwcfg_detect() != NAND_BOOT) {
		return;
	}

	swcfg = pm_read_reg(SWCFGR);
	if (swcfg & SWCFGR_POWERON_XLOAD) {
		swcfg &= ~SWCFGR_POWERON_XLOAD;
		pm_write_reg(SWCFGR, swcfg);
		return;
	}

	/* power on if usb charger is connected */
	at2600_pm_read_reg (AT2600_PM_REG_OOCS, &reg_val);
	if (reg_val & 0x20) {
		reg_val = 0x07;
		at2600_pm_write_reg (AT2600_PM_REG_CBCC3, reg_val);
		printf("usb charger connected.\n");
		return;
	}

	swcfg = pm_read_reg(SWCFGR);
	if (swcfg & SWCFGR_REBOOT_MASK) {
		return;
	}

#if 0
	t1 = get_timer (0);
	do {
		t2 = get_timer (0);
		if ((t2 - t1) >= 1000)
			break;
	} while (1);

#endif
	/*unmask onkey  rising */
	at2600_pm_read_reg (AT2600_PM_REG_M_INT1, &int1_mask_reg);
	int1_mask_reg &= ~AT2600_PM_INT1_ONKEYR;
	at2600_pm_write_reg(AT2600_PM_REG_M_INT1,int1_mask_reg);

	at2600_pm_read_reg (AT2600_PM_REG_INT1, &reg_val);

	if ((reg_val & AT2600_PM_INT1_ONKEYR) != 0)
	{
		goto power_off;
	}

	t1 = get_timer (0);
	do {
		t2 = get_timer (0);
		if ((t2 - t1) >= 1000)
			break;
	} while (1);

	return;

	return;

power_off:
	printf ("\n\rMiss power on,you must press power key more than 1s, turn off!\n");
	at2600_pm_write_reg (AT2600_PM_REG_OOCC1, AT2600_PM_OOCC1_GO_OFF);
	while (1);
}

uint32_t adc_get_pmu(void)
{
	uint16_t        adc_result = 0;
	uint8_t         adcc1, adcs1, adcs3, reg_val;

#if 0
	adcc1 = 0x8 | (0x1 << 4);
	pcf50626_write_reg(ADCC1, adcc1);

	pcf50626_read_reg(ADCC1, &adcc1);

	/* set start adc command by writing 1 to bit[0] of ADCC1 */
	adcc1 = (adcc1 & 0xFE) | 1;
	pcf50626_write_reg(ADCC1, adcc1);

	do {
		pcf50626_read_reg(INT3, &reg_val);
	} while ((reg_val & PCF50626_INT3_ADCRDY) == 0);

	pcf50626_read_reg(ADCS1, &adcs1);
	adc_result = adcs1 << 2;

	pcf50626_read_reg(ADCS3, &adcs3);
	adc_result |= (adcs3 & 0x3);
#endif        
	return adc_result;
}


int pmu_init(void)
{
	u8 buf;

	/* read a readonly register */
	at2600_pm_read_reg (AT2600_PM_REG_ID, &buf);
	if (buf != AT2600_PM_ID){
		printf ("\nPMU read ID failed, wrong id value = 0x%x", buf);
		return -1;
	}
	DPRINTF("******* at2600_pm read ID succeed *******\n");
	power_on_detect ();
	DPRINTF("uboot power supply befor\n");
	default_power_supply();

	return 0;
}

void pmu_power_show()
{
	ps_setting_t *ps;
	u32 i;
	u8 reg1, reg2;


	for (i = 0; i < PS_SETTING_COUNT; i++){
		ps = &ps_setting_default[i];
		printf("pn = %s; module = %d;"
				"reg = 0x%02x; v = %04d; regout =0x%02x, mode = %s\n",
				ps->ps_name, ps->module, ps->reg_idx, ps->voltage,
				at2600_pm_read_reg(ps->reg_idx, &reg1),
				(at2600_pm_read_reg(ps->reg_idx+1, &reg2)>>5) ? "on":"off");
	}
}

void set_backlight(u8 dimfreq, u8 ledman)
{
	uint8_t reg;
	int ret = 0;
	dimfreq = 0x4;
	at2600_pm_read_reg(AT2600_PM_REG_PWM1S, &reg);
	reg = (reg | PWMx_SELECT) | (dimfreq << 5)| PWMx_POLARITY;

	printf("set back light reg:%x\n",reg);

	at2600_pm_write_reg(AT2600_PM_REG_PWM1S, reg);
	at2600_pm_write_reg(AT2600_PM_REG_PWM1D, ((0x7f - ledman) << 1) & 0xff);

	/* power on */
	at2600_pm_write_reg(AT2600_PM_REG_GPIO1C1, GPIOx_PWM1_OUTPUT);

	at2600_pm_write_reg(AT2600_PM_REG_GPIO2C1, GPIOx_PWM1_OUTPUT);

	at2600_pm_write_reg(AT2600_PM_REG_GPIO3C1, GPIOx_PWM1_OUTPUT);

	at2600_pm_write_reg(AT2600_PM_REG_GPIO4C1, GPIOx_PWM1_OUTPUT);

	at2600_pm_write_reg(AT2600_PM_REG_GPIO5C1, GPIOx_PWM1_OUTPUT);

	at2600_pm_write_reg(AT2600_PM_REG_GPIO6C1, GPIOx_PWM1_OUTPUT);
	ret = atxx_request_gpio(GPIO_LCD_BL_EN);
	if (ret) {
		printf("Failed to request gpio LCD backlight %d!\n", ret);
		return;
	}

	atxx_set_gpio_direction(GPIO_LCD_BL_EN, 0);
	atxx_gpio_set(GPIO_LCD_BL_EN, 1);
	atxx_free_gpio(GPIO_LCD_BL_EN);
}

/* put whole system to shutdown  */
int at2600_pm_shutdown_system(void)
{
	uint8_t		reg_oocc1;

	/* set alarm and put system into off state */
	at2600_pm_read_reg(AT2600_PM_REG_OOCC1,&reg_oocc1);
	reg_oocc1 |= AT2600_PM_OOCC1_GO_OFF;
	at2600_pm_write_reg(AT2600_PM_REG_OOCC1,reg_oocc1);

	return 0;
}


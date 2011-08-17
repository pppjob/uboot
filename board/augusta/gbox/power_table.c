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
#ifndef	___PMU_TABLE_H_______
#define	___PMU_TABLE_H_______

typedef struct ps_setting {
	u8			reg_idx;
	uchar			ps_name[20];
	power_supply_component	module;

	u32			voltage;
	power_supply_mode	default_mode;
	power_supply_mode	cur_mode;
} ps_setting_t;

/* array order is same as struct div_name */
static ps_setting_t ps_setting_default[] = {
    {AT2600_PM_REG_INTC1,    "VINT",     PPS_COMMON,  INTC1_DOUT_2V5,   PS_ON,   PS_ON}, 
    {AT2600_PM_REG_S1V2C1,   "S1V2",     PPS_COMMON,  S1V2C1_DOUT_1V35, PS_ON,   PS_ON},
    {AT2600_PM_REG_S1V8C1,   "S1V8",     PPS_COMMON,  S1V8C1_DOUT_1V8,  PS_ON,   PS_ON},
    {AT2600_PM_REG_VBUSC1,   "VBUS",     PPS_VBUS,    VBUSC1_DOUT_5V0,  PS_OFF,  PS_OFF},
    {AT2600_PM_REG_CMRC1,    "CMR",      PPS_CAMERA,  CMRC1_DOUT_1V8,   PS_OFF,  PS_OFF},
    {AT2600_PM_REG_D3V0C1,   "D3V0",	 PPS_COMMON,  D3V0C1_DOUT_3V0,  PS_ON,   PS_ON},
    {AT2600_PM_REG_SIM1C1,   "SIM1",	 PPS_SIM,     SIM1C1_DOUT_3V0,  PS_OFF,  PS_OFF},
    {AT2600_PM_REG_SIM2C1,   "SIM2",	 PPS_SIM,     SIM2C1_DOUT_3V0,  PS_OFF,  PS_OFF},
    {AT2600_PM_REG_SDC1,     "SDC",      PPS_SD,      SDC1_DOUT_3V0,    PS_ON,  PS_ON},
    {AT2600_PM_REG_FOCUSC1,  "FOCUS",    PPS_CAMERA,  FOCUSC1_DOUT_3V0, PS_OFF,   PS_OFF},
    {AT2600_PM_REG_USBC1,    "USB",      PPS_COMMON,  USBC1_DOUT_3V3,   PS_ON,  PS_ON},
    {AT2600_PM_REG_ABBC1,    "ABB",      PPS_COMMON,  ABBC1_DOUT_2V5,   PS_ON,   PS_ON},
    {AT2600_PM_REG_IMGC1,    "IMG",      PPS_CAMERA,  IMGC1_DOUT_2V8,   PS_OFF,   PS_OFF},
    {AT2600_PM_REG_R2V8C1,   "R2V8",	 PPS_COMMON,  R2V8C1_DOUT_2V8,  PS_ON,   PS_ON}, 
    {AT2600_PM_REG_R3V0C1,   "R3V0",	 PPS_GPS,     R3V0C1_DOUT_3V0,  PS_OFF,  PS_OFF},
    {AT2600_PM_REG_A2V5C1,   "A2V5",	 PPS_COMMON,  A2V5C1_DOUT_2V5,  PS_ON,   PS_ON},
};

#define	PS_SETTING_COUNT		ARRAY_SIZE(ps_setting_default)

#endif	/*___PMU_TABLE_H_______*/


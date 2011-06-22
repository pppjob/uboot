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
 ---------------------------------------------------------------------------*/

#ifndef	___AT2600_PM_GENERIC_H_______
#define	___AT2600_PM_GENERIC_H_______

/*-----------------------------------------------------------------------------
 * The following is for Power Supply
 ----------------------------------------------------------------------------*/
 /*
*define LDO vol range
*define LDO vol range mask
*define LDO vol bit
*/
#define LDO_VOL_MIN_IDX 0x0
#define LDO_VOL_MAX_IDX 0x7
#define LDO_VOL_MASK (~0x7)
#define LDO_POWER_SHIFT 0x7
#define LDO_SLEEP_MODE_SHIFT 0x06

#define LDO_POWER_SWITCH_MASK (~(0x1 << LDO_POWER_SHIFT))

/*dcdled vol shift bit*/
#define DCDLED_VOL_MODE_SHIFT            0x3
#define DCDLED_STEP_COUNT_SHIFT          0x0
#define DCDLED_POWER_SHIFT               0x7

enum ldo_power_switch{
	 LDO_POWER_ON,
	 LDO_POWER_DOWN,	
};

enum ldo_sleep_mode{
     LDO_SLEEP_MODE_MANUAL,
	 LDO_SLEEP_MODE_PWREN,

};
/*power supply     */
enum at2600_pm_reg_dcd_intc1{
	INTC1_DOUT_2V5,
	INTC1_DOUT_2V55,	
	INTC1_DOUT_2V6,
	INTC1_DOUT_2V65,
	INTC1_DOUT_2V3,
	INTC1_DOUT_2V35,
	INTC1_DOUT_2V4,
	INTC1_DOUT_2V45,
};


enum at2600_pm_reg_s1v2c1{
	S1V2C1_DOUT_1V2,
	S1V2C1_DOUT_1V25,	
	S1V2C1_DOUT_1V0,
	S1V2C1_DOUT_1V1,
	S1V2C1_DOUT_1V4,
	S1V2C1_DOUT_1V45,
	S1V2C1_DOUT_1V3,
	S1V2C1_DOUT_1V35,
};

enum at2600_pm_reg_s1v8c1{
	S1V8C1_DOUT_1V8,
	S1V8C1_DOUT_1V85,	
	S1V8C1_DOUT_1V9,
	S1V8C1_DOUT_1V95,
	S1V8C1_DOUT_1V6,
	S1V8C1_DOUT_1V65,
	S1V8C1_DOUT_1V7,
	S1V8C1_DOUT_1V75,
};


enum at2600_pm_reg_vbusc1{
	VBUSC1_DOUT_5V0,
	VBUSC1_DOUT_5V1,	
	VBUSC1_DOUT_5V2,
	VBUSC1_DOUT_5V3,
	VBUSC1_DOUT_4V6,
	VBUSC1_DOUT_4V7,
	VBUSC1_DOUT_4V8,
	VBUSC1_DOUT_4V9,

};


enum at2600_pm_reg_cmrc1{
	CMRC1_DOUT_1V8,
	CMRC1_DOUT_1V85,	
	CMRC1_DOUT_1V9,
	CMRC1_DOUT_1V95,
	CMRC1_DOUT_1V6,
	CMRC1_DOUT_1V65,
	CMRC1_DOUT_1V7,
	CMRC1_DOUT_1V75,

};


enum at2600_pm_reg_d3v0c1{
	D3V0C1_DOUT_3V0,
	D3V0C1_DOUT_3V05,	
	D3V0C1_DOUT_3V1,
	D3V0C1_DOUT_3V15,
	D3V0C1_DOUT_2V8,
	D3V0C1_DOUT_2V85,
	D3V0C1_DOUT_2V9,
	D3V0C1_DOUT_2V95,

};



enum at2600_pm_reg_sim1c1{
	SIM1C1_DOUT_3V0,
	SIM1C1_DOUT_3V05,	
	SIM1C1_DOUT_3V1,
	SIM1C1_DOUT_3V15,
	SIM1C1_DOUT_2V8,
	SIM1C1_DOUT_2V85,
	SIM1C1_DOUT_2V9,
	SIM1C1_DOUT_2V95,

};




enum at2600_pm_reg_sim2c1{
	SIM2C1_DOUT_3V0,
	SIM2C1_DOUT_3V05,	
	SIM2C1_DOUT_3V1,
	SIM2C1_DOUT_3V15,
	SIM2C1_DOUT_2V8,
	SIM2C1_DOUT_2V85,
	SIM2C1_DOUT_2V9,
	SIM2C1_DOUT_2V95,

};


enum at2600_pm_reg_sdc1{
	SDC1_DOUT_3V0,
	SDC1_DOUT_3V05,	
	SDC1_DOUT_3V1,
	SDC1_DOUT_3V15,
	SDC1_DOUT_2V8,
	SDC1_DOUT_2V85,
	SDC1_DOUT_2V9,
	SDC1_DOUT_2V95,

};



enum at2600_pm_reg_focusc1{
	FOCUSC1_DOUT_3V0,
	FOCUSC1_DOUT_3V05,	
	FOCUSC1_DOUT_3V1,
	FOCUSC1_DOUT_3V15,
	FOCUSC1_DOUT_2V8,
	FOCUSC1_DOUT_2V85,
	FOCUSC1_DOUT_2V9,
	FOCUSC1_DOUT_2V95,

};


enum at2600_pm_reg_usbc1{
	USBC1_DOUT_3V3,
	USBC1_DOUT_3V35,	
	USBC1_DOUT_3V4,
	USBC1_DOUT_3V45,
	USBC1_DOUT_2V9,
	USBC1_DOUT_3V0,
	USBC1_DOUT_3V1,
	USBC1_DOUT_3V2,

};



enum at2600_pm_reg_abbc1{
	ABBC1_DOUT_2V5,
	ABBC1_DOUT_2V55,	
	ABBC1_DOUT_2V6,
	ABBC1_DOUT_2V65,
	ABBC1_DOUT_2V3,
	ABBC1_DOUT_2V35,
	ABBC1_DOUT_2V4,
	ABBC1_DOUT_2V45,

};


enum at2600_pm_reg_imgc1{
	IMGC1_DOUT_2V8,
	IMGC1_DOUT_2V85,	
	IMGC1_DOUT_2V9,
	IMGC1_DOUT_2V95,
	IMGC1_DOUT_2V6,
	IMGC1_DOUT_2V65,
	IMGC1_DOUT_2V7,
	IMGC1_DOUT_2V75,

};


enum at2600_pm_reg_r2v8c1{
	R2V8C1_DOUT_2V8,
	R2V8C1_DOUT_2V85,	
	R2V8C1_DOUT_2V9,
	R2V8C1_DOUT_2V95,
	R2V8C1_DOUT_2V6,
	R2V8C1_DOUT_2V65,
	R2V8C1_DOUT_2V7,
	R2V8C1_DOUT_2V75,

};



enum at2600_pm_reg_r3v0c1{
	R3V0C1_DOUT_3V0,
	R3V0C1_DOUT_3V05,	
	R3V0C1_DOUT_3V1,
	R3V0C1_DOUT_3V15,
	R3V0C1_DOUT_2V8,
	R3V0C1_DOUT_2V85,
	R3V0C1_DOUT_2V9,
	R3V0C1_DOUT_2V95,

};


enum at2600_pm_reg_a2v5c1{
	A2V5C1_DOUT_2V5,
	A2V5C1_DOUT_2V55,	
	A2V5C1_DOUT_2V6,
	A2V5C1_DOUT_2V65,
	A2V5C1_DOUT_2V3,
	A2V5C1_DOUT_2V35,
	A2V5C1_DOUT_2V4,
	A2V5C1_DOUT_2V45,

};


enum at2600_pm_reg_led1c1{
	LED1C1_DOUT_VALUE = 0x0,

};

enum at2600_pm_reg_led2c1{
	LED2C1_DOUT_VALUE = 0x0,

};


enum at2600_pm_reg_vibrc1{
	VIBRC1_DOUT_VALUE = 0x0,

};



enum at2600_pm_reg_dcdledc1{
	DCDLED_DOUT_VALUE = 0x0,

};



/* --------------------------------------------------------------------------*/
/* The following is for keypad & LCD backlight control */
#define	GPIOx_PWM1_OUTPUT		0x3
#define	GPIOx_PWM2_OUTPUT		0x4
#define	GPIOx_HIGH_IMPEDANCE		0x7
#define	GPIOx_ZERO			0x0
#define	GPXDIR_IN			0x80		/* 1: input 0:output */

#define	PWMx_SELECT			0x1
#define	PWMx_STEP			0x2

#define PWMx_POLARITY           0x10

#define	DCULED_ON_OPMOD			(0x7 << 5)	/* 111'b3 */
#define	DCULED_OFF_OPMOD		(0x0 << 5)	/* 000'b3 */


/*----------------------------------------------------------------------------*/

/*--------------------power operation-----------------------------------*/
/*dcd led bit 3 for led mode */
enum at2600_pm_dcdled_vol_mode{
     DCDLED_LED_STEP,
	 DCDLED_LED_DIRECT,

};

/*dcd led bit 0-2 for sink led step*/
enum at2600_pm_dcdled_vol_step{
     DCDLED_LED_STEP_30_5US,
	 DCDLED_LED_STEP_61_0US,
	 DCDLED_LED_STEP_91_5US,
	 DCDLED_LED_STEP_122_0US,
	 DCDLED_LED_STEP_152_5US,
	 DCDLED_LED_STEP_183_0US,
	 DCDLED_LED_STEP_213_5US,
	 DCDLED_LED_STEP_244_0US,
};

/* The following is for keypad & LCD backlight control */
enum at2600_pm_dcdled_power_mask{
     DCDLED_POWER_ON,
     DCDLED_POWER_DOWN,

};



#endif /* ___AT2600_PM_GENERIC_H_______ */

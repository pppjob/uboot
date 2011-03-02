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
#include <common.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/topctl.h>
#include <asm/arch-atxx/regs_base.h>

/* ****************************************************************** */
/* pmu power supply */
typedef enum {
	PPS_WIFI = 0,
	PPS_AUDIO,
	PPS_LCD,
	PPS_TP,
	PPS_VGA,
	PPS_TUNER,
	PPS_BT,
	PPS_GPS,
	PPS_USB,
	PPS_VBUS,
	PPS_SD,
	PPS_CAMERA,
	PPS_CMMB,
	PPS_ETH,
	PPS_SDIO,
	PPS_LSENSOR,

	PPS_COMMON = 255,
} power_supply_component;

/* pmu power supply mode */
typedef enum {
	PS_OFF = 0,
	PS_ON,
	PS_LOW,
} power_supply_mode;

int pmu_init(void);

/* pmu_power_control -- control device power
 * @power_supply: specify device module
 * @mode: power mode, such as on, off ...
 * @return: 0(successfull), or -1 error
 */
int pmu_power_control(power_supply_component module, power_supply_mode mode);

void pmu_power_show(void);

void set_backlight(u8 dimfreq, u8 ledman);

uint32_t adc_get_pmu(void);

void battery_check(void);


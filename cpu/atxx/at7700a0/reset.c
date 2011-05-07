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
#include <asm/arch-atxx/regs_base.h>
#include <asm/arch-atxx/pm.h>

void reset_cpu (ulong addr)
{
	uint8_t swcfg;

	swcfg = pm_read_reg(SWCFGR);
	swcfg |= SWCFGR_REBOOT_NORMAL;
	pm_write_reg(SWCFGR, swcfg);

	/* trigger software reset */
	pm_write_reg(SWRSTR, SWRSTR_SOFT_RESET);
}


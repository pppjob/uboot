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
#ifndef __ASM_ARCH_TOPCTL_H
#define __ASM_ARCH_TOPCTL_H

#include <config.h>
#include <asm/bitops.h>

#ifdef CONFIG_AT6600
#include "at6600_topctl.h"
#endif

#ifdef CONFIG_AT7700A0
#include "at7700a0_topctl.h"
#endif

#ifdef CONFIG_AT7700B0
#include "at7700b0_topctl.h"
#endif

static inline void topctl_write_reg(uint32_t reg, uint32_t val)
{
	writel(val, ATXX_TOPCTL_BASE + reg);
}

static inline uint32_t topctl_read_reg(uint32_t reg)
{
	return readl(ATXX_TOPCTL_BASE + reg);
}

static inline void topctl_set_bits(uint32_t reg, uint32_t bits)
{
	set_bit(bits, (volatile void *)(ATXX_TOPCTL_BASE + reg));
}

static inline void topctl_clear_bits(uint32_t reg, uint32_t bits)
{
	clear_bit(bits, (volatile void *)(ATXX_TOPCTL_BASE + reg));
}

#endif /*__ASM_ARCH_TOPCTL_H*/

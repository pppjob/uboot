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
 -----------------------------------------------------------------------------*/

#ifndef __MAP_TABLE_H
#define __MAP_TABLE_H

#include <asm/sizes.h>
#include <asm/arch-atxx/memory_map.h>


/* 
* MDDR physical base address and total size, set mddr size to
* max for  memory map
*/
#define CONFIG_ATXX_MDDR_SIZE	256

#define ATXX_MDDR_PHY_BASE		0x88000000
#define ATXX_MDDR_SIZE			(CONFIG_ATXX_MDDR_SIZE << 20)
#define ATXX_MDDR_MAP_BASE		ATXX_MDDR_PHY_BASE
#define ATXX_MDDR_MAP_SIZE		ATXX_MDDR_SIZE

#define ATXX_ITCM_PHY_BASE		0x0
#define ATXX_ITCM_SIZE			0x2000

#define ATXX_DTCM_PHY_BASE		0x10000
#define ATXX_DTCM_SIZE			0x2000
#define ATXX_TCM_MAP_BASE		0x0
#define ATXX_TCM_MAP_SIZE		SZ_1M

#define ATXX_ARM_ARC_BASE          	0X3FD70000
#define	ATXX_ARM_ARC_SIZE		(SZ_2M + SZ_512K + SZ_64K)
#define ATXX_ARM_ARC_MAP_BASE          	0X3FC00000
#define	ATXX_ARM_ARC_MAP_SIZE			SZ_4M

#define ATXX_DSP_TDM_BASE 			0X40000000
#define ATXX_DSP_TDM_SIZE				SZ_2M

#define ATXX_DSP_L20_BASE               0X40200000
#define ATXX_DSP_L20_SIZE               SZ_1M

#define ATXX_DSP_L21_BASE               0X40400000
#define ATXX_DSP_L21_SIZE               SZ_1M

/*
* Ref: ARMv6 page table format, 6-47
* Notice: there just have one level map table, the size of it 
* is 1M each entry, so make sure input is 1M alignment. 
*/

struct map_desc memory_map[] = {
	{
		.phys = ATXX_MDDR_MAP_BASE,
		.length = ATXX_MDDR_MAP_SIZE,
		.type = CACHEABLE,
		},
	{
		.phys = ATXX_TCM_MAP_BASE,
		.length = ATXX_TCM_MAP_SIZE,
		.type = UNCACHEABLE,
			},
	{
		.phys = ATXX_ARM_ARC_MAP_BASE,
		.length = ATXX_ARM_ARC_MAP_SIZE,
		.type = UNCACHEABLE,
		},
	{
		.phys = ATXX_DSP_TDM_BASE,
		.length = ATXX_DSP_TDM_SIZE,
		.type = CACHEABLE,
		},
	{
		.phys = ATXX_DSP_L20_BASE,
		.length = ATXX_DSP_L20_SIZE,
		.type = CACHEABLE,
		},
	{
		.phys = ATXX_DSP_L21_BASE,
		.length = ATXX_DSP_L21_SIZE,
		.type = CACHEABLE,
		},
	{	0, 0, 0 },
};

#endif

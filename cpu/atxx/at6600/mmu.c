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
#include <linux/types.h>
#include <asm/string.h>
#include <asm/sizes.h>
#include <asm/arch-atxx/cache.h>
#include <asm/arch-atxx/memory_map.h>

#define PMD_TYPE_SECT			(2 << 0)
#define PMD_SECT_AP_WRITE		(3 << 10)
#define PMD_SECT_BUFFERABLE     (1 << 2)
#define PMD_SECT_CACHEABLE      (1 << 3)

#define MMU_CACHE		PMD_TYPE_SECT | PMD_SECT_AP_WRITE	\
						| PMD_SECT_BUFFERABLE | PMD_SECT_CACHEABLE
						
#define MMU_NOCACHE		PMD_TYPE_SECT | PMD_SECT_AP_WRITE

uint32_t mmu_table[4096]  __attribute__((__aligned__(0x4000)));

/*
* Notice: there just have one level map table, the size of it 
* is 1M each entry, so make sure input is 1M alignment. 
*/
static int setup_mmu_table(struct map_desc *memory_map)
{
	int i = 0, j, index,msize;

	memset(mmu_table, 0, sizeof(mmu_table));
	
	while(memory_map[i].length != 0){
		/* error happen */
		if (memory_map[i].length % SZ_1M)
			return -1;
		/* find corresponding entry */
		index = memory_map[i].phys >> 20;
		msize = memory_map[i].length >> 20;
	
		for (j = 0; j < msize; j++) {
			switch(memory_map[i].type){
				case CACHEABLE:
					mmu_table[index] = (index << 20) | MMU_CACHE;
					break;
				case UNCACHEABLE:
					mmu_table[index] = (index << 20) | MMU_NOCACHE;
					break;
				default:
					return -1;
				}
			index ++;
		}
		i++;
	}
	return 0;
}

extern void __mmu_cache_on(uint32_t * mmu_table);
void mmu_cache_on(struct map_desc *memory_map)
{
	int ret;

	ret = setup_mmu_table(memory_map);
	if (ret == 0)
		__mmu_cache_on(mmu_table);
	return;
}

extern void __mmu_cache_off(void);
void mmu_cache_off(void)
{
	__mmu_cache_off();
}

extern void __flush_cache(void);
void arm1176_cache_flush(void)
{
	__flush_cache();
}

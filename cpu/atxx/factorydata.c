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
* Filename:  cpu/atxx/factorydata.c
* Abstract:  factory data interface
*
------------------------------------------------------------------------------*/
#include <common.h>
#include <environment.h>
#include <malloc.h>
#include <nand.h>
#include <watchdog.h>
#include <asm/errno.h>
#include <asm/arch-atxx/factorydata.h>

/*---------------------------------------------------------------------------------------
 * Definitions
 *-------------------------------------------------------------------------------------*/

/* magic string for factory data */
#define	MAGIC_STRING		"ATXXFACTORYDATA"
#define	MAGIC_LENGTH		16

/* store factory data in  + 16MB offset */
#define	FD_MTD_OFFSET		(16 << 20)
/* we allow the maximal bad size as 8MB */
#define	FD_MTD_BADB_SIZE	(8 << 20)

/* we allow the minimal page size as 4096 */
#define	FD_MTD_MINP_SIZE	(4096)

typedef struct factory_data_page {
	char		magic[MAGIC_LENGTH];
	uint32_t	flag;
	uint32_t	padding;
	factory_data_t	fd_user;
} factory_data_page_t;

#if 0
#define fd_debug_print(fmt, arg...)	do { \
					printf(fmt, ## arg); \
				} while(0)
#else
#define fd_debug_print(fmt, arg...)
#endif
#define fd_err_print(fmt, arg...)	printf(fmt, ## arg)

/*---------------------------------------------------------------------------------------
 * Internal Utilities
 *-------------------------------------------------------------------------------------*/

/* get block size */
#define FD_BLOCK_SIZE(nand)	(nand->erasesize)
/* get page size */
#define FD_PAGE_SIZE(nand)		max(nand->writesize, FD_MTD_MINP_SIZE)

/* find target offset include bad block */
static size_t fd_get_off_incl_bad(nand_info_t *nand, int fd_index)
{
	size_t offset = FD_MTD_OFFSET;
	size_t block_size = FD_BLOCK_SIZE(nand);
	size_t page_size = FD_PAGE_SIZE(nand);
	int page_count_per_block = block_size / page_size;

	do {
		if (nand_block_isbad (nand, offset)) {
			fd_debug_print("skipping bad block 0x%x\n", (uint)offset);
		} else {
			if (fd_index <= page_count_per_block)
				return offset + fd_index * page_size;
			else
				fd_index -= page_count_per_block;
		}
		offset += block_size;
	} while (offset < FD_MTD_OFFSET + FD_MTD_BADB_SIZE);
	return offset;
}

/*---------------------------------------------------------------------------------------
 * Factory Interfaces
 *-------------------------------------------------------------------------------------*/

/* interfaces to get factory data */
factory_data_t *factory_data_get(int fd_index)
{
	factory_data_page_t *page;
	nand_info_t *nand = &nand_info[nand_curr_device];
	size_t page_size, offset, op_size;
	int reval;

	if ((unsigned)fd_index >= FD_MAX) {
		fd_err_print("fd_index is bad\n");
		return NULL;
	}

	/* seek the non-bad block */
	offset = fd_get_off_incl_bad(nand, fd_index);
	if (offset >= FD_MTD_OFFSET + FD_MTD_BADB_SIZE) {
		fd_err_print("too much bad blocks\n");
		return NULL;
	}

	page_size = FD_PAGE_SIZE(nand);

	/* allocate factory data page */
	page = (factory_data_page_t *)malloc(page_size);
	if (page == NULL) {
		fd_err_print("failed to allocate memory!\n");
		return NULL;
	}

	/* read the nand */
	op_size = page_size;
	fd_debug_print("read nand offset 0x%x ", (uint)offset);
	fd_debug_print("size 0x%x\n", (uint)op_size);
	if ((reval = nand_read(nand, offset, &op_size, (u_char *)page)) < 0) {
		fd_err_print("failed to read nand offset: 0x%x (%d)", (uint)offset, reval);
		free(page);
		return NULL;
	}

	/* check magic string */
	if (strncmp(page->magic, MAGIC_STRING, MAGIC_LENGTH) != 0) {
		/* un-initialized page */
		memset(page, -1, page_size);
	}

	return &(page->fd_user);
}


/* interfaces to release factory data */
void factory_data_put(factory_data_t *fd)
{
	factory_data_page_t *page;
	page = container_of(fd, factory_data_page_t, fd_user);
	free(page);
}

/* this function is for calibration tool to store the data */
int factory_data_store(factory_data_t *fd)
{
	nand_info_t *nand = &nand_info[nand_curr_device];
	nand_erase_options_t erase_options;
	factory_data_page_t *page, *cur_page, **pages;
	size_t block_size, page_size, offset, op_size;
	size_t block_offset, cur_offset;
	int i, index_begin, index_count;
	int reval;

	if (fd == NULL) {
		fd_err_print("fd is bad\n");
		return -EINVAL;
	}
	if ((unsigned)fd->fd_index >= FD_MAX) {
		fd_err_print("fd_index is bad\n");
		return -EINVAL;
	}

	/* seek the non-bad block */
	offset = fd_get_off_incl_bad(nand, fd->fd_index);
	if (offset >= FD_MTD_OFFSET + FD_MTD_BADB_SIZE) {
		fd_err_print("too much bad blocks\n");
		return -ENOSPC;
	}

	block_size = FD_BLOCK_SIZE(nand);
	page_size = FD_PAGE_SIZE(nand);
	page = container_of(fd, factory_data_page_t, fd_user);

	/* label the magic */
	strncpy(page->magic, MAGIC_STRING, MAGIC_LENGTH);

	/* calculate page count */
	block_offset = offset & ~(block_size - 1);
	index_begin = fd->fd_index - (offset - block_offset) / page_size;
	index_count = min(block_size / page_size, FD_MAX - index_begin);

	/* allocate page array */
	pages = (factory_data_page_t **)malloc(index_count * sizeof(factory_data_page_t *));
	if (pages == NULL) {
		fd_err_print("failed to allocate memory!\n");
		return -ENOMEM;
		}
	memset(pages, 0, index_count * sizeof(factory_data_page_t *));

	cur_page = NULL;
	cur_offset = block_offset;
	for (i = 0; i < index_count; i++) {
		/* skip page to be erased */
		if (i == fd->fd_index - index_begin) {
			pages[i] = page;
			cur_offset += page_size;
			continue;
	}

		/* allocate page space */
		if (cur_page == NULL) {
			cur_page = (factory_data_page_t *)malloc(page_size);
			if (cur_page == NULL) {
				fd_err_print("failed to allocate memory!\n");
				reval = -ENOMEM;
				goto EXIT;
			}
		}

		/* read the nand */
		op_size = page_size;
		fd_debug_print("read nand offset 0x%x ", (uint)cur_offset);
		fd_debug_print("size 0x%x\n", (uint)op_size);
		if ((reval = nand_read(nand, cur_offset, &op_size, (u_char *)cur_page)) < 0) {
			fd_err_print("failed to read NAND offset: 0x%x, size: %d(%d)\n",
					(uint)cur_offset, op_size, reval);
		} else {
			if (strncmp(cur_page->magic, MAGIC_STRING, MAGIC_LENGTH) == 0) {
				pages[i] = cur_page;
				cur_page = NULL;
			}
		}

		cur_offset += page_size;
	}

	/* clear block */
	erase_options.offset = block_offset;
	erase_options.length = block_size;
	erase_options.quiet = 0;
	erase_options.jffs2 = 0;
	erase_options.scrub = 0;
	fd_debug_print("erase nand offset 0x%x ", (uint)block_offset);
	fd_debug_print("size 0x%x\n", (uint)block_size);
	if ((reval = nand_erase_opts(nand, &erase_options)) < 0) {
		/* erase failed, will be marked bad */
		fd_err_print("failed to erase nand offset 0x%x(%d)", (uint)block_offset, reval);
		goto EXIT;
	}

	/* write back useful page */
	cur_offset = block_offset;
	for (i = 0; i < index_count; i++) {
		if (pages[i] != NULL) {
	op_size = page_size;
			fd_debug_print("write nand offset 0x%x ", (uint)cur_offset);
	fd_debug_print("size 0x%x\n", (uint)op_size);
			if ((reval = nand_write(nand, cur_offset, &op_size, (u_char *)pages[i])) < 0) {
				fd_err_print("failed to write to nand offset 0x%x(%d)", (uint)offset, reval);
				goto EXIT;
			}
		}
		cur_offset += page_size;
	}

EXIT:
	/* free all buffers */
	if (cur_page != NULL)
		free(cur_page);
	if (pages != NULL) {
		for (i = 0; i < index_count; i++) {
			if (i != fd->fd_index && pages[i] != NULL)
				free(pages[i]);
		}
		free(pages);
	}

	return 0;
}

/* clear the whole factory data area for initial liveboot */
int factory_data_clearall(void)
{
	nand_info_t *nand = &nand_info[nand_curr_device];
	nand_erase_options_t erase_options;
	size_t block_size, offset, last_offset;
	int fd_index, page_count_per_block;

	block_size = FD_BLOCK_SIZE(nand);
	page_count_per_block = block_size / FD_PAGE_SIZE(nand);
	fd_index = 0;
	last_offset = -1;

	do {
		offset = fd_get_off_incl_bad(nand, fd_index);
		if (offset >= FD_MTD_OFFSET + FD_MTD_BADB_SIZE) {
			fd_err_print("too much bad blocks\n");
			return -ENOSPC;
		}

		/* dead loop check */
		if (last_offset == offset) {
			fd_err_print("unexpected erase failed!\n");
			return -EIO;
		}
		last_offset = offset;

		erase_options.offset = offset;
		erase_options.length = block_size;
		erase_options.quiet = 0;
		erase_options.jffs2 = 0;
		erase_options.scrub = 0;
		fd_debug_print("erase nand offset 0x%x ", (uint)offset);
		fd_debug_print("size 0x%x\n", (uint)block_size);
		if (nand_erase_opts(nand, &erase_options) < 0) {
			/* erase failed, will be marked bad */
			fd_err_print("failed to erase NAND offset 0x%x\n", (uint)offset);
		}
		else {
			fd_index += page_count_per_block;
		}
	} while (fd_index < FD_MAX);

	return 0;
}

/* this function is to prevent from out-of-range of data buf */
int factory_data_get_max_buf_len(void)
{
	nand_info_t *nand = &nand_info[nand_curr_device];
	return FD_PAGE_SIZE(nand) - sizeof(factory_data_page_t);
}


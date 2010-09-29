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
#include <command.h>
#include <asm/arch-atxx/factorydata.h>

static int do_factory(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int index;
	int ret;

	/* at least one arguments please */
	if (argc < 2)
		goto PRINT_USAGE;

	if (strcmp(argv[1], "clearall") == 0) {
		if (argc > 2)
			goto PRINT_USAGE;
		ret = factory_data_clearall();
		if ( ret == 0)
			printf("factory_data_clearall success!\n");
		else
			printf("factory_data_clearall failed (%d)\n", (uint)ret);
		return 0;
	}

	if (argc < 3)
		goto PRINT_USAGE;

	index = simple_strtol(argv[2], NULL, 10);

	if (strcmp(argv[1], "read") == 0) {
		factory_data_t *data;
		if (argc != 3)
			goto PRINT_USAGE;

		data = factory_data_get(index);
		if (data == NULL) {
			printf("read factory data %d failed!\n", index);
		} else if (data->fd_index != index){
			printf("factory data %d unavailable!\n", index);
		} else {
			int i;
			printf("read factory data %d success!\n", index);
			printf("index: %d\n", data->fd_index);
			printf("length: %d\n", data->fd_length);
			printf("buf:\n");
			for (i = 0; i < data->fd_length; i++) {
				printf("%02x ", data->fd_buf[i]);
				if (((i + 1) % 16) == 0)
					printf("\n");
			}
			printf("\n");
		}
	}
	else if (strcmp(argv[1], "write") == 0) {
		factory_data_t *data;
		if (argc != 4)
			goto PRINT_USAGE;
		data = factory_data_get(index);
		if (data == NULL) {
			printf("read factory data %d failed!\n", index);
			return 1;
		}
		data->fd_index = index;
		data->fd_length = strlen(argv[3]);
		strcpy(data->fd_buf, argv[3]);
		ret = factory_data_store(data);
		if (ret != 0) {
			printf("write factory data %d failed!\n", index);
			printf("ret: %d\n", ret);
		}
		else {
			printf("write factory data %d success!\n", index);
		}
	}

	if (data != NULL)
		factory_data_put(data);
	return 0;

PRINT_USAGE: /* print usage */
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(fd, 4, 1, do_factory,
	"fd clear/read/write",
	"	fd clearall - clear all factory data page\n"
	"	fd read index - download and print target factory page data\n"
	"	fd write index <data> - update target factory page by input data\n"
);

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

#ifndef __ASM_ARCH_FACTORYDATA_H
#define __ASM_ARCH_FACTORYDATA_H

#define	FD_UNKNOWN	-1

enum factory_data_index {
	FD_MDDR = 0,
	FD_BATTERY,
	FD_RTP,
	FD_LSENSOR,
	FD_BLUETOOTH,
	FD_WIFI,
	FD_GSM,
	FD_MENU_CONSOLE,
	FD_SN,
	FD_IMEI,
	FD_MASK,
	FD_ETHERNET,
	FD_CAMERA,
	FD_GSM_0,
	FD_GSM_1,
	FD_GSM_2,
	FD_GSM_3,
	FD_GSM_4,
	FD_GSM_5,
	FD_GSM_6,
	FD_GSM_7,
	FD_GSM_8,
	FD_GSM_9,
	FD_GSM_10,
	FD_GSM_11,
	FD_GSM_12,
	FD_GSM_13,
	FD_GSM_14,
	FD_GSM_15,
	FD_CONFIG,
	FD_MAX
};

typedef	struct factory_data {
	int	fd_index;	/* factory data index */
	int	fd_length;	/* data length */
	char	fd_buf[0];	/* customized data area */
} factory_data_t;

typedef struct mddr_f_data{
	uint8_t mddr_size;
	uint8_t mddr_cal_data[8];
}mddr_f_data_t;

struct boot_parameter{
        char magic[16];
        int mddr_data_send;
        mddr_f_data_t f_mddr;
};

/* interfaces to get/release factory data */
factory_data_t	*factory_data_get(int fd_index);
void		factory_data_put(factory_data_t *fd);

/* this function is for calibration tool to store the data */
int		factory_data_store(factory_data_t *fd);

/* clear the whole factory data area for initial liveboot */
int		factory_data_clearall(void);

/* this fucttion is to prevent from out-of-range of data buf */
int		factory_data_get_max_buf_len(void);

#endif

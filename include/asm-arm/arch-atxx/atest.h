#ifndef __ATESTPARAM_H
#define __ATESTPARAM_H

typedef enum {
	READ_ID = 0,
	READ_WRITE,
} i2c_test_type;

typedef struct atest_i2c_setting {
	uchar		name[16];
	u8			chip_addr;
	i2c_test_type			test_type;
	u32			id_addr;
	u8			id_addr_len;
	u32			id_data;
	u8			id_data_len;
} atest_i2c_setting_t;


#if	defined(CONFIG_BOARD_CAYMAN20)
#include "atest_cayman20.h"
#elif	defined(CONFIG_BOARD_MIRACLE)
#include "atest_miracle.h"
#elif	defined(CONFIG_BOARD_G20)
#include "atest_g20.h"
#elif	defined(CONFIG_BOARD_T3C)
#include "atest_t3c.h"
#elif	defined(CONFIG_BOARD_W002)
#include "atest_w002.h"
#elif	defined(CONFIG_BOARD_AD8071)
#include "atest_ad8071.h"
#elif	defined(CONFIG_BOARD_ATB9004)
#include "atest_atb9004.h"
#elif	defined(CONFIG_BOARD_FPGA7700B0)
#include "atest_fpga7700b0.h"
#elif	defined(CONFIG_BOARD_ATB1004)
#include "atest_atb1004.h"
#elif   defined(CONFIG_BOARD_ATB1005)
#include "atest_atb1005.h"
#elif   defined(CONFIG_BOARD_G01)
#include "atest_g01.h"
#elif   defined(CONFIG_BOARD_GAMEBOX)
#include "atest_gbox.h"
#elif   defined(CONFIG_BOARD_M70P)
#include "atest_m70p.h"
#else
#error   "atest_<board>.h is missing!"
#endif

#define	ATEST_I2C_COUNT		ARRAY_SIZE(atest_i2c_item)


#endif /* __ATESTPARAM_H */

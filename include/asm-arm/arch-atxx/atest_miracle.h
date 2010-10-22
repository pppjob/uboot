#ifndef __TEST_MIRACLE_H
#define __TEST_MIRACLE_H


static atest_i2c_setting_t atest_i2c_item[] = {
	{"pcf50626",   0x70,   READ_ID,     0,       1,       0x31,     1},
#if 0
	{"ov3640",     0x3c,   READ_ID,     0x300a,  2,       0x364c,   2},
	{"mxc6202x",   0x15,   READ_WRITE,  0,       1,       0x31,     1},
	{"bg0328",     0x52,   READ_WRITE,  0,       1,       0x31,     1},
#endif
};

#endif /* __TEST_MIRACLE_H */

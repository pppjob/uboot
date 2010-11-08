#ifndef __TEST_T3C_H
#define __TEST_T3C_H

static atest_i2c_setting_t atest_i2c_item[] = {
	{"pcf50626",   0x70,   READ_ID,     0,       1,       0x35,     1},
#if 0
	{"ov2655",     0x30,   READ_ID,     0x300a,  2,       0x2656,   2},
	{"mxc6202x",   0x15,   READ_WRITE,  0,       1,       0xA,      1},
	{"tpeeti",     0x0a,   READ_WRITE,  0,       1,       0x31,     1},
#endif
};


#endif /* __TEST_T3C_H */

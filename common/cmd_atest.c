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
#include <command.h>
#include <common.h>
#include <serial.h>
#include <nand.h>
#include <linux/types.h>
#include <asm/sizes.h>
#include <asm/string.h>
#include <asm/arch-atxx/clock.h>
#include <asm/arch-atxx/topctl.h>
#include <asm/arch-atxx/gpio.h>
#include <asm/arch-atxx/adc.h>
#include <asm/arch-atxx/delay.h>
#include <asm/arch-atxx/cache.h>
#include <asm/arch-atxx/memory_map.h>
#include <asm/arch-atxx/factorydata.h>
#include <asm/arch-atxx/hdcvt.h>
#include <asm/arch-atxx/atest.h>
#include <i2c.h>

#define DOWNLOAD_ADDR   (MDDR_BASE_ADDR + 16 * SZ_1M)

#ifdef	CONFIG_GSM_TEST

typedef union
{
	uint8_t uint8_data[4];
	uint32_t uint32_data;
} NEW_BAUDRATE;

static void gsm_delay(uint32_t ms)
{
#if 0
	mdelay(ms/2);
#else
	uint32_t tick1, tick2;

	tick1 = get_timer(0);
	do {
		tick2 = get_timer(0);
	} while ((tick2 - tick1) < ms);
#endif
}

static void gsm_bridge(int argc, char *argv[])
{
	struct clk *clk_pll;
	struct clk *clk_uart;
	char *flow_control;
	uint32_t flow_control_en = 0, val;
	uint32_t clk_freq, uart_baudrate;
	uart_t uart0, uart1;
	uint8_t ch;

	printf("GSM bridge!\n");
	if (argc == 4) {
		flow_control = argv[3];
		if (!strcmp(flow_control, "1")) {
			flow_control_en = 1;
		} else {
			flow_control_en = 0;
		}
	}

	mdelay(100);
	val = topctl_read_reg(TOPCTL1);
	val &= ~(1 << 10);
	val |= (1 << 13);
	topctl_write_reg(TOPCTL1, val);

	clk_pll = clk_get("pll3");
	clk_uart = clk_get("uart");

	clk_set_rate(clk_pll, 312 * MHZ);
	clk_set_parent(clk_uart, clk_pll);
	clk_set_rate(clk_uart, 15 * MHZ);

	uart0.clkfreq = 14745600;
	uart0.baudrate = 115200 * 8;
	uart0.fifo_cfg = 2;
	uart0.loop_enable = 0;
	uart0.flow_control = flow_control_en;
	serial_assign("serial0");
	serial_init_adv(&uart0);

	mdelay(100);

	clk_freq = 14745600;
	uart_baudrate = 115200;

	mdelay(100);
	uart1.clkfreq = clk_freq;
	uart1.baudrate = uart_baudrate;
	uart1.fifo_cfg = 2;
	uart1.loop_enable = 0;
	uart1.flow_control = 1; //flow_control_en;
	printf("assign uart1!\n");
	serial_assign("serial1");
	serial_init_adv(&uart1);
	serial_assign("serial0");

	mdelay(100);
	printf("set gpio!\n");
	if (atxx_request_gpio(GPIO_GSM_DTR) 
			|| atxx_request_gpio(GPIO_GSM_ON)
			|| atxx_request_gpio(GPIO_GSM_ACKON)
			|| atxx_request_gpio(GPIO_GSM_RESETn)) {
		printf("Wrong gpio request!\n");
		return;
	}

	atxx_set_gpio_direction(GPIO_GSM_ACKON, 1);
	atxx_set_gpio_direction(GPIO_GSM_DTR, 0);
	atxx_set_gpio_direction(GPIO_GSM_ON, 0);

	atxx_gpio_set(GPIO_GSM_ON,0);
	atxx_gpio_set(GPIO_GSM_DTR,0);
	mdelay(10);
	atxx_gpio_set(GPIO_GSM_ON,1);
	mdelay(1000);
	atxx_gpio_set(GPIO_GSM_ON,0);

	uart0.clkfreq = clk_freq;
	uart0.baudrate = uart_baudrate;
	uart0.fifo_cfg = 2;
	uart0.loop_enable = 0;
	uart0.flow_control = flow_control_en;
	serial_assign("serial0");
	serial_init_adv(&uart0);

	atxx_gpio_set(GPIO_GSM_DTR,1);

	while(1) {
		if (serial_get_bridge(&ch) == 1) {
			serial_assign("serial1");
			serial_putc(ch);
		} else {
			serial_assign("serial0");
			serial_putc(ch);
		}
	}

}

static void gsm_download(void)
{
	struct clk *clk_pll;
	struct clk *clk_uart;
	uart_t uart0, uart1;
	uint32_t clk_freq, uart_baudrate;
	uint32_t timer_counts;
	uint8_t last_data=0,count_start=0,change_signal=0,change_count=0;
	uint8_t ch;
	uint8_t head_count,head_string[3];
	NEW_BAUDRATE new_baudrate;

	printf("GSM download!\n");

	clk_pll = clk_get("pll3");
	clk_uart = clk_get("uart");

	clk_set_rate(clk_pll, 624 * MHZ);
	clk_set_parent(clk_uart, clk_pll);
	clk_set_rate(clk_uart, 30 * MHZ);

	gsm_delay(100);

	clk_freq = (14745600 * 2);
	uart_baudrate = 115200;

	uart1.clkfreq = clk_freq;
	uart1.baudrate = uart_baudrate;
	uart1.fifo_cfg = 2;
	uart1.loop_enable = 0;
	uart1.flow_control = 0;
	serial_assign("serial1");
	serial_init_adv(&uart1);

	uart0.clkfreq = clk_freq;
	uart0.baudrate = uart_baudrate;
	uart0.fifo_cfg = 2;
	uart0.loop_enable = 0;
	uart0.flow_control = 0;
	serial_assign("serial0");
	serial_init_adv(&uart0);

	gsm_delay(10000);

	if (atxx_request_gpio(GPIO_GSM_DTR) 
			|| atxx_request_gpio(GPIO_GSM_ON)
			|| atxx_request_gpio(GPIO_GSM_ACKON)
			|| atxx_request_gpio(GPIO_GSM_RESETn)) {
		printf("Wrong gpio request!\n");
		return;
	}

	atxx_set_gpio_direction(GPIO_GSM_ACKON, 1);
	atxx_set_gpio_direction(GPIO_GSM_DTR, 0);
	atxx_set_gpio_direction(GPIO_GSM_RESETn, 0);
	atxx_set_gpio_direction(GPIO_GSM_ON, 0);

	atxx_gpio_set(GPIO_GSM_ON, 0);
	atxx_gpio_set(GPIO_GSM_DTR, 0);
	atxx_gpio_set(GPIO_GSM_RESETn, 1);
	gsm_delay(10);

	atxx_gpio_set(GPIO_GSM_ON, 1);
	atxx_gpio_set(GPIO_GSM_DTR, 1);
	atxx_gpio_set(GPIO_GSM_RESETn, 0);
	gsm_delay(1000);

	timer_counts = get_timer(0);
	while(1) {
		if (get_timer(timer_counts) >= 100)
			atxx_gpio_set(GPIO_GSM_RESETn, 1);

		if (change_signal == 1) {
			if (change_count >= 8 && change_count <= 11) {
				new_baudrate.uint8_data[change_count-8] = ch;
			}
			if (change_count >= 29) {  //29=15+6+8 
				gsm_delay(1);

				uart1.clkfreq = clk_freq;
				uart1.baudrate = new_baudrate.uint32_data;
				uart1.fifo_cfg = 2;
				uart1.loop_enable = 0;
				uart1.flow_control = 0;
				serial_assign("serial1");
				serial_init_adv(&uart1);

				uart0.clkfreq = clk_freq;
				uart0.baudrate = new_baudrate.uint32_data;
				uart0.fifo_cfg = 2;
				uart0.loop_enable = 0;
				uart0.flow_control = 0;
				serial_assign("serial0");
				serial_init_adv(&uart0);

				change_count = 0;
				change_signal = 2;
				while(1) {
					if (serial_get_bridge(&ch) == 1) {
						serial_assign("serial1");
						serial_putc(ch);
					} else {
						serial_assign("serial0");
						serial_putc(ch);
					}
				}
			}
			change_count++;
		}

		if (serial_get_bridge(&ch) == 1) {
			if (last_data == 2) {
				if (change_signal == 0) {
					count_start = 1;
					head_count = 0;
				}
			}
			if ((count_start == 1) && (change_signal != 2)) {
				head_string[head_count++] = ch;
				if (head_count == 3) {
					if ((head_string[0] == 0x02) && 
							(head_string[1] == 0x00) && 
							(head_string[2] == 0x82)) {
						change_signal = 1;
					}
					count_start = 0;
				}
			}
			serial_assign("serial1");
			serial_putc(ch);
			serial_assign("serial0");
			last_data = 1;
		} else {
			serial_assign("serial0");
			serial_putc(ch);
			last_data = 2;
		}
	}
}

#endif

/*----------------------------------------------------------------------------*/
static int string_data_test(int argc, char *argv[], int index, const char* name)
{
	factory_data_t *data = NULL;
	char *action;
	int rw;
	int ret = -1;

	if (argc < 3){
		return ret;
	}

	action = argv[2];
	if (!strcmp(action, "read")) {
		rw = 0;
	} else if (!strcmp(action, "write")) {
		rw = 1;
		if (argc < 4) {
			return ret;
		}
	} else {
		return ret;
	}

	data = factory_data_get(index);
	if (data == NULL) {
		printf("read %s failed!\n", name);
		return ret;
	}

	if (rw) {
		data->fd_index = index;
		data->fd_length = strlen(argv[3]);
		strcpy(data->fd_buf, argv[3]);
		ret = factory_data_store(data);
		if (ret == 0) {
			printf("write %s success!\n", name);
		} else {
			printf("write %s failed!\n", name);
		}
		ret = 0;
	} else {
		int i;
		if (data->fd_index != index){
			printf("%s unavailable!\n", name);
		} else {
			printf("%s: ", name);
			for (i = 0; i < data->fd_length; i++)
				putc(data->fd_buf[i]);
			putc('\n');
		}
		ret = 0;
	}

	if (data != NULL)
		factory_data_put(data);
	return ret;
}

static int sn_test(int argc, char *argv[])
{
	return string_data_test(argc, argv, FD_SN, "sn");
}

static int imei_test(int argc, char *argv[])
{
	return string_data_test(argc, argv, FD_IMEI, "imei");
}

static int wifi_test(int argc, char *argv[])
{
	return string_data_test(argc, argv, FD_WIFI, "wifi");

}

static int bt_test(int argc, char *argv[])
{
	return string_data_test(argc, argv, FD_BLUETOOTH, "bt");

}

#define MAX_STATION_MASK	15
static int mask_test(int argc, char *argv[])
{
	char *action;
	int ret = -1, i, station_num;
	factory_data_t *data;

	if (argc < 2){
		return ret;
	}
	action = argv[2];
	if (!strcmp(action, "read")) {
		data = factory_data_get(FD_MASK);
		if (data == NULL) {
			printf("read factory data failed!\n");
			return 1;
		}
		
		for (i = 0; i < MAX_STATION_MASK; i++) {
			printf("Station: %d------%d!\n", i, (data->fd_buf[i] == 0)?0:1);
		}

		printf("\n");
		ret = 0;
	} else if (!strcmp(action, "write")){
		if (argc != 4)
			return ret;
	
		station_num = simple_strtoul(argv[3], NULL, 10);
	
		data = factory_data_get(FD_MASK);
		if (data == NULL) {
			printf("read factory data failed!\n");
			return 1;
		}

		data->fd_index = FD_MASK;
		data->fd_length = MAX_STATION_MASK;
		data->fd_buf[station_num] = 0;

		ret = factory_data_store(data);
		if (ret != 0) {
			printf("write MASK failed!\n");
		} else {
			printf("write MASK success!\n");
		}
		
		printf("\n");
		ret = 0;
	}
	return ret;
}

static int battery_test(int argc, char *argv[])
{
	char *action;
	int ret = -1;
	int adc_value, voltage, calibration_k, calibration_b;
	factory_data_t *data;
	uint32_t *p_buf;

	if (argc < 2){
		return ret;
	}
	action = argv[2];
	if (!strcmp(action, "read")) {
		adc_value = adc_get_aux(TSC_ADC_AUX1);
		printf("adc value: %d\n", adc_value);
		ret = 0;
	} else if (!strcmp(action, "write")){
		if (argc != 5)
			return ret;
	
		calibration_k = simple_strtoul(argv[3], NULL, 16);
		calibration_b = simple_strtoul(argv[4], NULL, 16);
		data = factory_data_get(FD_BATTERY);
		if (data == NULL) {
			printf("read factory data failed!\n");
			return 1;
		}

		data->fd_index = FD_BATTERY;
		data->fd_length = 8;
		p_buf = (uint32_t *)&data->fd_buf[0];
		*p_buf = calibration_k;
		p_buf = (uint32_t *)&data->fd_buf[4];
		*p_buf = calibration_b;

		printf("k: %d, b: %d\n", calibration_k, calibration_b);

		ret = factory_data_store(data);
		if (ret != 0) {
			printf("write battery data failed!\n");
		} else {
			printf("write battery data success!\n");
		}
	
		printf("\n");
		ret = 0;
	} else if (!strcmp(action, "check")){
		adc_value = adc_get_aux(TSC_ADC_AUX1);
		data = factory_data_get(FD_BATTERY);
		if (data == NULL) {
			printf("read factory data failed!\n");
		} else if (data->fd_index != FD_BATTERY){
			printf("battery factory data unavailable!\n");
		} else {
			calibration_k = *(uint32_t *)&data->fd_buf[0]; 
			calibration_b = *(uint32_t *)&data->fd_buf[4];
			voltage = (calibration_k * adc_value + calibration_b) / 1000;
			printf("voltage: %d\n", voltage);
		}
		ret = 0;
	}
	return ret;
}

static int auto_test(int argc, char *argv[])
{
	int ret = -1;
	int sd_result = 0, nand_result = 0;
	char cmd[CONFIG_SYS_CBSIZE];
	atxx_image_header_t *head = (atxx_image_header_t *)DOWNLOAD_ADDR;
	nand_info_t *nand = &nand_info[nand_curr_device];
	int nand_offset, i;
	u_char *test_buf = (u_char *)DOWNLOAD_ADDR;
	unsigned char i2c_buf[6];

	/* sd test */
	run_command("mmc init", 0);

	sprintf(cmd, "fatload mmc 1 0x%08x xloader.img", DOWNLOAD_ADDR);
	if (run_command(cmd, 0)) {
		ret = 1;
	}

	if (ret != 1) {
		if (head->boot_signature == HEAD_SIGNATURE)
			sd_result = 0; 
		else
			sd_result = 1; 
	} else {
		sd_result = 1; 
	}

	/* nand test */
	ret = 0;
	run_command("nand erase", 0);

	for (i = 0; i < nand->writesize; i++) {
		test_buf[i] = i & 0xff;
	}

	nand_offset = 0x100000;
	if ((nand_write(nand, nand_offset, &nand->writesize, test_buf)) < 0) { 
		ret = 1;
		goto nand_test_result;
	}
	if ((nand_read(nand, nand_offset, &nand->writesize, 
					test_buf + nand->writesize)) < 0) { 
		ret = 1;
		goto nand_test_result;
	}

	for (i = 0; i < nand->writesize; i++) {
		if (test_buf[i + nand->writesize] != (i & 0xff)) {
			ret = 1;
			break;
		}
	}

	run_command("nand erase 0x100000 0x100000", 0);

nand_test_result:
	if (ret)
		nand_result = 1; 
	else
		nand_result = 0; 

	/* print out the test result */
	printf("############ Test Result ##############\n");

	if (nand_result)
		printf("NAND test FAIL!\n");
	else
		printf("NAND test OK!\n");

	if (sd_result)
		printf("SD test FAIL!\n");
	else
		printf("SD test OK!\n");


	/* i2c devices test */

	ret = 0;
	memset(i2c_buf, 0, sizeof(i2c_buf));

	for (i = 0; i < ATEST_I2C_COUNT; i++) {
		if (atest_i2c_item[i].test_type == READ_ID) {
			if ( 0 != i2c_read(atest_i2c_item[i].chip_addr, 
						atest_i2c_item[i].id_addr, 
						atest_i2c_item[i].id_addr_len, 
						i2c_buf, 
						atest_i2c_item[i].id_data_len) ) {
				ret = 1;
			} else {
				if (i2c_buf[0] != atest_i2c_item[i].id_data)
					ret = 1;
			}
		} else {
			//TODO
		}
		if (ret)
			printf("%s test FAIL!\n", atest_i2c_item[i].name);
		else
			printf("%s test OK!\n", atest_i2c_item[i].name);
	}

	return ret;
}

#ifdef	CONFIG_GSM_TEST
static int gsm_test(int argc, char *argv[])
{
	char *action;
	int ret = -1;

	if (argc < 2){
		return ret;
	}
	action = argv[2];
	if (!strcmp(action, "download")) {
		gsm_download();
		ret = 0;
	} else if (!strcmp(action, "bridge")){
		gsm_bridge(argc, argv);
		printf("\n");
		ret = 0;
	}
	return ret;
}
#else
static int gsm_test(int argc, char *argv[])
{
	printf("Not supported!\n");
	return 0;
}
#endif

int do_atest(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = -1;
	char *subcmd;

	if (argc < 1) {
		goto done;
	}
	subcmd = argv[1];

	if (!strcmp(subcmd, "sn")) {
		ret = sn_test(argc, argv);
	} else if (!strcmp(subcmd, "imei")) {
		ret = imei_test(argc, argv);
	} else if (!strcmp(subcmd, "wifi")) {
		ret = wifi_test(argc, argv);
	} else if (!strcmp(subcmd, "bt")) {
		ret = bt_test(argc, argv);
	} else if (!strcmp(subcmd, "mask")) {
		ret = mask_test(argc, argv);
	} else if (!strcmp(subcmd, "battery")) {
		ret = battery_test(argc, argv);
	} else if (!strcmp(subcmd, "autotest")) {
		ret = auto_test(argc, argv);
	} else if (!strcmp(subcmd, "gsm")) {
		ret = gsm_test(argc, argv);
	}

	if (ret == 0)
		return 0;
done:
	if (ret < 0)		
		cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	atest,   8,   0,	do_atest,
	"do factory related test.",
	"usage:\n"
	"atest sn|imei|wifi|bt|mask|battery read|write [data]\n"
	"atest autotest --- do factory autotest\n"
	"atest gsm download|bridge --- connect gsm uart to uart0\n"
);


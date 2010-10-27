#ifndef __BOOTPARAM_H
#define __BOOTPARAM_H

enum boot_mode {
	NAND_BOOT = 0,
	NAND_RECOVERY,
	SD_BOOT,
	SD_INSTALL,
	SD_PLATFORM,
	SD_BOARDTEST,
	SD_PHONETEST,
	SD_RECOVERY,
	CMD_MODE,
};

enum boot_mode hwcfg_detect(void);
enum boot_mode swcfg_detect(void);

int build_boot_cmd(enum boot_mode mode, char *fstype);

#endif /* __BOOTPARAM_H */

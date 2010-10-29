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
#include <asm/string.h>

int clock_env (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong freq;

	if (argc < 3) {
		goto failed;
	}

	if (strcmp (argv[1], "clk-arm") == 0){
		setenv("clk-arm", argv[2]);
		saveenv();
	}else if(strcmp (argv[1], "clk-axi") == 0){
		setenv("clk-axi", argv[2]);
		saveenv();
	}else if(strcmp (argv[1], "clk-mddr") == 0){
		setenv("clk-mddr", argv[2]);
		saveenv();
	}else if(strcmp (argv[1], "clk-app") == 0){
		setenv("clk-app", argv[2]);
		saveenv();
	}else if(strcmp (argv[1], "clk-dspcore") == 0){
		setenv("clk-dspcore", argv[2]);
		saveenv();
	}else if(strcmp (argv[1], "clk-vpclk") == 0){
		setenv("clk-vpclk", argv[2]);
		saveenv();
	}else if(strcmp (argv[1], "clk-gclk") == 0){
		setenv("clk-gclk", argv[2]);
		saveenv();
	}else if(strcmp (argv[1], "clk-vsclk") == 0){
		setenv("clk-vsclk", argv[2]);
		saveenv();
	}else{
		goto failed;
	}

	return 0;

failed:
	cmd_usage(cmdtp);

	return -1;
}


U_BOOT_CMD(
	clockenv, 3, 0, clock_env,
	"set clock frequency command",
	"usage:\n"
	"clockenv clk-arm|clk-axi|clk-mddr|clk-app|clk-dspcore\n"
	"\t |clk-vpclk|clk-gclk|clk-vsclk frequency\n"
);

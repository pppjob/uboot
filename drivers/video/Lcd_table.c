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

#include	"Lcd_table.h"

#define	LCDKEYWORD1	"driver"
#define LCDKEYWORD2 "lcd"
#define LCDKEYWORD3 "YES"
#define	maxlcdname	20

struct	lcdattribe	   *lcdcurrentops=NULL;
vidinfo_t panel_info;

struct lcdattribe Lcdopstbl[]=
{
// id            readid           name              pannel_set_ops       pannel_set_power       	pannel_set_refresh_rate
{LCD_rm68041,	0,"rm68041",	   pannel_set_ops_rm68041,		pannel_set_power_rm68041,		pannel_set_refresh_rate_rm68041},
{LCD_lms350df04,0, "lms350df04",   pannel_set_ops_lms350df04,	pannel_set_power_lms350df04,	pannel_set_refresh_rate_lms350df04},
{LCD_rm68040, 	0,"rm68040",	   pannel_set_ops_rm68040,		pannel_set_power_rm68040,		pannel_set_refresh_rate_rm68040},
{LCD_hsd0701dw1,0, "hsd0701dw1",   pannel_set_ops_hsd0701,		pannel_set_power_hsd0701,		pannel_set_refresh_rate_hsd0701},
{LCD_inno7dd, 	0,"inno7dd",  	   pannel_set_ops_inno7dd,		pannel_set_power_inno7dd,		pannel_set_refresh_rate_inno7dd},
{LCD_inno8d2, 	0,"inno8d2",  	   pannel_set_ops_inno8d2,		pannel_set_power_inno8d2,		pannel_set_refresh_rate_inno8d2},
{LCD_r61581,	0,"r61581",		   pannel_set_ops_r61581,		pannel_set_power_r61581,		pannel_set_refresh_rate_r61581},
{LCD_sharpis035,0,"sharpis035",	   pannel_set_ops_sharp_ls035,	pannel_set_power_sharp_ls035,	pannel_set_refresh_rate_sharp_ls035},
{LCD_tm320,		0,"tm320",	   	   pannel_set_ops_tm320,		pannel_set_power_tm320,		    pannel_set_refresh_rate_tm320},
{LCD_ili9486,	0,"ili9486",       pannel_set_ops_ili9486,		pannel_set_power_ili9486,		 pannel_set_refresh_rate_ili9486},
{LCD_ssd2805c,	0, "ssd2805c",	   pannel_set_ops_ssd2805c,		pannel_set_power_ssd2805c,		pannel_set_refresh_rate_ssd2805c},
{LCD_nt35510,	0, "nt35510",	   pannel_set_ops_nt35510,		pannel_set_power_nt35510,		pannel_set_refresh_rate_nt35510},
{LCD_max,		0,NULL,NULL,NULL,NULL},
};

/*********************
read conf file,find lcd name
**********************/
void FindLcdType(char *lcdname)
{
	factory_data_t *pcal_data=NULL;
	char	*startaddress,*dataaddress;
	uint8_t		i=0;

	pcal_data = factory_data_get(FD_CONFIG);
	if(pcal_data==NULL)
		goto defaultset;

	dataaddress=pcal_data->fd_buf;
	while(1)
	{
		startaddress=strstr(dataaddress,"\n");
		if(startaddress==NULL)
			goto defaultset;
		*startaddress=0;
		startaddress+=strlen("\n");
		if(strlen(dataaddress)==0)  /*two \n*/
		{
			dataaddress=startaddress;
			continue;
		}
		if(strstr(dataaddress,LCDKEYWORD1)==NULL)
		{
			dataaddress=startaddress;
			continue;
		}
		if(strstr(dataaddress,LCDKEYWORD2)==NULL)
		{
			dataaddress=startaddress;
			continue;
		}
		if(strstr(dataaddress,LCDKEYWORD3)==NULL)
		{
			dataaddress=startaddress;
			continue;
		}

		startaddress=strstr(dataaddress,LCDKEYWORD2);
		startaddress+=strlen(LCDKEYWORD2);
		dataaddress=startaddress;
		startaddress=strstr(dataaddress,":");
		startaddress+=strlen(":");

		while((*startaddress!=':')&&(i++<=maxlcdname))
		{
			if(*startaddress==' ')  /*filter space :from :*/
			{
				startaddress++;
				continue;
			}
			*lcdname++=*startaddress;
			startaddress++;
		}
		if(i>maxlcdname)
			goto defaultset;
		factory_data_put(pcal_data);
		return;
	}
	defaultset:
		if(pcal_data)
		{
			atxxfb_dbg("LCD can not get config data \n");
			factory_data_put(pcal_data);
		}
		strcpy(lcdname,DEFAULTLCDNAME);
		return;
}

/*********************
 set lcd pannel info
 *********************/
static void SetPannelinfo(char *name)
{
	uint8_t		id=0;

	for(id=0;id<LCD_max;id++)
	{
			if((name==NULL)||strlen(name)==0)
				break;
			if(!strcmp(Lcdopstbl[id].name,name))
				break;
	}
	switch(id)
	{
		case LCD_rm68041:
			panel_info=panel_info_rm68041;
			break;
		case LCD_lms350df04:
			panel_info=panel_info_lms350df04;
			break;
		case LCD_rm68040:
			panel_info=panel_info_rm68040;
			break;
		case LCD_hsd0701dw1:
			panel_info=panel_info_hsd0701;
			break;
		case LCD_inno7dd:
			panel_info=panel_info_inno7dd;
			break;
		case LCD_inno8d2:
			panel_info=panel_info_inno8d2;
			break;
		case LCD_r61581:
			panel_info=panel_info_r61581;
			break;
		case LCD_sharpis035:
			panel_info=panel_info_sharp_ls035;
			break;
		case LCD_tm320:
			panel_info=panel_info_tm320;
			break;
		case LCD_ili9486:
			panel_info=panel_info_ili9486;
		case LCD_ssd2805c:
			panel_info=panel_info_ssd2805c;
			break;
		case LCD_nt35510:
			panel_info=panel_info_nt35510;
			break;
		default:
			break;
	}

}

/*********************
set lcdcurrentopt
**********************/
static void SetLcdops(char *name)
{
	uint8_t		id=0;

	for(id=0;id<LCD_max;id++)
	{
		if((name==NULL)||strlen(name)==0)
			break;
		if(!strcmp(Lcdopstbl[id].name,name))
			break;
	}
	if((id==0)||(id==LCD_max)) /*no use*/
	{
		id=0;
		atxxfb_dbg("LCD module use default config\n");
	}
	lcdcurrentops=&Lcdopstbl[id];
	atxxfb_dbg("LCD type is %s\n",lcdcurrentops->name);
}

/**********************
Init lcdops
**********************/
void InitLcdops(void)
{
	char lcdname[maxlcdname]={0};

	atxxfb_dbg("LCD module Init config\n");
	FindLcdType(lcdname);
	SetLcdops(lcdname);
	SetPannelinfo(lcdname);
}

/**********************
pannel_set_ops
***********************/
int pannel_set_ops(struct atxxfb *dev)
{
	return lcdcurrentops->pannel_set_ops(dev);
}
/**********************
pannel_set_refresh_rate
***********************/
void pannel_set_refresh_rate(struct clk *lcd_clk)
{
	lcdcurrentops->pannel_set_refresh_rate(lcd_clk);
}

/*********************
pannel_set_power
*********************/
int pannel_set_power(int on_off)
{
	return lcdcurrentops->pannel_set_power(on_off);
}




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
#include <image.h>
#include <u-boot/crc.h>
#include <asm/arch-atxx/hdcvt.h>

/* convert atxx header to u-boot header */
void ahd_to_uhd(atxx_image_header_t *ahd, void *body, image_header_t *uhd)
{
	uint32_t checksum;

	/* Use a middle variable to allow ahd and uhd overlapped */
	image_header_t uheader;
	image_header_t *hdr = (image_header_t *)&uheader;

	memset(hdr, 0, sizeof(image_header_t));

	/* Build new header */
	image_set_magic(hdr, IH_MAGIC);
	image_set_time(hdr, 0);
	image_set_size(hdr, ahd->firm_size);
	image_set_load(hdr, ahd->run_address - sizeof(image_header_t));
	image_set_ep(hdr, ahd->run_address);
	image_set_dcrc(hdr, crc32(0, body, ahd->firm_size));
	image_set_os(hdr, IH_OS_LINUX);
	image_set_arch(hdr, IH_ARCH_ARM);
	image_set_type(hdr, ahd->image_type);
	image_set_comp(hdr, IH_COMP_NONE);
	image_set_name(hdr, ahd->image_name);

	checksum = crc32(0, (void *)hdr, sizeof(image_header_t));
	image_set_hcrc(hdr, checksum);

	memcpy(uhd, hdr, sizeof(image_header_t));
}


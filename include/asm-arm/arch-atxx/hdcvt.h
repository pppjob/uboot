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

#define	IV_SIZE			16
#define	CERT_SIZE		288
#define	SIGE_SIZE		128

#define HEAD_SIGNATURE		0xB0087006

typedef struct atxx_image_header {
	unsigned char		iv[IV_SIZE];
	unsigned int		boot_signature;
	unsigned int		load_address;
	unsigned int		run_address;
	unsigned int		firm_size;
	unsigned int		nand_offset;
	unsigned int		image_type;
	unsigned char		board_name[16];
	unsigned char		image_name[32];
	unsigned char		reserved[8];
	unsigned char 		certificate[CERT_SIZE];
	unsigned char 		signature[SIGE_SIZE];
} atxx_image_header_t;

void ahd_to_uhd(atxx_image_header_t *ahd, void *body, image_header_t *uhd);

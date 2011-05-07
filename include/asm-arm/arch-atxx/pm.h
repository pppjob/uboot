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
#ifndef __ASM_ARCH_PM_H
#define __ASM_ARCH_PM_H
#include <config.h>

#ifdef CONFIG_AT6600 
#include "at6600_pm.h"
#endif

#ifdef CONFIG_AT7700A0 
#include "at7700a0_pm.h"
#endif

#ifdef CONFIG_AT7700B0 
#include "at7700b0_pm.h"
#endif

#endif /*__ASM_ARCH_PM_H*/

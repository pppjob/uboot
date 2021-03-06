/*
 * Copyright (c) 2009 Wind River Systems, Inc.
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _GPIO_H
#define _GPIO_H

#if	defined(CONFIG_BOARD_CAYMAN20)
#include "cayman20_gpio.h"
#elif	defined(CONFIG_BOARD_MIRACLE)
#include "miracle_gpio.h"
#elif	defined(CONFIG_BOARD_G20)
#include "g20_gpio.h"
#elif	defined(CONFIG_BOARD_T3C)
#include "t3c_gpio.h"
#elif	defined(CONFIG_BOARD_W002)
#include "w002_gpio.h"
#elif	defined(CONFIG_BOARD_AD8071)
#include "ad8071_gpio.h"
#elif	defined(CONFIG_BOARD_ATB9004)
#include "atb9004_gpio.h"
#elif	defined(CONFIG_BOARD_FPGA7700B0)
#include "fpga7700b0_gpio.h"
#elif	defined(CONFIG_BOARD_ATB1004)
#include "atb1004_gpio.h"
#elif   defined(CONFIG_BOARD_ATB1005)
#include "atb1005_gpio.h"
#elif   defined(CONFIG_BOARD_G01)
#include "g01_gpio.h"
#elif   defined(CONFIG_BOARD_GAMEBOX)
#include "gbox_gpio.h"
#elif   defined(CONFIG_BOARD_M70P)
#include "m70p_gpio.h"
#elif	defined(CONFIG_BOARD_A500)
#include "a500_gpio.h"
#elif	defined(CONFIG_BOARD_S9L)
#include "s9l_gpio.h"
#elif	defined(CONFIG_BOARD_G23D)
#include "g23d_gpio.h"
#else
#error   "<board>_gpio.h is missing!"
#endif

/*------ This is the interface ----------------------------------------*/

/* Sets the gpio as input or output */
int atxx_set_gpio_direction(int gpio, int is_input);
/* Get the value of a gpio input */
int atxx_gpio_get(int gpio);
/* Set a gpio output */
void atxx_gpio_set(int gpio, int value);
/* Request a gpio before using it */
int atxx_request_gpio(int gpio);
/* Reset and free a gpio after using it */
void atxx_free_gpio(int gpio);

#endif /* _GPIO_H_ */

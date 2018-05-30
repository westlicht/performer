/*
 * This file is part of the libusbhost library
 * hosted at http://github.com/libusbhost/libusbhost
 *
 * Copyright (C) 2015 Amir Hammad <amir.hammad@hotmail.com>
 *
 *
 * libusbhost is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef USBH_LLD_STM32F4_H_
#define USBH_LLD_STM32F4_H_

#include "usbh_core.h"

#include <stdint.h>

BEGIN_DECLS

// pass this to usbh init
extern const usbh_low_level_driver_t usbh_lld_stm32f4_driver_fs;
extern const usbh_low_level_driver_t usbh_lld_stm32f4_driver_hs;

#ifdef USART_DEBUG
void print_channels(const void *drvdata);
#else
#define print_channels(arg) ((void)arg)
#endif

END_DECLS

#endif

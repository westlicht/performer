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

#ifndef USBH_USART_HELPERS_H
#define USBH_USART_HELPERS_H

#include "usbh_core.h"
#include <stdint.h>
#include <stdarg.h>

BEGIN_DECLS

#ifdef USART_DEBUG
void dbg_printf(char const *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
#define LOG_PRINTF(format, ...) dbg_printf(format, ##__VA_ARGS__);
#define LOG_FLUSH()
#else
#define LOG_PRINTF(dummy, ...) ((void)dummy)
#define LOG_FLUSH()
#endif

END_DECLS

#endif

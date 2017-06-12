/*
 * Copyright (C) 2009-2017 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef WTW_PLUG_TEMPLATE_STDINC_H
#define WTW_PLUG_TEMPLATE_STDINC_H

#ifdef _WIN32
#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE 1
#define _ATL_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1
#endif

#define STRICT
#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT	0x0501
#define _WIN32_IE		0x0501
#define WINVER			0x501

#include <cstdio>			// to musi byc PRZED includami z api wtw
#include <windows.h>
#include <plInterface.h>

extern WTWFUNCTIONS* wtw;	// extern, zebysmy mogli uzywac w kazdym miejscu programu

#endif
#endif // WTW_PLUG_TEMPLATE_STDINC_H

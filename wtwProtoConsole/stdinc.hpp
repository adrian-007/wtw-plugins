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

#ifndef STDINC_HPP
#define STDINC_HPP

#define WIN32_LEAN_AND_MEAN
#define STRICT

#define _WTL_NO_CSTRING
#define _ATL_NO_OPENGL
#define _ATL_NO_MSIMG
#define _ATL_NO_COM
#define _ATL_NO_HOSTING
#define _ATL_NO_OLD_NAMES
#define _ATL_NO_COM_SUPPORT
#define _ATL_NO_PERF_SUPPORT
#define _ATL_NO_SERVICE
#define _ATL_NO_DOCHOSTUIHANDLER

#define _WIN32_WINNT 0x0600
#define _WIN32_IE	0x0600
#define WINVER 0x600

#if _MSC_VER <= 1500
#define nullptr 0
#endif

#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>

#include <atltypes.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <string>
#include <vector>

#include <plInterface.h>

extern void* config;
extern WTWFUNCTIONS* wtw;
extern HINSTANCE ghInstance;

inline void cfgGetStr(std::wstring& buf, const wchar_t* name, const wchar_t* def = L"")
{
	wchar_t* tmp;
	wtwGetStr(wtw, config, name, def, &tmp);
	buf = tmp;
	delete[] tmp;
}

inline int cfgGetInt(const wchar_t* name, int def)
{
	return wtwGetInt(wtw, config, name, def);
}

inline void cfgSetStr(const wchar_t* name, const wchar_t* value)
{
	wtwSetStr(wtw, config, name, value);
}

inline void cfgSetInt(const wchar_t* name, int value)
{
	wtwSetInt(wtw, config, name, value);
}

#endif // STDINC_HPP

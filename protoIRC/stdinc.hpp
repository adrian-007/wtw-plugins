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

#ifndef STDINC_H
#define STDINC_H

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
#define _WIN32_WINNT 0x0501
#define _WIN32_IE	0x0501
#define WINVER 0x501

#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atltypes.h>
#include <atlctrls.h>
#include <atlscrl.h>

#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/intrusive_ptr.hpp>

typedef std::vector<std::string> StringVectorA;
typedef std::vector<std::wstring> StringVectorW;

using namespace std;

using boost::int8_t;
using boost::int16_t;
using boost::int32_t;
using boost::int64_t;
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

// WTW stuff ////////////////////////////////////////////////////////
#include <plInterface.h>
#include <wtwOther.h>
#include <cpp/Core.h>
#include <cpp/Globals.h>

extern void* config;
extern HINSTANCE ghInstance;
extern wtw::CCore* core;

inline void cfgGetStr(wstring& buf, const wchar_t* name, const wchar_t* def = L"") {
	wchar_t* tmp;
	wtwGetStr(core, config, name, def, &tmp);
	buf = tmp;
	delete[] tmp;
}

inline int cfgGetInt(const wchar_t* name, int def) {
	return wtwGetInt(core, config, name, def);
}

inline void cfgSetStr(const wchar_t* name, const wchar_t* value) {
	wtwSetStr(core, config, name, value);
}

inline void cfgSetInt(const wchar_t* name, int value) {
	wtwSetInt(core, config, name, value);
}

#define PROTO_CLASS L"IRC"
#define PROTO_NAME L"Internet Relay Chat"

#endif
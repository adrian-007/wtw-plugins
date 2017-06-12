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

// rzeczy potrzebne do kompilacji programu pod win32
// dodatkowo naglowki oraz definicje od systemu, biblioteki standardowej oraz 
// ATL/WTL (szablony na ktorzych zostal napisany graficzny interfejs uzytkownika)

#ifdef _WIN32

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

#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atltypes.h>
#include <atlctrls.h>
#include <atldlgs.h>

#endif // _WIN32

#include <string>
#include <list>

#endif // STDINC_HPP

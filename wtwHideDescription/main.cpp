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

#include "stdinc.h"
#include "Manager.h"
#include "../pluginInfo.h"

WTWFUNCTIONS* wtw = 0;
Manager* mng = 0;
HINSTANCE ghInstance = 0;

WTWPLUGINFO plugInfo =
{
	sizeof(WTWPLUGINFO),										// rozmiar struktury
	L"wtwHideDescription",									// nazwa wtyczki
	L"Ukrywanie opisów",										// opis wtyczki
	__COPYRIGHTS(2009),										// prawa autorskie
	__AUTHOR_NAME,											// autor
	__AUTHOR_CONTACT,										// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,										// strona www autora
	__AUTOUPDATE_URL(L"wtwHideDescription.xml"),			// url do pliku xml z danymi do autoupdate
	PLUGIN_API_VERSION,										// wersja api z ktora zostal skompilowany plug
	MAKE_QWORD(0, 2, 0, 2),									// wersja plug'a
	WTW_CLASS_UTILITY,										// klasa plug'a
	0,														// fcja ktora wtw wywola jezeli user bedzie chcial zobaczyc about plug'a
	L"{D8320BBB-A73E-4B6B-8BF8-52F02717D373}",				// GUID, wymagane...
	0,														// zaleznosci (tablica GUID'ow, ostatnia pozycja MA byc 0
	0, 0, 0, 0												// zarezerwowane
};

bool __stdcall DllMain(HANDLE hInst, DWORD reason, LPVOID /*reserved*/)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	{
							   ghInstance = (HINSTANCE)hInst;
							   break;
	}
	case DLL_PROCESS_DETACH:
	{
							   ghInstance = 0;
							   break;
	}
	default: break;
	}
	return true;
}

extern "C"
{
	WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/)
	{
		return &plugInfo;
	}

	int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f)
	{
		wtw = f;
		mng = new Manager(ghInstance);
		return 0;
	}

	int __stdcall pluginUnload(DWORD /*callReason*/)
	{
		delete mng;
		mng = 0;
		wtw = 0;
		return 0;
	}
} // extern "C"

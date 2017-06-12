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

#include "stdinc.hpp"
#include "pexGGPubDir.hpp"
#include "../pluginInfo.h"

WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;

bool __stdcall DllMain(HANDLE hInst, DWORD reason, LPVOID /*reserved*/)
{
	switch(reason)
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

WTWPLUGINFO plugInfo =
{
	sizeof(WTWPLUGINFO),														// rozmiar struktury
	L"pexGGPubDir",																// nazwa wtyczki
	L"Obs�uga katalogu publicznego w sieci Gadu-Gadu",							// opis wtyczki
	__COPYRIGHTS(2009),															// prawa autorskie
	__AUTHOR_NAME,																// autor
	__AUTHOR_CONTACT,															// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,															// strona www autora
	__AUTOUPDATE_URL(L"pexGGPubDir.xml"),										// url do pliku xml z danymi do autoupdate
	PLUGIN_API_VERSION,															// wersja api
	MAKE_QWORD(0, 0, 1, 1),														// wersja wtyczki
	WTW_CLASS_PROTO_EXT,														// klasa wtyczki
	0,																			// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{B3F01CC1-C83B-4d3f-9A3F-D1DB5E6124AC}",									// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,																			// zaleznosci
	0, 0, 0, 0																	// zarezerwowane (4 pola)
};

/// api functions
extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion)
{
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS* fC)
{
    wtw = fC;
	pexGGPubDir::create(new pexGGPubDir(ghInstance));
    return 0;
}

int __stdcall pluginUnload(DWORD callReason)
{
	pexGGPubDir::destroy();
	return 0;
}

}

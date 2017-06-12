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
#include "wtwProtoConsole.hpp"
#include "../pluginInfo.h"

void* config = nullptr;
WTWFUNCTIONS* wtw = nullptr;
HINSTANCE ghInstance = nullptr;
HINSTANCE hRichEditDll = nullptr;

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
	sizeof(WTWPLUGINFO),								// rozmiar struktury
	L"wtwProtoConsole",									// nazwa wtyczki
	L"Konsola protoko��w",								// opis wtyczki
	__COPYRIGHTS(2009),									// prawa autorskie
	__AUTHOR_NAME,										// autor
	__AUTHOR_CONTACT,									// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,									// strona www autora
	nullptr,											// url do pliku xml z danymi do autoupdate										
	PLUGIN_API_VERSION,									// wersja api
	MAKE_QWORD(0, 0, 0, 1),								// wersja wtyczki
	WTW_CLASS_UTILITY,									// klasa wtyczki
	0,													// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{B907FCE2-3C0E-475a-BAAB-FC8B5D1E6662}",			// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,													// zaleznosci
	0, 0, 0, 0											// zarezerwowane (4 pola)
};

/// api functions
extern "C" 
{
	WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion) 
	{
		return &plugInfo;
	}

	int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS* fC) 
	{
		wtw = fC;
		hRichEditDll = ::LoadLibrary(_T("RICHED20.DLL"));

		wtwProtoConsole::create(new wtwProtoConsole());
		return 0;
	}

	int __stdcall pluginUnload(DWORD callReason) 
	{
		wtwProtoConsole::destroy();

		if(hRichEditDll != nullptr)
		{
			::FreeLibrary(hRichEditDll);
			hRichEditDll = nullptr;
		}

		return 0;
	}
}

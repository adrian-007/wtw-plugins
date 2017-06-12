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
#include "Emots.hpp"
#include "../pluginInfo.h"

#include <gdiplus.h>
using namespace Gdiplus;

void* config = 0;
WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;
ULONG_PTR gdiplusToken;

bool __stdcall DllMain(HANDLE hInst, DWORD reason, LPVOID /*reserved*/) {
	switch(reason) {
		case DLL_PROCESS_ATTACH: {
			ghInstance = (HINSTANCE)hInst;
			break;
		}
		case DLL_PROCESS_DETACH: {
			ghInstance = 0;
			break;
		}
		default: break;
	}
	return true;
}

WTWPLUGINFO plugInfo = {
	sizeof(WTWPLUGINFO),								// rozmiar struktury
	L"Emots",											// nazwa wtyczki
	L"Emotikony w WTW",									// opis wtyczki
	__COPYRIGHTS(2009),									// prawa autorskie
	__AUTHOR_NAME,										// autor
	__AUTHOR_CONTACT,									// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,									// strona www autora
	__AUTOUPDATE_URL(L"emots.xml"),						// url do pliku xml z danymi do autoupdate										
	PLUGIN_API_VERSION,									// wersja api
	MAKE_QWORD(0, 7, 5, 1),								// wersja wtyczki
	WTW_CLASS_UTILITY,									// klasa wtyczki
	0,													// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{B897076D-D098-4cae-8E93-7819A1F60073}",			// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,													// zaleznosci
	0, 0, 0, 0											// zarezerwowane (4 pola)
};

/// api functions
extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS* fC) {
    wtw = fC;
	{
		wtwMyConfigFile pBuff;
		initStruct(pBuff);

		pBuff.bufferSize = MAX_PATH + 1;
		pBuff.pBuffer = new wchar_t[MAX_PATH + 1];

		wtw->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&pBuff), reinterpret_cast<WTW_PARAM>(ghInstance));

		if(wtw->fnCall(WTW_SETTINGS_INIT_EX, reinterpret_cast<WTW_PARAM>(pBuff.pBuffer), reinterpret_cast<WTW_PARAM>(&config)) != S_OK)
		{
			config = 0;
		}
		else
		{
			wtw->fnCall(WTW_SETTINGS_READ, (WTW_PARAM)config, NULL);
		}

		delete[] pBuff.pBuffer;
	}

	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}

	EmotsManager::create(new EmotsManager(ghInstance));
	
    return 0;
}

int __stdcall pluginUnload(DWORD callReason) {
	EmotsManager::destroy();

	wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
	wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
	config = 0;

	GdiplusShutdown(gdiplusToken);

	return 0;
}

}

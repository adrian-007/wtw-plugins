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
#include "wtwPastebin.hpp"
#include "SettingsPage.h"
#include "../pluginInfo.h"

void* config = 0;
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
	sizeof(WTWPLUGINFO),										// rozmiar struktury
	L"wtwPastebin",												// nazwa wtyczki
	L"Wysy�ka tekstu na pastebin.com",							// opis wtyczki
	__COPYRIGHTS(2009),											// prawa autorskie
	__AUTHOR_NAME,												// autor
	__AUTHOR_CONTACT,											// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,											// strona www autora
	__AUTOUPDATE_URL(L"wtwPastebin.xml"),						// url do pliku xml z danymi do autoupdate
	PLUGIN_API_VERSION,											// wersja api
	MAKE_QWORD(0, 3, 0, 2),										// wersja wtyczki
	WTW_CLASS_UTILITY,											// klasa wtyczki
	0,															// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{C55245D3-781B-4c23-978B-E94E16F1B7A3}",					// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,															// zaleznosci
	0, 0, 0, 0													// zarezerwowane (4 pola)
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

	wtwPastebin::create(new wtwPastebin(ghInstance));

	// dodawanie strony do opcji
	/*{
		wtwOptionPageDef def;
		initStruct(def);

		def.callback	= SettingsPage::callback;
		def.id			= L"wtwPastebin/Options";
		def.caption		= L"pastebin.com";
		def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;
		def.iconId		= PLUGIN_ICON;

		wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
	}*/

    return 0;
}

int __stdcall pluginUnload(DWORD callReason)
{
	wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

	wtwPastebin::destroy();

	wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
	wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
	config = 0;
	return 0;
}

}

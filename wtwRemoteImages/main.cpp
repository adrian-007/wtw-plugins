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
#include "wtwRemoteImages.hpp"
#include "SettingsPage.h"

#include "../utils/wtw.hpp"
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
	sizeof(WTWPLUGINFO),													// rozmiar struktury
	L"wtwRemoteImages",														// nazwa wtyczki
	L"Automagiczne wysy�anie obrazk�w na " SERVICE_PROVIDER L" ^^",			// opis wtyczki
	__COPYRIGHTS(2009),														// prawa autorskie
	__AUTHOR_NAME,															// autor
	__AUTHOR_CONTACT,														// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,														// strona www autora
	__AUTOUPDATE_URL(L"wtwRemoteImages.xml"),								// url do pliku xml z danymi do autoupdate								
	PLUGIN_API_VERSION,														// wersja api
	MAKE_QWORD(0, 3, 0, 1),													// wersja wtyczki
	WTW_CLASS_UTILITY,														// klasa wtyczki
	0,																		// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{C16D6438-484C-4405-B113-820A8F1AD23E}",								// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,																		// zaleznosci
	0, 0, 0, 0																// zarezerwowane (4 pola)
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

	wtw_t::preloadIcon(wtw, ghInstance, L"wtwRemoteImages/icon16", L"wtwRemoteImages16.png", IDB_PNG1);
	wtw_t::preloadIcon(wtw, ghInstance, L"wtwRemoteImages/icon24", L"wtwRemoteImages24.png", IDB_PNG2);
	wtw_t::preloadIcon(wtw, ghInstance, L"wtwRemoteImages/icon32", L"wtwRemoteImages32.png", IDB_PNG3);
	wtw_t::preloadIcon(wtw, ghInstance, L"wtwRemoteImages/icon48", L"wtwRemoteImages48.png", IDB_PNG4);

	wtwRemoteImages::create(new wtwRemoteImages(ghInstance));

	// dodawanie strony do opcji
	{
		wtwOptionPageDef def;
		initStruct(def);

		def.callback	= SettingsPage::callback;
		def.id			= L"wtwRemoteImages/Options";
		def.caption		= L"Obrazki (" SERVICE_PROVIDER L")";
		def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;
		def.iconId		= L"wtwRemoteImages/icon16";

		wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
	}
	
    return 0;
}

int __stdcall pluginUnload(DWORD callReason)
{
	wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

	wtwRemoteImages::destroy();

	wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
	wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
	config = 0;
	
	return 0;
}

}

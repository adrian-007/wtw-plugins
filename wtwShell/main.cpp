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
#include "wtwShell.hpp"
#include "Command.h"
#include "../pluginInfo.h"

void* config = 0;
WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;

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
	sizeof(WTWPLUGINFO),									// rozmiar struktury
	L"wtwShell",											// nazwa wtyczki
	L"Obs�uga pow�oki Windows z poziomu listy kontakt�w",	// opis wtyczki
	__COPYRIGHTS(2009),										// prawa autorskie
	__AUTHOR_NAME,											// autor
	__AUTHOR_CONTACT,										// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,										// strona www autora
	__AUTOUPDATE_URL(L"wtwShell.xml"),						// url do pliku xml z danymi do autoupdate
	PLUGIN_API_VERSION,										// wersja api
	MAKE_QWORD(0, 1, 0, 2),									// wersja wtyczki
	WTW_CLASS_PROTOCOL,										// klasa wtyczki
	0,														// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{FDB53577-79FC-4004-A285-16066E678055}",				// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,														// zaleznosci
	0, 0, 0, 0												// zarezerwowane (4 pola)
};

/// api functions
extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS* fC) {
    wtw = fC;
	__LOG(wtw, PROTO_NAME, L"wtwShell protocol plugin starting up...");

	/*{
		wtwMyConfigFile pBuff;
		initStruct(pBuff);

		pBuff.bufferSize = MAX_PATH + 1;
		pBuff.pBuffer = new wchar_t[MAX_PATH + 1];

		wtw->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&pBuff), reinterpret_cast<WTW_PARAM>(ghInstance));

		config = (void*)wtw->fnCall(WTW_SETTINGS_INIT, (WTW_PARAM)pBuff.pBuffer, (WTW_PARAM)ghInstance);
		wtw->fnCall(WTW_SETTINGS_READ, (WTW_PARAM)config, NULL);

		delete[] pBuff.pBuffer;
	}*/

/*	{
		wtwLoadIconSet lis;
		initStruct(lis);

		lis.flags = WTW_ICSET_LOAD_FLAG_DEFLOC;

		lis.iStates = 3;
		lis.pStates = new int[lis.iStates];

		lis.pStates[0] = WTW_PRESENCE_OFFLINE;
		lis.pStates[1] = WTW_PRESENCE_ONLINE;
		lis.pStates[2] = WTW_PRESENCE_AWAY;

		lis.setID = L"IRC/Icon";
		lis.iType = WTW_ICSET_TYPE_PRE_TEXT;
		lis.netClass = L"IRC";

		wtw->fnCall(WTW_ICSET_LOAD_PNG, reinterpret_cast<WTW_PARAM>(&lis), 0);
		delete [] lis.pStates;
	}*/

	wtwShell::create(new wtwShell(ghInstance));
	wtwShell::instance().load();

    return 0;
}

int __stdcall pluginUnload(DWORD callReason) {
	__LOG(wtw, PROTO_NAME, L"wtwShell protocol plugin shutting down...");

	wtwShell::instance().save();

	wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);
	wtw->fnCall(WTW_MENU_ITEM_CLEAR, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

	wtwShell::destroy();

	//wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
	//wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
	//config = 0;
	return 0;
}

}

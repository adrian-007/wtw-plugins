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
#include "protoBonjour.hpp"
#include "SettingsWindow.h"
#include "../pluginInfo.h"

void* config = 0;
HINSTANCE ghInstance = 0;

//BOOL IsSystemServiceDisabled();
extern "C" BOOL __cdecl IsSystemServiceDisabled();

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
	sizeof(WTWPLUGINFO),
	L"protoBonjour",
	L"Bonjour protocol support for WTW",
	__COPYRIGHTS(2009),
	__AUTHOR_NAME,
	__AUTHOR_CONTACT,
	__AUTHOR_WEBSITE,
	__AUTOUPDATE_URL(L"protoBonjour.xml"),							
//	L"http://wtw.adrian-007.eu/au/protoBonjour.xml",
	PLUGIN_API_VERSION,
	MAKE_QWORD(0, 0, 0, 4),
	WTW_CLASS_PROTOCOL,
	0,
	L"{B9DDE825-86C5-4a3c-A0BF-76F0C5588FD3}",
	0,
	0, 0, 0, 0 //zarezerwowane
};

extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f) {
	if(IsSystemServiceDisabled())
	{
		MessageBoxW(0, L"Usługa Bonjour nie jest zainstalowana/aktywna. Jest ona niezbędna do używania tej wtyczki, także może zechciał(a)byś zainstalować/aktywować usługę Bonjour na swoim komputerze i ponownie uruchomić WTW...", L"WTW - protoBonjour", MB_ICONERROR);
		return 0;
	}

	srand(static_cast<unsigned int>(time(0)));

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	using wtw_t::api;

	new api(f, ghInstance);

	{
		wtwMyConfigFile pBuff;
		initStruct(pBuff);

		pBuff.bufferSize = MAX_PATH + 1;
		pBuff.pBuffer = new wchar_t[MAX_PATH + 1];

		api::get()->call(WTW_SETTINGS_GET_MY_CONFIG_FILE, &pBuff, ghInstance);

		if(api::get()->call(WTW_SETTINGS_INIT_EX, pBuff.pBuffer, &config) != S_OK)
		{
			config = 0;
		}
		else
		{
			api::get()->call(WTW_SETTINGS_READ, config);
		}

		delete[] pBuff.pBuffer;
	}

	// ladowanie ikony
	wtw_t::preloadIcon(api::get(), api::get()->instance(), L"protoBonjour/icon16", L"protoBonjour16.png", IDB_PNG1);

	// ladowanie setu
	{
		wtwLoadIconSet lis;
		initStruct(lis);

		lis.flags = WTW_ICSET_LOAD_FLAG_DEFLOC;

		lis.iStates = 4;
		lis.pStates = new int[lis.iStates];

		lis.pStates[0] = WTW_PRESENCE_OFFLINE;
		lis.pStates[1] = WTW_PRESENCE_ONLINE;
		lis.pStates[2] = WTW_PRESENCE_AWAY;
		lis.pStates[3] = WTW_PRESENCE_DND;

		lis.setID = PROTO_CLASS L"/Icon";
		lis.iType = WTW_ICSET_TYPE_PRE_TEXT;
		lis.netClass = PROTO_CLASS;

		api::get()->call(WTW_ICSET_LOAD_PNG, &lis);
		delete [] lis.pStates;
	}

	{
		wtwOptionPageDef def;
		initStruct(def);

		def.callback	= SettingsWindow::callback;
		def.id			= L"protoBonjour/Options";
		def.caption		= L"Bonjour (LAN)";
		def.parentId	= WTW_OPTIONS_GROUP_NETWORK;
		def.iconId		= L"protoBonjour/icon16";

		api::get()->call(WTW_OPTION_PAGE_ADD, wtw_t::api::get()->instance(), &def);
	}

	protoBonjour::create(new protoBonjour(L"{18CBB4EC-302E-4b53-AA6F-72B8446BDA63}"));

    return 0;
}

int __stdcall pluginUnload(DWORD /*callReason*/) {
	using wtw_t::api;

	if(wtw_t::api::get() != 0)
	{
		api::get()->call(WTW_OPTION_PAGE_REMOVE_ALL, api::get()->instance());

		api::get()->call(WTW_OPTION_PAGE_REMOVE_ALL, api::get()->instance());
		api::get()->call(WTW_MENU_ITEM_CLEAR, api::get()->instance());

		protoBonjour::destroy();

		api::get()->call(WTW_MENU_ITEM_CLEAR, api::get()->instance());

		api::get()->call(WTW_SETTINGS_WRITE, config);
		api::get()->call(WTW_SETTINGS_DESTROY, config, api::get()->instance());
		config = 0;

		wtw_t::api::cleanup();
		WSACleanup();
	}

    return 0;
}
} // extern "C"

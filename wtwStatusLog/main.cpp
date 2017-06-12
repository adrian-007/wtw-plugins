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
#include "Logger.hpp"
#include "ArchPage.h"
#include "../pluginInfo.h"

WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;
void* config = NULL;

WTWPLUGINFO plugInfo = {
   sizeof(WTWPLUGINFO),
   L"wtwStatusLog",
   L"Logowanie statusów",
	__COPYRIGHTS(2009),
	__AUTHOR_NAME,
	__AUTHOR_CONTACT,
	__AUTHOR_WEBSITE,
	__AUTOUPDATE_URL(L"wtwStatusLog.xml"),
   PLUGIN_API_VERSION,
   MAKE_QWORD(0, 4, 7, 2),
   WTW_CLASS_UTILITY,
   0,
   L"{1B6AADCF-BFEC-4B1E-9CE9-690C2CC3A194}",
   0,
   0, 0, 0, 0 //zarezerwowane
};

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

extern "C" {

WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f) {
	InitCommonControls();
    AtlInitCommonControls(ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES);

	wtw = f;
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

	Logger::createInstance(new Logger);

	// dodawanie strony do opcji
	{
		wtwOptionPageDef def;

		def.callback	= ArchPage::callback;
		def.id			= L"wtwStatusLog/LogPage";
		def.caption		= L"Statusy";
		wtw->fnCall(WTW_ADD_ARCH_PAGE, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
	}
    return 0;
}

int __stdcall pluginUnload(DWORD /*callReason*/) {
	wtw->fnCall(WTW_REMOVE_ARCH_PAGES, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

	Logger::deleteInstance();

	wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), NULL);
	wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));

    return 0;
}
} // extern "C"

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
#include "Changer.h"
#include "SettingsPage.h"
#include "../pluginInfo.h"

WTWFUNCTIONS* wtw = 0;
Changer* changer = 0;
HINSTANCE ghInstance = 0;
void* config = 0;

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
   L"wtwAutoDescription",
   L"Automatycznia zmiana opisu",
	__COPYRIGHTS(2009),
	__AUTHOR_NAME,
	__AUTHOR_CONTACT,
	__AUTHOR_WEBSITE,
	__AUTOUPDATE_URL(L"wtwAutoDescription.xml"),
   PLUGIN_API_VERSION,
   MAKE_QWORD(0, 1, 0, 2),
   WTW_CLASS_UTILITY,
   0,
   L"{94F85897-5F74-4216-877C-16372C1EAA90}",
   0, 
   0, 0, 0, 0 //zarezerwowane
};

WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	bool active = changer->changeState();
	{
		wtw->fnCall(WTW_MENU_ITEM_DELETE, wParam, 0);

		wtwMenuItemDef mi;
		initStruct(mi);

		mi.itemId		= L"wtwAutoDesc/Active";
		mi.menuID		= WTW_MENU_ID_MAIN_OPT;
		mi.callback		= MenuCallback;
		mi.hModule		= ghInstance;
		mi.menuCaption	= active ? L"Dezaktywuj wtwAutoDescription" : L"Aktywuj wtwAutoDescription";

		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
	}
	{
		wtwTrayNotifyDef nt;
		initStruct(nt);
		nt.textMain = L"wtwAutoDescription";
		nt.textLower = active ? L"Automatyczna zmiana opisu włączona" : L"Automatyczna zmiana opisu wyłączona";
		wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), NULL);
	}
	return 0;
}

extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f) {
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
	{
		wtwOptionPageDef def;
		initStruct(def);

		def.callback	= SettingsPage::callback;
		def.id			= L"wtwAutoDesc/Options";
		def.caption		= L"Automatyczne opisy";
		def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;

		wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
	}
	{
		wtwMenuItemDef mi;
		initStruct(mi);

		mi.itemId		= L"wtwAutoDesc/Active";
		mi.menuID		= WTW_MENU_ID_MAIN_OPT;
		mi.callback		= MenuCallback;
		mi.hModule		= ghInstance;
		mi.menuCaption	= L"Aktywuj wtwAutoDescription";

		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
	}

	if(!changer)
		changer = new Changer;
    return 0;
}

int __stdcall pluginUnload(DWORD /*callReason*/) {
	wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

	if(changer) {
		delete changer;
		changer = 0;
	}
	wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), NULL);
	wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
    return 0;
}
} // extern "C"

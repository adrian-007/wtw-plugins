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
#include "SettingsPage.h"
#include "TabControl.h"

#include "../utils/wtw.hpp"
#include "../pluginInfo.h"

#include <wtwPanels.h>

WTWPLUGINFO plugInfo = {
   sizeof(WTWPLUGINFO),
   L"wtwListTabs",
   L"Implementacja wbudowanych filtrów jako kart na liście kontaktów (konnektowcy, można szaleć)",
	__COPYRIGHTS(2009),
	__AUTHOR_NAME,
	__AUTHOR_CONTACT,
	__AUTHOR_WEBSITE,
	__AUTOUPDATE_URL(L"wtwListTabs.xml"),
   PLUGIN_API_VERSION,
   MAKE_QWORD(0, 4, 3, 2),
   WTW_CLASS_UTILITY,
   0,
   L"{FC7BFE97-90D6-486E-B95F-B88A55CBDAF4}",
   0,
   0, 0, 0, 0
};

HINSTANCE ghInstance = 0;
WTWFUNCTIONS* wtw = 0;
void* config = 0;

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
	}
	return true;
}

extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS *fC) 
{
	wtw = fC;
	
	wtw_t::preloadIcon(wtw, ghInstance, L"wtwListTabs/icon16", L"wtwListTabs16.png", IDB_PNG1);
	wtw_t::preloadIcon(wtw, ghInstance, L"wtwListTabs/icon32", L"wtwListTabs32.png", IDB_PNG2);

	AtlInitCommonControls(ICC_STANDARD_CLASSES | ICC_TAB_CLASSES); 

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
		def.id			= L"wtwListTabs/Options";
		def.caption		= L"wtwListTabs";
		def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;
		def.iconId		= L"wtwListTabs/icon16";

		wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
	}

	TabControl::init();

	TabControl::getInstance()->Create();
	TabControl::getInstance()->RebuildTabs();
	TabControl::getInstance()->SelectTab(cfgGetInt(SETTING_LAST_TAB_POS, 0));
	//TabControl::getInstance()->RepaintTabs();
	TabControl::getInstance()->RepositionTabs(true);
	TabControl::getInstance()->ActivatePanel();

    return 0;
}

int __stdcall pluginUnload(DWORD callReason)
{
	wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);
	
	//TabControl::getInstance()->Destroy();
	TabControl::cleanup();

	wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
	wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
	config = 0;	
	wtw = 0;

    return 0;
}
} // extern "C"

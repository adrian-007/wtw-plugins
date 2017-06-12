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
#include "Replacer.hpp"
#include "SettingsWindow.h"
#include "../pluginInfo.h"

void* config = 0;
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
	sizeof(WTWPLUGINFO),
	L"wtwRegexReplacer",
	L"Zastępowanie wyrazów na podstawie wyrażeń regularnych",
	__COPYRIGHTS(2009),
	__AUTHOR_NAME,
	__AUTHOR_CONTACT,
	__AUTHOR_WEBSITE,
	__AUTOUPDATE_URL(L"wtwRegexReplacer.xml"),
	PLUGIN_API_VERSION,
	MAKE_QWORD(0, 3, 8, 2),
	WTW_CLASS_UTILITY,
	0,
	L"{187ACBB1-885D-4706-9DFE-8164C935D235}",
	0,
	0, 0, 0, 0 //zarezerwowane
};

extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f) {
	new wtw_t::api(f, ghInstance);

	Replacer::create(new Replacer);

	// ladowanie ikony
	wtw_t::preloadIcon(wtw_t::api::get(), wtw_t::api::get()->instance(), L"wtwRegexReplacer/icon16", L"wtwRegexReplacer16.png", IDB_PNG1);
	wtw_t::preloadIcon(wtw_t::api::get(), wtw_t::api::get()->instance(), L"wtwRegexReplacer/icon32", L"wtwRegexReplacer32.png", IDB_PNG2);

	{
		wtwOptionPageDef def;
		initStruct(def);

		def.callback	= SettingsWindow::callback;
		def.id			= L"wtwRegexReplacer/Options";
		def.caption		= L"Replacer";
		def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;
		def.iconId		= L"wtwRegexReplacer/icon16";

		wtw_t::api::get()->call(WTW_OPTION_PAGE_ADD, wtw_t::api::get()->instance(), &def);
	}

    return 0;
}

int __stdcall pluginUnload(DWORD /*callReason*/) {
	wtw_t::api::get()->call(WTW_OPTION_PAGE_REMOVE_ALL, wtw_t::api::get()->instance());

	Replacer::destroy();

	wtw_t::api::cleanup();

    return 0;
}
} // extern "C"

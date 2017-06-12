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
#include "protoIRC.hpp"
#include "SettingsPage.h"
#include "ContactPage.h"
#include "Account.h"

#include "version.hpp"
#include "../pluginInfo.h"

void* config = nullptr;
HINSTANCE ghInstance = nullptr;
wtw::CCore* core = nullptr;

bool __stdcall DllMain(HANDLE hInst, DWORD reason, LPVOID /*reserved*/)
{
	switch (reason)
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
	L"IRC Protocol",									// nazwa wtyczki
	L"IRC protocol support for WTW",					// opis wtyczki
	__COPYRIGHTS(2009),									// prawa autorskie
	__AUTHOR_NAME,										// autor
	__AUTHOR_CONTACT,									// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,									// strona www autora
	__AUTOUPDATE_URL(L"protoIRC.xml"),					// url do pliku xml z danymi do autoupdate										
	PLUGIN_API_VERSION,									// wersja api
	PLUGIN_VERSION,										// wersja wtyczki
	WTW_CLASS_PROTOCOL,									// klasa wtyczki
	0,													// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{635D0891-4596-4831-B5BC-519A7E92EF29}",			// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,													// zaleznosci
	0, 0, 0, 0											// zarezerwowane (4 pola)
};

/// api functions
extern "C" {
	WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion)
	{
		return &plugInfo;
	}

	int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* fC)
	{
		wtw::libWtwInit(fC);
		core = wtw::g_pF;

		// inicjalizacja socket'ow
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);

		__LOG(core, PROTO_CLASS, L"IRC protocol plugin starting up...");

		{
			wtwMyConfigFile pBuff;
			initStruct(pBuff);

			pBuff.bufferSize = MAX_PATH + 1;
			pBuff.pBuffer = new wchar_t[MAX_PATH + 1];

			core->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&pBuff), reinterpret_cast<WTW_PARAM>(ghInstance));

			if (core->fnCall(WTW_SETTINGS_INIT_EX, reinterpret_cast<WTW_PARAM>(pBuff.pBuffer), reinterpret_cast<WTW_PARAM>(&config)) != S_OK)
			{
				config = 0;
			}
			else
			{
				core->fnCall(WTW_SETTINGS_READ, (WTW_PARAM)config, NULL);
			}

			delete[] pBuff.pBuffer;
		}

	{
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

		if (core->fnCall(WTW_ICSET_LOAD_PNG, lis, 0) != S_OK)
		{
			std::wstring defaultIconsPath = core->getProfileDir();
			defaultIconsPath += L"PluginData\\setIRC\\";
			lis.setPath = defaultIconsPath.c_str();
			lis.flags = 0;

			core->fnCall(WTW_ICSET_LOAD_PNG, lis, 0);
		}

		delete[] lis.pStates;
	}

		IRCManager::create(new IRCManager(ghInstance));
		IRCManager::instance().load();

		return 0;
	}

	int __stdcall pluginUnload(DWORD callReason)
	{
		__LOG(core, PROTO_CLASS, L"IRC protocol plugin shutting down...");

		IRCManager::instance().save();

		core->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);
		core->fnCall(WTW_MENU_DESTROY_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

		IRCManager::destroy();

		core->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
		core->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
		config = 0;

		// zwolnienie socket'ow
		WSACleanup();

		wtw::libWtwTerminate();

		return 0;
	}

}

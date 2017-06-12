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

#include "Sounds.hpp"
#include "SettingsPage.h"
#include "../pluginInfo.h"

void* config = 0;
WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;

#define SOUNDS_ICON_ID WTW_GRAPH_ID_AUDIO

WTWPLUGINFO plugInfo = 
{
	sizeof(WTWPLUGINFO),									// rozmiar struktury
	L"Sounds",												// nazwa wtyczki
	L"Powiadomienia dźwiękowe o zdarzeniach",				// opis wtyczki
	__COPYRIGHTS(2009),										// prawa autorskie
	__AUTHOR_NAME,											// autor
	__AUTHOR_CONTACT,										// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,										// strona www autora
	__AUTOUPDATE_URL(L"sounds.xml"),						// url do pliku xml z danymi do autoupdate										
	PLUGIN_API_VERSION,										// wersja api
	MAKE_QWORD(0, 9, 5, 0),									// wersja wtyczki
	WTW_CLASS_UTILITY,										// klasa wtyczki
	0,														// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{B897076D-D098-4cae-8E93-7719A1F60073}",				// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,														// zaleznosci
	0, 0, 0, 0												// zarezerwowane (4 pola)
};

bool __stdcall DllMain(HANDLE hInst, DWORD reason, LPVOID /*reserved*/) 
{
	switch(reason) 
	{
	case DLL_PROCESS_ATTACH: 
		{
			ghInstance = reinterpret_cast<HINSTANCE>(hInst);
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

bool switchMute();

WTW_PTR MenuCallback(WTW_PARAM /*wParam*/, WTW_PARAM /*lParam*/, void* /*ptr*/) 
{
	switchMute();
	return 0;
}

WTW_PTR panelCallback(WTW_PARAM wParam, WTW_PARAM /*lParam*/, void* /*ptr*/)
{
	wtwPanelEvent* pe = reinterpret_cast<wtwPanelEvent*>(wParam);

	if(pe != 0 && wcscmp(pe->panelId, L"sounds/muteOnOff") == 0)
	{
		switch(pe->event)
		{
		case WTW_PANEL_EVENT_LBUTTON:
			{
				switchMute();
				break;
			}
		case WTW_PANEL_EVENT_RBUTTON:
			{
				break;
			}
		}
	}

	return 0;
}

bool switchMute()
{
	bool mute = SoundsManager::instance().getPlayer()->getMute();

	wtwMenuItemDef mi;
	initStruct(mi);

	wtwPanelDef p;
	initStruct(p);

	wtwTrayNotifyDef nt;
	initStruct(nt);

	mi.itemId		= L"Sounds/SetSoundActivity";
	mi.menuID		= WTW_MENU_ID_TRAY;
	mi.flags		= WTW_MENU_MODIFY_FLAGS;

	p.panelId		= L"sounds/muteOnOff";
	p.flags			= WTW_PANEL_FLAG_AS_BUTTON | WTW_PANEL_FLAG_MOD_FLAG;

	if(mute == false)
	{
		mi.flags	|= WTW_MENU_ITEM_FLAG_CHECKED;
		p.flags		|= WTW_PANEL_FLAG_CHECKED;
	}

	wtw->fnCall(WTW_MENU_ITEM_MODIFY, reinterpret_cast<WTW_PARAM>(&mi), 0);
	wtw->fnCall(WTW_PANEL_MOD, reinterpret_cast<WTW_PARAM>(&p), 0);

	nt.textMain		= L"Sounds";
	nt.textLower	= mute ? L"Powiadomienia dźwiękowe nieaktywne" : L"Powiadomienia dźwiękowe aktywne";
	nt.iconId		= SOUNDS_ICON_ID;
	nt.graphType	= WTW_TN_GRAPH_TYPE_SKINID;
	wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), 0);

	return mute;
}

/// api functions
extern "C"
{
	WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) 
	{
		return &plugInfo;
	}

	int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* fC)
	{
		//InitCommonControls();
		//AtlInitCommonControls(ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES);

		wtw = fC;

		{
			wtwMyConfigFile pBuff;
			initStruct(pBuff);

			pBuff.bufferSize	= MAX_PATH + 1;
			pBuff.pBuffer		= new wchar_t[pBuff.bufferSize];
			memset(pBuff.pBuffer, 0, pBuff.bufferSize * sizeof(wchar_t));

			wtw->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&pBuff), reinterpret_cast<WTW_PARAM>(ghInstance));

			if(wtw->fnCall(WTW_SETTINGS_INIT_EX, reinterpret_cast<WTW_PARAM>(pBuff.pBuffer), reinterpret_cast<WTW_PARAM>(&config)) != S_OK)
			{
				config = 0;
			}
			else
			{
				wtw->fnCall(WTW_SETTINGS_READ, reinterpret_cast<WTW_PARAM>(config), 0);
			}

			delete[] pBuff.pBuffer;

			if(config == 0)
			{
				MessageBoxW(0, L"Nie udało się otworzyć pliku konfiguracyjnego... bummer", L"Sounds", MB_ICONERROR);
				return 1;
			}
		}

		CoInitializeEx(0, 0);

		SoundsManager::createInstance();

		// dodawanie strony do opcji
		{
			wtwOptionPageDef def;
			initStruct(def);

			def.callback	= SettingsPage::callback;
			def.id			= L"Sounds/Options";
			def.caption		= L"Dźwięki";
			def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;
			def.iconId		= SOUNDS_ICON_ID;
			wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
		}

		// dodawanie pozycji do menu okna glownego i menu w trayu
		{
			wtwMenuItemDef mi;
			initStruct(mi);

			mi.itemId		= L"Sounds/SetSoundActivity";
			mi.callback		= MenuCallback;
			mi.hModule		= ghInstance;
			mi.menuCaption	= L"Dźwięki";
			mi.menuID		= WTW_MENU_ID_TRAY;
			mi.flags		= WTW_MENU_ITEM_FLAG_CHECKED;

			wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
		}

		// przycisk na panelu
		{
			wtwPanelDef p;
			p.panelId		= L"sounds/muteOnOff";
			p.panelIcon		= SOUNDS_ICON_ID;
			p.callback		= panelCallback;
			p.panelHint		= L"Dźwięki";
			p.flags			= WTW_PANEL_FLAG_AS_BUTTON | WTW_PANEL_FLAG_CHECKED;

			wtw->fnCall(WTW_PANEL_ADD, reinterpret_cast<WTW_PARAM>(&p), 0);
		}

		return 0;
	}

	int __stdcall pluginUnload(DWORD /*callReason*/)
	{
		wtwPanelDef p;
		p.panelId = L"sounds/muteOnOff";
		wtw->fnCall(WTW_PANEL_DEL, reinterpret_cast<WTW_PARAM>(&p), 0);

		wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);
		wtw->fnCall(WTW_MENU_ITEM_CLEAR, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

		SoundsManager::destroyInstance();

		CoUninitialize();

		if(config != 0)
		{
			wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
			wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
			config = 0;
		}

		wtw = 0;
		return 0;
	}
}

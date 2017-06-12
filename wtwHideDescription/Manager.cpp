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
#include "Manager.h"

#include <algorithm>
using std::wstring;

#define LOG(x) __LOG(wtw, L"WHID", x) // piekna nazwa...

#define MENU_ITEM_ID_ADD	L"wtwHideDesc/add"
#define MENU_ITEM_NAME_ADD	L"Ukryj opis"

#define MENU_ITEM_ID_REM	L"wtwHideDesc/rem"
#define MENU_ITEM_NAME_REM	L"Poka� opis"

Manager::Manager(HINSTANCE hInst) : menuRebuildHook(0), presenceHook(0), hInstance(hInst), emptyString(L"")
{
	{
		// dodajemy pozycje do menu z ktorych pozniej bedziemy korzystac
		// nie ma znaczenia ile tu dodamy pozycji, w hook'u _MENU_REBUILD
		// wybierzemy sobie ktore maja byc widoczne dla danego kontaktu
		wtwMenuItemDef mi;
		initStruct(mi);

		mi.menuID = WTW_MENU_ID_CONTACT;
		mi.hModule = hInstance;
		mi.callback = Manager::MenuCallback;
		mi.cbData = static_cast<void*>(this);

		mi.menuCaption = MENU_ITEM_NAME_ADD;
		mi.itemId = MENU_ITEM_ID_ADD;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.menuCaption = MENU_ITEM_NAME_REM;
		mi.itemId = MENU_ITEM_ID_REM;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
	}
	{
	// ladujemy plik konfiguracyjny
	wtwMyConfigFile pBuff;
	initStruct(pBuff);

	pBuff.bufferSize = MAX_PATH + 1;
	pBuff.pBuffer = new wchar_t[MAX_PATH + 1];

	wtw->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&pBuff), reinterpret_cast<WTW_PARAM>(hInstance));

	if (wtw->fnCall(WTW_SETTINGS_INIT_EX, reinterpret_cast<WTW_PARAM>(pBuff.pBuffer), reinterpret_cast<WTW_PARAM>(&config)) != S_OK)
	{
		config = 0;
	}
	else
	{
		wtw->fnCall(WTW_SETTINGS_READ, (WTW_PARAM)config, NULL);
	}

	delete[] pBuff.pBuffer;
}

	load();

	{
		// zakladamy hook'i na odpowiednie zdarzenia
		menuRebuildHook = wtw->evHook(WTW_EVENT_MENU_REBUILD, Manager::MenuRebuild, (void*)this);
		presenceHook = wtw->evHook(WTW_ON_PROTOCOL_EVENT, Manager::PresenceListener, (void*)this);
	}
}

Manager::~Manager()
{
	{
		// MUSIMY zdjac hooki ktore zalozylismy
		wtw->evUnhook(menuRebuildHook);
		wtw->evUnhook(presenceHook);
	}
	save();
	{
		// usuwamy pozycje w menu ktore utworzylismy
		wtw->fnCall(WTW_MENU_ITEM_CLEAR, reinterpret_cast<WTW_PARAM>(hInstance), 0);
		// zapisujemy ustawienia
		wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
		// czyscimy pamiec i zamykamy plik ustawien
		wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(hInstance));
	}
}
// metoda wczytuje kontakty jako ciag znakow oddzielony spacja
void Manager::load()
{
	wstring buf;
	{
		wchar_t* tmp = 0;
		wtwGetStr(wtw, config, L"items", L"", &tmp);
		if (tmp != 0) {
			buf = tmp;
			delete[] tmp;
		}
	}

	if (buf.empty())
		return;

	wstring::size_type i = 0;
	wstring::size_type j = 0;
	while ((i = buf.find(L' ', j)) != wstring::npos)
	{
		addItem(buf.substr(j, i - j));
		j = i + 1;
	}
}
// metoda zapisuje wszystkie kontakty (ktorym ukrywamy opis) do jednej zmiennej, oddzielajac je spacja
void Manager::save()
{
	wstring str;
	for (Contacts::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		str += ((*i) + L" ");
	}
	wtwSetStr(wtw, config, L"items", str.c_str());
}

void Manager::addItem(const wstring& id)
{
	Contacts::const_iterator i = std::find(items.begin(), items.end(), id);
	if (i == items.end())
	{
		items.push_back(id);
	}
}

void Manager::remItem(const wstring& id)
{
	Contacts::iterator i = std::find(items.begin(), items.end(), id);
	if (i != items.end())
	{
		items.erase(i);
	}
}

bool Manager::findItem(const wstring& id) const
{
	Contacts::const_iterator i = std::find(items.begin(), items.end(), id);
	return i != items.end();
}

void Manager::onMenuCallback(wtwMenuItemDef* menuItem, wtwMenuPopupInfo* menuInfo)
{
	bool add = wcscmp(menuItem->itemId, MENU_ITEM_ID_ADD) == 0;

	for (int i = 0; i < menuInfo->iContacts; ++i)
	{
		wtwContactDef* cnt = &menuInfo->pContacts[i];

		if (add)
		{
			addItem(cnt->id);
		}
		else
		{
			remItem(cnt->id);
		}
	}
}

void Manager::onMenuRebuild(wtwMenuCallbackEvent* event)
{
	for (int i = 0; i < event->pInfo->iContacts; ++i)
	{
		wtwContactDef* cnt = &event->pInfo->pContacts[i];
		if (findItem(cnt->id))
		{
			event->slInt.add(event->itemsToShow, MENU_ITEM_ID_REM);
		}
		else
		{
			event->slInt.add(event->itemsToShow, MENU_ITEM_ID_ADD);
		}
	}
}

void Manager::onPresence(wtwPresenceDef* presence)
{
	if (findItem(presence->pContactData->id))
	{
		presence->curDescription = emptyString.c_str();
	}
}

// statyczne funkcje, wywoluja metody odpowiadajace za wystepujace zdarzenie
WTW_PTR Manager::MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwMenuItemDef* menuItem = reinterpret_cast<wtwMenuItemDef*>(wParam);
	wtwMenuPopupInfo* menuInfo = reinterpret_cast<wtwMenuPopupInfo*>(lParam);
	reinterpret_cast<Manager*>(ptr)->onMenuCallback(menuItem, menuInfo);
	return 0;
}

WTW_PTR Manager::MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwMenuCallbackEvent* event = reinterpret_cast<wtwMenuCallbackEvent*>(wParam);
	reinterpret_cast<Manager*>(ptr)->onMenuRebuild(event);
	return 0;
}

WTW_PTR Manager::PresenceListener(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr)
{
	wtwProtocolEvent* evn = reinterpret_cast<wtwProtocolEvent*>(lParam);
	if (evn->event == WTW_PEV_PRESENCE_RECV && evn->type == WTW_PEV_TYPE_BEFORE)
	{
		reinterpret_cast<Manager*>(ptr)->onPresence(reinterpret_cast<wtwPresenceDef*>(wParam));
	}
	return 0;
}

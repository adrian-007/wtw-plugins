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
#include "../pluginInfo.h"

WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;
HANDLE onMenuRebuild = 0;
HWND hMainWnd = 0;

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
   L"wtwCopy",
   L"Kopiowanie informacji o użyszkodniku",
	__COPYRIGHTS(2009),
	__AUTHOR_NAME,
	__AUTHOR_CONTACT,
	__AUTHOR_WEBSITE,
	__AUTOUPDATE_URL(L"wtwCopy.xml"),
   PLUGIN_API_VERSION,
   MAKE_QWORD(0, 1, 4, 3),
   WTW_CLASS_UTILITY,
   0,
   L"{35EAB0E4-3F84-4081-AAAA-4976BE125DA8}",
   0,
   0, 0, 0, 0 //zarezerwowane
};

void addToClipboard(const wchar_t* data) {
	if(data != 0 && OpenClipboard(hMainWnd)) {
		EmptyClipboard();

		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, (wcslen(data) + 1) * sizeof(wchar_t));

		wchar_t* cbData = (wchar_t*)GlobalLock(hClipboardData);
		wcscpy(cbData, data);

		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_UNICODETEXT, hClipboardData);
		CloseClipboard();
	}
}

WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;
	wtwMenuItemDef* menu = (wtwMenuItemDef*)wParam;

	if(!nfo || !menu)
		return 0;
	if(nfo->iContacts == 1) {
		wtwContactDef* cnt = &nfo->pContacts[0];

		void* h = 0;
		wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)cnt, (WTW_PARAM)&h);
		if(!h)
			return 0;

		wtwContactListItem item;
		initStruct(item);
		wtw->fnCall(WTW_CTL_CONTACT_GET, (WTW_PARAM)h, (WTW_PARAM)&item);

		wtwExtData xd;
		initStruct(xd);
		xd.contactData.id = cnt->id;
		xd.contactData.netClass = cnt->netClass;
		xd.contactData.netId = cnt->netId;

		xd.fieldBufferLen = 2048;
		xd.retValue = new wchar_t[xd.fieldBufferLen];

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		switch((int)menu->ownerData) {
			case 0: {
				addToClipboard(cnt->id);
				break;
			}
			case 1: {
				addToClipboard(item.itemText);
				break;
			}
			case 2: {
				wtwPresenceDef pd;
				initStruct(pd);
				wtw->fnCall(WTW_CTL_GET_PRESENCE, (WTW_PARAM)&pd, (WTW_PARAM)h);
				addToClipboard(pd.curDescription);
				break;
			}
			case 3: {
				xd.fieldName = WTW_EXTCTC_FIELD_NAME_MAIL;
				wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
				addToClipboard(xd.retValue);
				break;
			}
			case 4: {
				xd.fieldName = WTW_EXTCTC_FIELD_NAME_CELL_PHONE;
				wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
				addToClipboard(xd.retValue);
				break;
			}
			case 5: {
				xd.fieldName = WTW_EXTCTC_FIELD_NAME_STD_PHONE;
				wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
				addToClipboard(xd.retValue);
				break;
			}
			case 6: {
				xd.fieldName = WTW_EXTCTC_FIELD_NAME_ADDRESS;
				wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
				addToClipboard(xd.retValue);
				break;
			}
			case 7: {
				xd.fieldName = WTW_EXTCTC_FIELD_NAME_NAME;
				wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
				addToClipboard(xd.retValue);
				break;
			}
			case 8: {
				xd.fieldName = WTW_EXTCTC_FIELD_NAME_SURNAME;
				wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
				addToClipboard(xd.retValue);
				break;
			}
			case 9: {
				xd.fieldName = WTW_EXTCTC_FIELD_NAME_BIRTH;
				wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
				addToClipboard(xd.retValue);
				break;
			}
			default: { 
				/* w00t?! */
			}
		}
		delete[] xd.retValue;

	}
	return 0;
}

#define MENU_POPUP_ID			L"wtwCopy/root"

#define MENU_ITEM_COPY_ID		L"wtwCopy/ID"
#define MENU_ITEM_COPY_NICK		L"wtwCopy/Nick"
#define MENU_ITEM_COPY_DESC		L"wtwCopy/Description"
#define MENU_ITEM_COPY_CELL		L"wtwCopy/Cell"
#define MENU_ITEM_COPY_MAIL		L"wtwCopy/Mail"
#define MENU_ITEM_COPY_PHONE	L"wtwCopy/Phone"
#define MENU_ITEM_COPY_ADDRESS	L"wtwCopy/Address"
#define MENU_ITEM_COPY_NAME		L"wtwCopy/Name"
#define MENU_ITEM_COPY_SURNAME	L"wtwCopy/Surname"
#define MENU_ITEM_COPY_BIRTHDAY L"wtwCopy/BD"

WTW_PTR MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwMenuCallbackEvent* event = (wtwMenuCallbackEvent*)wParam;

	if(event->pInfo->iContacts == 1) {
		wtwContactDef* cnt = event->pInfo->pContacts;

		void* h = 0;
		wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)cnt, (WTW_PARAM)&h);
		if(!h)
			return 0;

		event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_ID);
		event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_NICK);

		wtwContactListItem item;
		initStruct(item);
		wtw->fnCall(WTW_CTL_CONTACT_GET, (WTW_PARAM)h, (WTW_PARAM)&item);
		{
			wtwPresenceDef pd;
			initStruct(pd);
			//wtw->fnCall(WTW_CTL_GET_PRESENCE, (WTW_PARAM)&pd, (WTW_PARAM)h) == S_OK
			wtw->fnCall(WTW_CTL_GET_PRESENCE, (WTW_PARAM)&pd, (WTW_PARAM)h);
			if(pd.curDescription != 0 && wcslen(pd.curDescription) > 0)
				event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_DESC);
		}

		wtwExtData xd;
		initStruct(xd);
		xd.contactData.id = cnt->id;
		xd.contactData.netClass = cnt->netClass;
		xd.contactData.netId = cnt->netId;

		xd.retValue = new wchar_t[2048];
		xd.fieldBufferLen = 2048;

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		xd.fieldName = WTW_EXTCTC_FIELD_NAME_MAIL;
		wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
		if(wcslen(xd.retValue) > 0)
			event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_MAIL);

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		xd.fieldName = WTW_EXTCTC_FIELD_NAME_STD_PHONE;
		wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
		if(wcslen(xd.retValue) > 0)
			event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_PHONE);

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		xd.fieldName = WTW_EXTCTC_FIELD_NAME_CELL_PHONE;
		wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
		if(wcslen(xd.retValue) > 0)
			event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_CELL);

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		xd.fieldName = WTW_EXTCTC_FIELD_NAME_ADDRESS;
		wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
		if(wcslen(xd.retValue) > 0)
			event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_ADDRESS);

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		xd.fieldName = WTW_EXTCTC_FIELD_NAME_NAME;
		wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
		if(wcslen(xd.retValue) > 0)
			event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_NAME);

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		xd.fieldName = WTW_EXTCTC_FIELD_NAME_SURNAME;
		wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
		if(wcslen(xd.retValue) > 0)
			event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_SURNAME);

		memset(xd.retValue, 0, sizeof(wchar_t) * xd.fieldBufferLen);

		xd.fieldName = WTW_EXTCTC_FIELD_NAME_BIRTH;
		wtw->fnCall(WTW_EXTCTD_GET_FIELD, (WTW_PARAM)&xd, 0);
		if(wcslen(xd.retValue) > 0)
			event->slInt.add(event->itemsToShow, MENU_ITEM_COPY_BIRTHDAY);

		delete[] xd.retValue;
	}
	return 0;
}

extern "C" {
WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f) {
	wtw = f;

	hMainWnd = ::FindWindowW(L"{B993D471-D465-43f2-BBA5-DEEA18A1789E}", NULL);

	if(hMainWnd) {
		wtwMenuItemDef mi;
		initStruct(mi);

		mi.menuID		= WTW_MENU_ID_CONTACT;
		mi.itemId		= MENU_POPUP_ID;
		mi.hModule		= ghInstance;
		mi.menuCaption  = L"Kopiuj...";
		mi.flags		= WTW_MENU_ITEM_FLAG_POPUP;
		mi.callback		= MenuCallback;

		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
	
		mi.flags		= 0;
		mi.menuID		= MENU_POPUP_ID;

		mi.itemId		= MENU_ITEM_COPY_ID;
		mi.menuCaption	= L"Identyfikator";
		mi.ownerData	= (void*)0;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_NICK;
		mi.menuCaption	= L"Nazwa";
		mi.ownerData	= (void*)1;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_DESC;
		mi.menuCaption	= L"Opis";
		mi.ownerData	= (void*)2;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_MAIL;
		mi.menuCaption	= L"Mail";
		mi.ownerData	= (void*)3;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_CELL;
		mi.menuCaption	= L"Telefon kom.";
		mi.ownerData	= (void*)4;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_PHONE;
		mi.menuCaption	= L"Telefon";
		mi.ownerData	= (void*)5;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_ADDRESS;
		mi.menuCaption	= L"Adres";
		mi.ownerData	= (void*)6;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_NAME;
		mi.menuCaption	= L"Imię";
		mi.ownerData	= (void*)7;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_SURNAME;
		mi.menuCaption	= L"Nazwisko";
		mi.ownerData	= (void*)8;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.itemId		= MENU_ITEM_COPY_BIRTHDAY;
		mi.menuCaption	= L"Data urodzin";
		mi.ownerData	= (void*)9;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		mi.callback = 0;
		mi.flags = 0;
		mi.hModule = 0;
		mi.menuCaption = 0;
		mi.itemId = WTW_MIID_CT_COPY_DESC;
		wtw->fnCall(WTW_MENU_ITEM_DELETE, reinterpret_cast<WTW_PARAM>(&mi), 0);

		onMenuRebuild = wtw->evHook(WTW_EVENT_MENU_REBUILD, MenuRebuild, 0);
	} else {
		MessageBoxW(NULL, L"Nie odnaleziono głównego okna programu!", L"wtwCopy", MB_OK);
		onMenuRebuild = 0;
		return 1;
	}
    return 0;
}

int __stdcall pluginUnload(DWORD /*callReason*/) {
	if(onMenuRebuild) {
		wtw->evUnhook(onMenuRebuild);
		onMenuRebuild = 0;
	}
    return 0;
}
} // extern "C"

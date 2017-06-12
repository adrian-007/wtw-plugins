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
#include "SettingsPage.h"

#include <algorithm>

#include "wtwClipboard.hpp"

#include "../utils/WinApiFile.hpp"
#include "../utils/text.hpp"
#include "../utils/wtw.hpp"

wtwClipboard* wtwClipboard::inst = 0;

#define SCI_GETTEXTLENGTH 2183
#define SCI_ADDTEXT 2001

wtwClipboard::wtwClipboard(HINSTANCE hInstance) : wndCreateHook(0) {
	this->hInstance = hInstance;

	wtw_t::preloadIcon(wtw, hInstance, L"wtwClipboard/icon16", L"wtwClipboard16.png", IDB_PNG1);
	wtw_t::preloadIcon(wtw, hInstance, L"wtwClipboard/icon32", L"wtwClipboard32.png", IDB_PNG2);

	wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, filePath);

	filePath += L"wtwClipboard.bin";

	// dodawanie strony do opcji
	{
		wtwOptionPageDef def;
		initStruct(def);

		def.callback	= SettingsPage::callback;
		def.id			= L"wtwClipboard/Options";
		def.caption		= L"Schowek WTW";
		def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;
		def.iconId		= L"wtwClipboard/icon16";

		wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(hInstance), reinterpret_cast<WTW_PARAM>(&def));
	}

	WinApiFile file(filePath, WinApiFile::READ_MODE);
	if(file.valid()) {
		unsigned int len;

		while(true) {
			file >> len;
			if(file.good() == false)
				break;

			std::wstring str;
			str.resize(len);
			file >> str;

			items.push_back(str);
		}
	}
	wndCreateHook = wtw->evHook(WTW_EVENT_ON_CHATWND_CREATE, wtwClipboard::onWndCreate, (void*)this);
}

wtwClipboard::~wtwClipboard() {
	if(wndCreateHook) {
		wtw->evUnhook(wndCreateHook);
		wndCreateHook = 0;
	}

	WinApiFile file(filePath, WinApiFile::WRITE_MODE, CREATE_ALWAYS);
	if(file.valid()) {
		for(WStringList::const_iterator i = items.begin(); i != items.end(); ++i) {
			file << (unsigned int)(*i).length();
			file << *i;
		}
	}

	wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(hInstance), 0);
}

bool wtwClipboard::add(const std::wstring& str) {
	WStringList::iterator i = std::find(items.begin(), items.end(), str);
	if(i == items.end()) {
		items.push_back(str);
		return true;
	}
	return false;
}

bool wtwClipboard::rem(const std::wstring& str) {
	WStringList::iterator i = std::find(items.begin(), items.end(), str);
	if(i != items.end()) {
		items.erase(i);
		return true;
	}
	return false;
}

void wtwClipboard::addButton(wtwContactDef *cnt, wtwChatWindowInfo *nfo) {
	wtwCommandEntry entry;
	initStruct(entry);
	entry.pContactData = cnt;
	entry.callback = onClickCB;
	entry.cbData = (void*)this;
	entry.hInstance = hInstance;
	entry.itemData = (void*)nfo->hWindow;
	entry.itemFlags = 0;
	entry.itemId = L"wtwClipboard/button";
	entry.itemType = CCB_TYPE_STANDARD;
	entry.toolTip = L"Poka� zawarto�� schowka wtwClipboard";
	entry.graphId = L"wtwClipboard/icon16";
	entry.pWnd = nfo->pWnd;

	wtw->fnCall(WTW_CCB_FUNCT_ADD, (WTW_PARAM)&entry, 0);
}

void wtwClipboard::onClick(wtwCommandCallback* c) {
	if(c->action == CCB_ACTION_LCLICK && items.size() > 0) {
		HWND edit = FindWindowEx((HWND)c->itemData, 0, L"A7A0B242-211B-424c-96E0-73D31C1A2436", 0);
		if(edit) {
			CPoint mousePt(c->pt);
			CPoint clientPt(mousePt);
			ScreenToClient((HWND)c->itemData, &clientPt);
			CMenu menu;
			menu.CreatePopupMenu();

			for(size_t i = 0; i < items.size(); ++i) {
				menu.AppendMenu(MF_ENABLED, (UINT_PTR)(i+1), items[i].c_str());
			}

			UINT cmdId = menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, mousePt.x, mousePt.y, (HWND)c->itemData, 0);
			if(cmdId == 0)
				return;

			std::string text = utils::text::fromWide(items[cmdId-1]);
			SendMessage(edit, SCI_ADDTEXT, text.size(), (LPARAM)text.c_str());
		}
	}
}

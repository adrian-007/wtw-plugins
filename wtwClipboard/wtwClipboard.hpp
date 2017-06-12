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

#ifndef WTW_CLIPBOARD
#define WTW_CLIPBOARD

#include <vector>

typedef std::vector<std::wstring> WStringList;

#define PLUGIN_ICON L"wtwCB/icon"

class wtwClipboard {
	static wtwClipboard* inst;
public:
	wtwClipboard(HINSTANCE hInstance);
	~wtwClipboard();

	static void create(wtwClipboard* m) {
		wtwClipboard::inst = m;
	}

	static void destroy() {
		delete wtwClipboard::inst;
		wtwClipboard::inst = 0;
	}

	static wtwClipboard* instance() {
		return wtwClipboard::inst;
	}

	static bool isInstance() {
		return wtwClipboard::inst != 0;
	}

	bool add(const std::wstring& str);
	bool rem(const std::wstring& str);

	const WStringList& getList() const { return items; }

private:
	void addButton(wtwContactDef* cnt, wtwChatWindowInfo* nfo);
	void onClick(wtwCommandCallback* c);

	static WTW_PTR onWndCreate(WTW_PARAM cnt, WTW_PARAM nfo, void* ptr) {
		((wtwClipboard*)ptr)->addButton((wtwContactDef*)cnt, (wtwChatWindowInfo*)nfo);
		return 0;
	}
	static WTW_PTR onClickCB(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr) {
		((wtwClipboard*)ptr)->onClick((wtwCommandCallback*)lParam);
		return 0;
	}

	WStringList items;
	std::wstring filePath;

	HINSTANCE hInstance;
	HANDLE wndCreateHook;
};

#endif

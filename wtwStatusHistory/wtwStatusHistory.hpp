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

#ifndef WTW_STATUS_HISTORY_HPP
#define WTW_STATUS_HISTORY_HPP

class wtwStatusHistoryManager {
public:
	wtwStatusHistoryManager(HINSTANCE hInst);
	~wtwStatusHistoryManager();
	
	static void create(wtwStatusHistoryManager* m) {
		wtwStatusHistoryManager::inst = m;
	}

	static void destroy() {
		delete wtwStatusHistoryManager::inst;
		wtwStatusHistoryManager::inst = 0;
	}

	static wtwStatusHistoryManager& instance() {
		return *wtwStatusHistoryManager::inst;
	}

	static bool isInstance() {
		return wtwStatusHistoryManager::inst != 0;
	}

	void clearList() {
		ctrlList.DeleteAllItems();
	}
private:
	LRESULT subclassFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass);

	static wtwStatusHistoryManager* inst;
	static LRESULT CALLBACK subclassFunc_static(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
		return ((wtwStatusHistoryManager*)dwRefData)->subclassFunc(hWnd, uMsg, wParam, lParam, uIdSubclass);
	}
	static WTW_PTR onDescChange(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
		wtwNewGlobalStatus* gs = (wtwNewGlobalStatus*)wParam;
		((wtwStatusHistoryManager*)ptr)->onDescriptionChange(gs);
		return 0;
	}

	void CreateToolTip(const wchar_t* text);
	void onDescriptionChange(wtwNewGlobalStatus* gs);

	void showUpdatedList();

	HINSTANCE hInstance;
	HANDLE hook;
	wchar_t toolBuffer[4096];

	CListViewCtrl ctrlList;

	HWND hMainWnd;
	HWND hStatusWnd;
	HWND hStatusEditWnd;
	HWND hToolTip;
	HFONT hFont;
	int lastTipItem;

	bool lockList;
};

#endif // WTW_STATUS_HISTORY_HPP

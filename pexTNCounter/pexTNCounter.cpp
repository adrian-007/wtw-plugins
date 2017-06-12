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
#include <algorithm>
#include "pexTNCounter.hpp"
#include "../utils/windows.hpp"

pexTNCounter* pexTNCounter::inst = 0;
unsigned int pexTNCounter::item::subclassIDs = 0;
HFONT pexTNCounter::item::hFont = 0;

pexTNCounter::pexTNCounter(HINSTANCE hInstance) : wndCreateHook(0) {
	this->hInstance = hInstance;

	pexTNCounter::item::hFont = utils::windows::getSystemFont();

	wndCreateHook = wtw->evHook(WTW_EVENT_ON_CHATWND_CREATE, pexTNCounter::onWndCreate, (void*)this);
	wndDestroyHook = wtw->evHook(WTW_EVENT_ON_CHATWND_DESTROY, pexTNCounter::onWndDestroy, (void*)this);
	protoHook = wtw->evHook(WTW_ON_PROTOCOL_EVENT, pexTNCounter::onProtoEvent, (void*)this);
}

pexTNCounter::~pexTNCounter() {
	if(wndCreateHook) {
		wtw->evUnhook(wndCreateHook);
		wndCreateHook = 0;
	}
	if(wndDestroyHook) {
		wtw->evUnhook(wndDestroyHook);
		wndDestroyHook = 0;
	}
	if(protoHook) {
		wtw->evUnhook(protoHook);
		protoHook = 0;
	}

	for(pexTNCounter::item::list::iterator i = items.begin(); i != items.end(); ++i) {
		delete *i;
	}
	items.clear();

	::DeleteObject(pexTNCounter::item::hFont);
	pexTNCounter::item::hFont = 0;
}

pexTNCounter::item::item(HWND parent) {
	this->subclassID = subclassIDs++;
	label.Create(parent, 0, 0, WS_CHILD | SS_RIGHT);
	label.SetFont(pexTNCounter::item::hFont);

	frame.Create(parent, 0, 0, WS_CHILD | SS_BLACKFRAME);

	::SetWindowSubclass(parent, SubclassFunc, subclassID, (DWORD_PTR)this);
}

pexTNCounter::item::~item() {
	::RemoveWindowSubclass(label.GetParent().m_hWnd, SubclassFunc, subclassID);

	label.DestroyWindow();
	frame.DestroyWindow();
}

void pexTNCounter::item::setCount(unsigned int count) {
	if(count > 0) {
		wchar_t buf[512];
		wsprintf(buf, L"%d", count);

		label.SetWindowText(buf);
		label.ShowWindow(SW_SHOW);
		frame.ShowWindow(SW_SHOW);
	} else {
		label.ShowWindow(SW_HIDE);
		frame.ShowWindow(SW_HIDE);
	}
}

LRESULT CALLBACK pexTNCounter::item::SubclassFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	if(uMsg == WM_SIZE) {
		CRect rc;
		::GetClientRect(hWnd, &rc);
		pexTNCounter::item* item = (pexTNCounter::item*)dwRefData;

		int boxWidth = 35;
		int boxHeight = 18;
		int offsetWidth = 25;
		int offsetHeight = 3;

		item->frame.MoveWindow(rc.right - boxWidth - offsetWidth, rc.bottom - boxHeight - offsetHeight, boxWidth, boxHeight);
		item->label.MoveWindow(rc.right - boxWidth - offsetWidth + 1, rc.bottom - boxHeight - offsetHeight + 1, boxWidth - 2, boxHeight - 2);
	}
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void pexTNCounter::onWindowCreated(wtwContactDef *cnt, wtwChatWindowInfo *nfo) {
	if(wcscmp(cnt->netClass, L"GG") != 0) 
		return;

	pexTNCounter::item* i = findItem(cnt->id, cnt->netClass, cnt->netId);
	if(i == 0) {
		HWND parent = nfo->hWindow;
		parent = FindWindowExW(parent, 0, L"Shell Embedding", 0);
		parent = FindWindowExW(parent, 0, L"Shell DocObject View", 0);
		parent = FindWindowExW(parent, 0, L"Internet Explorer_Server", 0);

		i = new pexTNCounter::item(parent);
		i->id = cnt->id;
		i->netClass = cnt->netClass;
		i->netId = cnt->netId;

		items.push_back(i);
	} else {

	}
}

void pexTNCounter::onWindowDestroyed(wtwContactDef *cnt, wtwChatWindowInfo *nfo) {
	if(wcscmp(cnt->netClass, L"GG") != 0) 
		return;

	pexTNCounter::item* i = findItem(cnt->id, cnt->netClass, cnt->netId);

	if(i != 0) {
		std::remove(items.begin(), items.end(), i);
		delete i;
	}
}

void pexTNCounter::onTypingNotify(wtwContactDef* cnt, int count) {
	pexTNCounter::item* i = findItem(cnt->id, cnt->netClass, cnt->netId);
	if(i != 0) {
		i->setCount(count);
	}
}

pexTNCounter::item* pexTNCounter::findItem(const wchar_t* id, const wchar_t* netClass, int netId) {
	for(pexTNCounter::item::list::const_iterator i = items.begin(); i != items.end(); ++i) {
		if((*i)->id == id && (*i)->netClass == netClass && (*i)->netId == netId) {
			return *i;
		}
	}
	return 0;
}

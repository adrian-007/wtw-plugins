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
#include "ArchPage.h"
#include "Logger.hpp"
#include <boost/format.hpp>
#include "SQLite.hpp"
#include "../utils/windows.hpp"
#include "SqlConsoleDialog.h"

#define PANEL_WIDTH 220

ArchPage::ArchPage() {
	hFont = utils::windows::getSystemFont();
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	pDialog = 0;

	worker.parent = this;
	Logger::getInstance()->merge();
}

ArchPage::~ArchPage() {
	DeleteObject(hFont);
	DeleteObject(hBrush);
}

std::wstring ArchPage::getStatusName(int status) const {
	switch(status) {
		case WTW_PRESENCE_AWAY: {
			return L"Zaraz wracam";
			break;
		}
		case WTW_PRESENCE_BLOCK: {
			return L"Zablokowany";
			break;
		}
		case WTW_PRESENCE_CHAT: {
			return L"Porozmawiajmy";
			break;
		}
		case WTW_PRESENCE_DND: {
			return L"Jestem zaj�ty";
			break;
		}
		case WTW_PRESENCE_ERROR: {
			return L"B��d";
			break;
		}
		case WTW_PRESENCE_INV: {
			return L"Niewidoczny";
			break;
		}
		case WTW_PRESENCE_OFFLINE: {
			return L"Niedost�pny";
			break;
		}
		case WTW_PRESENCE_ONLINE: {
			return L"Dost�pny";
			break;
		}
		case WTW_PRESENCE_XA: {
			return L"Wr�c� p�niej";
			break;
		}
	}
	return L"Nieznany";
}

void ArchPage::Worker::addItem(const wchar_t* id, const wchar_t* name, int status, const wchar_t* desc, const wchar_t* time, const wchar_t* net, int netId) {
	HWND hWnd = parent->m_hWnd;
	if(::IsWindow(hWnd) && ::IsWindow(::GetParent(hWnd))) {
		item* i = new item;
		i->id = id;
		i->name = name;
		i->status = status;
		i->desc = desc;
		i->time = time;
		i->net = net;
		i->netId = netId;

		::PostMessage (parent->m_hWnd, WM_ADD_ITEM, (WPARAM)i, 0);
	}
}

void ArchPage::Worker::reloadItems() {
	sqlite3* db = Logger::getInstance()->getDB();

	std::wstring query;
	//std::wstring* id = 0;

	//std::wstring buf;

	/*if(filter.empty() == false) {
		LVITEM item = { 0 };
		item.pszText = (LPWSTR)filter.c_str();
		item.iSubItem = 1;
		item.state = LVIF_TEXT;
		if(parent->ctrlContact.GetItem(&item)) {
			contacts.push_back(filter);
			parent->ctrlContact.SetCheckState(item.iItem, TRUE);
			filter.clear();
		}
	}*/

	if(contacts.empty()) {
		query = L"select c.contactId as id, c.netClass as netClass, c.name as name, l.status as status, l.description as desc, l.time as time, c.netId as netId from logs l, contacts c where c.id = l.contactID ";
	} else {
		query = L"select c.contactId as id, c.netClass as netClass, c.name as name, l.status as status, l.description as desc, l.time as time, c.netId as netId from logs l, contacts c where c.id = l.contactID and (";
		for(size_t i = 0; i < contacts.size(); ++i) {
			query += L"c.contactId = ? or ";
		}
		query.erase(query.size()-4);
		query += L") ";
	}

	if(descFilter.empty() == false) {
		query += L"and l.description like ? ";
	}

	query += boost::str(
		boost::wformat(L"and time between datetime('%|1$04|-%|2$02|-%|3$02| 00:00:00') and datetime('%|4$04|-%|5$02|-%|6$02| 23:59:59') ") 
		% startTime.wYear 
		% startTime.wMonth 
		% startTime.wDay 
		% endTime.wYear 
		% endTime.wMonth 
		% endTime.wDay
		);
	query += L"order by time desc";	

	if(limit > 0) {
		try {
			query += boost::str(boost::wformat(L" limit %1%") % limit);
		} catch(...) {

		}
	}

	query += L";";

	::PostMessage (parent->m_hWnd, WM_SETUP_WINDOW, 0, contacts.size() == 1);

	try {
		SQLStatement stm;
		stm.create(db, query.c_str());
		int pos = 1;

		if(contacts.empty() == false) {
			for(std::list<std::wstring>::const_iterator i = contacts.begin(); i != contacts.end(); ++i) {
				stm.bind(pos++, i->c_str());
			}
		}
		if(descFilter.empty() == false) {
			stm.bind(pos++, descFilter.c_str());
		}

		const wchar_t* desc = L"";

		while(!stop) {
			int rc = stm.step();
			if(rc == SQLITE_ROW) {
				desc = stm.getString(4);
				if(desc == 0) {
					desc = L"";
				}
				
				addItem(stm.getString(0), stm.getString(2), stm.getInt(3), desc, stm.getString(5), stm.getString(1), stm.getInt(6));
				continue;
			} 
			
			if(rc == SQLITE_DONE) {
				//__LOG(wtw, L"STLG", L"Finished fetching rows from DB");
			}

			break;
		}
	} catch(const SQLException& ex) {
		__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
	}

	contacts.clear();
	::PostMessage (parent->m_hWnd, WM_SETUP_WINDOW, 1, 0);
}

void ArchPage::Worker::reloadContacts() {
	sqlite3* db = Logger::getInstance()->getDB();

	::PostMessage(parent->m_hWnd, WM_SETUP_WINDOW, 2, 0);

	try {
		SQLStatement st;
		st.create(db, L"select contactId, name, netClass, netId from contacts group by id order by lower(name) desc;");

		int rc;
		std::wstring id, name;
		ArchPage::Worker::ProtoMap::const_iterator it;

		while(!stop) {
			rc = st.step();
			if(rc == SQLITE_ROW) {
				id = st.getString(0);
				name = st.getString(1);

				if(name.empty() || id.empty())
					continue;

				item* i = new item;
				i->name = name;
				i->id = id;
				i->status = WTW_PRESENCE_ONLINE;
				i->net = st.getString(2);
				i->netId = st.getInt(3);

				::PostMessage(parent->m_hWnd, WM_ADD_ITEM, (WPARAM)i, TRUE);
				continue;
			}
			break;
		}
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
	}

	::PostMessage(parent->m_hWnd, WM_SETUP_WINDOW, 3, 0);
}

void ArchPage::Worker::deleteItems(bool noDesc /*= false*/) {
	sqlite3* db = Logger::getInstance()->getDB();

	std::wstring query;

	if(contacts.empty()) {
		query = L"delete from logs where 1=1 ";
	} else {
		query = L"delete from logs where logs.contactID in (select contacts.id from contacts where ";
		for(size_t i = 0; i < contacts.size(); ++i) {
			query += L"contactId = ? or ";
		}
		query.erase(query.size()-4);
		query += L") ";
	}

	if(noDesc) {
		query += L"and logs.description is null or length(logs.description) == 0 ";
	} else {
		if(descFilter.empty() == false) {
			query += L"and logs.description like ? ";
		}

		query += boost::str(
			boost::wformat(L"and logs.time between datetime('%|1$04|-%|2$02|-%|3$02| 00:00:00') and datetime('%|4$04|-%|5$02|-%|6$02| 23:59:59')") 
			% startTime.wYear 
			% startTime.wMonth 
			% startTime.wDay 
			% endTime.wYear 
			% endTime.wMonth 
			% endTime.wDay
			);
	}

	query += L";";

	::PostMessage (parent->m_hWnd, WM_SETUP_WINDOW, 0, 0);

	try {
		SQLStatement stm;
		stm.create(db, query.c_str());
		int pos = 1;

		if(contacts.empty() == false) {
			for(std::list<std::wstring>::const_iterator i = contacts.begin(); i != contacts.end(); ++i) {
				stm.bind(pos++, i->c_str());
			}
		}

		if(noDesc == false && descFilter.empty() == false) {
			stm.bind(pos++, descFilter.c_str());
		}

		int rc;
		while(!stop) {
			rc = stm.step();
			if(rc == SQLITE_ROW) {
				continue;
			}
			break;
		}
	} catch(const SQLException& ex) {
		__LOG_F(wtw, 1, L"STLG", L"Failed to delete values from DB: %s", ex.message());
	}

	// usun sieroty
	try {
		SQLStatement stm;
		stm.create(db, L"delete from contacts where id not in (select distinct contactID from logs);");

		int rc;
		while(!stop) {
			rc = stm.step();
			if(rc == SQLITE_ROW) {
				continue;
			}
			break;
		}
	} catch(const SQLException& ex) {
		__LOG_F(wtw, 1, L"STLG", L"Failed to delete 'empty' contacts from DB: %s", ex.message());
	}
	//contacts.clear();
	::PostMessage (parent->m_hWnd, WM_SETUP_WINDOW, 1, 0);
}

void ArchPage::Worker::deleteContacts() {
	if(contacts.empty())
		return;

	sqlite3* db = Logger::getInstance()->getDB();

	std::wstring query;

	query = L"delete from logs where logs.contactID in (select contacts.id from contacts where ";
	for(size_t i = 0; i < contacts.size(); ++i) {
		query += L"contactId = ? or ";
	}
	query.erase(query.size()-4);
	query += L");";

	::PostMessage (parent->m_hWnd, WM_SETUP_WINDOW, 2, 0);

	try {
		SQLStatement stm;
		stm.create(db, query.c_str());
		int pos = 1;

		for(std::list<std::wstring>::const_iterator i = contacts.begin(); i != contacts.end(); ++i) {
			stm.bind(pos++, i->c_str());
		}

		int rc;
		while(!stop) {
			rc = stm.step();
			if(rc == SQLITE_ROW) {
				continue;
			}
			break;
		}
	} catch(const SQLException& ex) {
		__LOG_F(wtw, 1, L"STLG", L"Failed to delete values from DB: %s", ex.message());
	}

	query = L"delete from contacts where (";
	for(size_t i = 0; i < contacts.size(); ++i) {
		query += L"contactId = ? or ";
	}
	query.erase(query.size()-4);
	query += L");";

	try {
		SQLStatement stm;
		stm.create(db, query.c_str());
		int pos = 1;

		for(std::list<std::wstring>::const_iterator i = contacts.begin(); i != contacts.end(); ++i) {
			stm.bind(pos++, i->c_str());
		}

		int rc;
		while(!stop) {
			rc = stm.step();
			if(rc == SQLITE_ROW) {
				continue;
			}
			break;
		}
	} catch(const SQLException& ex) {
		__LOG_F(wtw, 1, L"STLG", L"Failed to delete contacts from DB: %s", ex.message());
	}

	::PostMessage (parent->m_hWnd, WM_SETUP_WINDOW, 3, 0);
	contacts.clear();
}

void ArchPage::Worker::threadMain() {
	while(true) {
		s.wait();
		if(stop)
			break;

		if(tasks & TASK_DELETE_SELECTED)
			deleteContacts();
		if(tasks & TASK_DELETE_DATETIME)
			deleteItems(false);
		if(tasks & TASK_DELETE_NO_DESC)
			deleteItems(true);
		if(tasks & TASK_UPDATE_CONTACTS)
			reloadContacts();
		if(tasks & TASK_UPDATE_LIST)
			reloadItems();

		contacts.clear();
		filter.clear();
		descFilter.clear();
		tasks = 0;
	}
}

LRESULT ArchPage::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	ctrlContact.Create(m_hWnd, rcDefault, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL, WS_EX_CLIENTEDGE);
	ctrlContact.SetExtendedListViewStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP);
	utils::windows::SetExplorerTheme(ctrlContact.m_hWnd);

	ctrlContact.InsertColumn(0, L"Nazwa", LVCFMT_LEFT, PANEL_WIDTH - 25, 0);
	ctrlContact.InsertColumn(1, L"ID", LVCFMT_LEFT, 0, 0);

	ctrlExecute.Create(m_hWnd, rcDefault, L"Wykonaj", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, ID_BUTTON_EXECUTE);

	ctrlList.Create(m_hWnd, rcDefault, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL, WS_EX_CLIENTEDGE);
	ctrlList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP);
	utils::windows::SetExplorerTheme(ctrlList.m_hWnd);

	ctrlList.InsertColumn(0, L"Nazwa", LVCFMT_LEFT, 200, 0);
	ctrlList.InsertColumn(1, L"ID", LVCFMT_LEFT, 130, 0);
	ctrlList.InsertColumn(2, L"Czas", LVCFMT_LEFT, 130, 0);
	ctrlList.InsertColumn(3, L"Opis", LVCFMT_LEFT, 340, 0);

	columnsWidth[0] = 200;
	columnsWidth[1] = 130;

	ctrlList.SetWindowPos(0, PANEL_WIDTH + 5, 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	ctrlContact.SetWindowPos(0, 0, 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	{
		{
			ctrlLimitText.Create(m_hWnd, rcDefault, L"Limit wynik�w:", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			ctrlLimitText.ModifyStyle(0, SS_RIGHT);
			ctrlLimitText.SetFont(hFont);
		}
		{
			int lastLimit = cfgGetInt(L"lastLimit", 100);
			if(lastLimit < 0) {
				lastLimit = 100;
				cfgSetInt(L"lastLimit", lastLimit);
			}
			wchar_t buffer[128] = { 0 };
			wsprintf(buffer, L"%d", lastLimit);

			ctrlLimit.Create(m_hWnd, rcDefault, buffer, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, ID_EDIT_LIMIT);
			ctrlLimit.SetFont(hFont);
			ctrlLimit.ModifyStyle(0, ES_NUMBER);
		}
		{
			ctrlFilterText.Create(m_hWnd, rcDefault, L"Opis zawiera:", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			ctrlFilterText.SetFont(hFont);
		}
		{
			ctrlFilter.Create(m_hWnd, rcDefault, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, ID_EDIT_FILTER);
			ctrlFilter.SetFont(hFont);
		}
		{
			ctrlStartDate.Create(m_hWnd, rcDefault, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN/* | DTS_SHOWNONE*/, WS_EX_CLIENTEDGE);
			ctrlEndDate.Create(m_hWnd, rcDefault, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN/* | DTS_SHOWNONE*/, WS_EX_CLIENTEDGE);

			SYSTEMTIME st = { 0 };
			::GetSystemTime(&st);
			worker.endTime = st;
			addSeconds(st, -3600 * 24 * 30);
			worker.startTime = st;
			ctrlStartDate.SetSystemTime(GDT_VALID, &st);
		}

		{
			ctrlAction.Create(m_hWnd, rcDefault, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBS_DROPDOWNLIST, WS_EX_CLIENTEDGE, ID_COMBO_ACTION);
			ctrlAction.SetFont(hFont);

			ctrlAction.AddString(L"Odfiltruj");
			ctrlAction.AddString(L"Usu� wybrane kontakty");
			ctrlAction.AddString(L"Usu� wpisy pasuj�ce do warunk�w");
			ctrlAction.AddString(L"Usu� wpisy bez opis�w");
			ctrlAction.AddString(L"Kompaktuj baz�");
			ctrlAction.AddString(L"Wy�wietl konsol� SQL");
			ctrlAction.SetCurSel(0);
		}
	}

	utils::windows::SetChildFont(m_hWnd, hFont);

	{
		int cnt = (int)wtw->fnCall(WTW_PROTO_FUNC_ENUM, 0, -1);
		int imgIndex = 0;
		images.Create(16, 16, ILC_COLOR32, 0, 1);

		if(cnt > 0) {
			wtwProtocolInfo* nfo = new wtwProtocolInfo[cnt];
			wtw->fnCall(WTW_PROTO_FUNC_ENUM, (WTW_PARAM)nfo, cnt);

			wtwGraphics graph;
			HBITMAP hBitmap;
			wchar_t buf[1024] = { 0 };
			const wchar_t* iconId = 0;
			const wchar_t* fmt = 0;

			for(int i = 0; i < cnt; ++i) {
				if(nfo[i].flags & WTW_PROTO_FLAG_PSEUDO)
					continue;
				if(nfo[i].netClass == 0)
					continue;

				if(nfo[i].protoIconId == 0 || wcslen(nfo[i].protoIconId) == 0) {
					iconId = nfo[i].netClass;
					fmt = L"%s/Icon/%s";
				} else {
					fmt = L"%s/%s";
					iconId = nfo[i].protoIconId;
				}

				initStruct(graph);
				graph.flags = WTW_GRAPH_FLAG_GENERATE_HBITMAP;

				std::vector<int> index(10);
				index[0] = -1;

				wsprintf(buf, fmt, iconId, L"available");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_ONLINE] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_ONLINE] = -1;
				}

				wsprintf(buf, fmt, iconId, L"unavailable");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_OFFLINE] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_OFFLINE] = -1;
				}

				wsprintf(buf, fmt, iconId, L"invisible");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_INV] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_INV] = -1;
				}

				wsprintf(buf, fmt, iconId, L"blocked");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_BLOCK] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_BLOCK] = -1;
				}

				wsprintf(buf, fmt, iconId, L"unknown");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[0] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[0] = -1;
				}

				wsprintf(buf, fmt, iconId, L"chat");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_CHAT] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_CHAT] = -1;
				}

				wsprintf(buf, fmt, iconId, L"away");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_AWAY] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_AWAY] = -1;
				}

				wsprintf(buf, fmt, iconId, L"xa");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_XA] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_XA] = -1;
				}

				wsprintf(buf, fmt, iconId, L"dnd");
				graph.graphId = buf;
				hBitmap = reinterpret_cast<HBITMAP>(wtw->fnCall(WTW_GRAPH_GET_IMAGE, reinterpret_cast<WTW_PARAM>(&graph), 0));
				if(hBitmap) {
					index[WTW_PRESENCE_DND] = imgIndex++;
					images.Add(hBitmap);
				} else {
					index[WTW_PRESENCE_DND] = -1;
				}

				worker.protocols[nfo[i].netClass][nfo[i].netId] = index;
			}
		}
	}

	ctrlList.SetImageList(images, LVSIL_SMALL);
	ctrlContact.SetImageList(images, LVSIL_SMALL);

	worker.stop = false;
	worker.limit = cfgGetInt(L"lastLimit", 100);

	worker.tasks |= Worker::TASK_UPDATE_CONTACTS | Worker::TASK_UPDATE_LIST;

	worker.runThread();
	worker.s.signal();

	return 0;
}

LRESULT ArchPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	if(pDialog && pDialog->IsWindow()) {
		pDialog->DestroyWindow();
		delete pDialog;
		pDialog = 0;
	}

	worker.shutdown();
	worker.join();

	ctrlContact.DeleteAllItems();
	ctrlList.DeleteAllItems();

	{
		wchar_t buf[128] = { 0 };
		ctrlLimit.GetWindowText(buf, sizeof(buf)-1);
		int limit = _wtoi(buf);
		if(wcslen(buf) <= 0 || limit < 0) {
			limit = 100;
		}
		cfgSetInt(L"lastLimit", limit);
	}

	ctrlContact.DestroyWindow();
	ctrlList.DestroyWindow();
	ctrlLimitText.DestroyWindow();
	ctrlLimit.DestroyWindow();
	ctrlFilterText.DestroyWindow();
	ctrlFilter.DestroyWindow();
	ctrlAction.DestroyWindow();
	ctrlStartDate.DestroyWindow();
	ctrlEndDate.DestroyWindow();

	images.Destroy();
	return 0;
}

LRESULT ArchPage::onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CRect rc;
	CRect rcPanel;

	GetClientRect(&rc);
	rc.top += 10;

	rcPanel = rc;
	rcPanel.right = PANEL_WIDTH;
	rc.left += PANEL_WIDTH + 5;

	ctrlList.SetWindowPos(0, 0, 0, rc.Width() - 10, rc.Height() - 10 - 24, SWP_NOMOVE | SWP_NOZORDER);
	ctrlList.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);

	{
		ctrlLimit.MoveWindow(rc.right - 80, rc.bottom - 28, 70, 22);
		ctrlLimitText.MoveWindow(rc.right - 170, rc.bottom - 25, 80, 22);
	}

	int yOffset = rcPanel.top;
	int height = rcPanel.Height();
	const int minHeight = 115; // wysokosc dolnego panelu

	if(height < minHeight)
		height = minHeight;
	else
		height -= (minHeight + 50);

	ctrlContact.SetWindowPos(0, 0, 0, rcPanel.Width(), height, SWP_NOMOVE | SWP_NOZORDER);
	
	yOffset += height + 10;

	{
		ctrlFilterText.MoveWindow(rcPanel.left, yOffset + 2, rcPanel.Width() - 80, 22);
		yOffset += 24;
		ctrlFilter.MoveWindow(0, yOffset, rcPanel.Width(), 22);
		yOffset += 16;
	}

	yOffset += 24;
	ctrlStartDate.MoveWindow(rcPanel.left, yOffset, rcPanel.Width() / 2 - 5, 22);
	ctrlEndDate.MoveWindow(rcPanel.Width() / 2 + 5, yOffset, rcPanel.Width() / 2 - 5, 22);
	yOffset += 10;

	yOffset += 24;
	ctrlAction.MoveWindow(rcPanel.left, yOffset, rcPanel.Width(), 22);
	yOffset += 28;
	ctrlExecute.MoveWindow(rcPanel.left, yOffset, rcPanel.Width(), 22);

	return 0;
}

LRESULT ArchPage::onMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	CPoint mousePt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	CPoint clientPt(mousePt);
	ScreenToClient(&clientPt);

	if(reinterpret_cast<HWND>(wParam) == ctrlList.m_hWnd) {
		if(ctrlList.GetSelectedCount() == 1) {
			CMenu copyMenu;
			copyMenu.CreatePopupMenu();
			copyMenu.AppendMenu(MF_ENABLED, (UINT_PTR)1, L"Nazwa");
			copyMenu.AppendMenu(MF_ENABLED, (UINT_PTR)2, L"Identyfikator");
			copyMenu.AppendMenu(MF_ENABLED, (UINT_PTR)3, L"Czas");
			copyMenu.AppendMenu(MF_ENABLED, (UINT_PTR)4, L"Opis");

			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_POPUP, (UINT)(HMENU)copyMenu, L"Kopiuj...");

			UINT cmdId = menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, mousePt.x, mousePt.y, ctrlList.m_hWnd, 0);
			if(cmdId > 0 && cmdId < 5) {
				wchar_t buf[2048] = { 0 };
				ctrlList.GetItemText(ctrlList.GetSelectedIndex(), cmdId-1, buf, 2048);
				addToClipboard(buf);
			}
		}
	} else if(reinterpret_cast<HWND>(wParam) == ctrlContact.m_hWnd) {
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_ENABLED, (UINT_PTR)1, L"Zaznacz wszystkie kontakty");
		menu.AppendMenu(MF_ENABLED, (UINT_PTR)2, L"Odznacz wszystkie kontakty");
		menu.AppendMenu(MF_ENABLED, (UINT_PTR)3, L"Odwr�� zaznaczenia");
		menu.AppendMenu(MF_ENABLED, (UINT_PTR)4, (ctrlContact.GetColumnWidth(1) == 0 ? L"Poka� kolumn� z ID" : L"Ukryj kolumn� z ID"));

		UINT cmdId = menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, mousePt.x, mousePt.y, ctrlContact.m_hWnd, 0);
		switch(cmdId) {
			case 1:
			case 2:
				{
					BOOL checked = cmdId == 1;
					for(int i = 0; i < ctrlContact.GetItemCount(); ++i) {
						ctrlContact.SetCheckState(i, checked);
					}
					break;
				}
			case 3:
				{
					for(int i = 0; i < ctrlContact.GetItemCount(); ++i) {
						ctrlContact.SetCheckState(i, ctrlContact.GetCheckState(i) ? FALSE : TRUE);
					}
					break;
				}
			case 4:
				{
					if(ctrlContact.GetColumnWidth(1) == 0) {
						ctrlContact.SetColumnWidth(1, 200);
					} else {
						ctrlContact.SetColumnWidth(1, 0);
					}
					break;
				}
			default:
				{
				}
		}
	}

	return 0;
}

LRESULT ArchPage::onExecute(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	Logger::getInstance()->merge();

	switch(ctrlAction.GetCurSel()) {
		case 0: // odfiltruj
			{
				setupFilters();
				worker.tasks |= Worker::TASK_UPDATE_LIST;
				worker.s.signal();
				break;
			}
		case 1: // usun kontakty
			{
				setupFilters();
				worker.tasks |= Worker::TASK_DELETE_SELECTED | Worker::TASK_UPDATE_CONTACTS | Worker::TASK_UPDATE_LIST;
				worker.s.signal();
				break;
			}
		case 2: // usun wpisy pasujace do wzorca
			{
				setupFilters();
				worker.tasks |= Worker::TASK_DELETE_DATETIME | Worker::TASK_UPDATE_CONTACTS | Worker::TASK_UPDATE_LIST;
				worker.s.signal();
				break;
			}
		case 3: // usun wpisy bez opisow
			{
				setupFilters();
				worker.tasks |= Worker::TASK_DELETE_NO_DESC | Worker::TASK_UPDATE_CONTACTS | Worker::TASK_UPDATE_LIST;
				worker.s.signal();
				break;
			}
		case 4: // kompaktuj baze
			{
				if(MessageBox(L"Uwaga!\nTa operacja mo�e zaj�� du�o czasu i na ten czas 'zamrozi�' ca�y program!\nDodatkowo wszystkie sieci MUSZ� by� roz��czone - inaczej prosisz si� o du�y problem!\n\nJe�eli zrozumia�e�/a� te warunki i przy okazji je spe�niasz i je akceptujesz to mo�esz klikn�� OK", L"Kompaktowanie", MB_ICONWARNING | MB_OKCANCEL) == IDOK) {
					sqlite3* db = Logger::getInstance()->getDB();

					try {
						SQLStatement stm;
						stm.create(db, L"vacuum;");
						stm.step();
						MessageBox(L"Operacja zosta�a wykonana pomy�lnie!", L"Kompaktowanie", MB_ICONINFORMATION);
					} catch(const SQLException& ex) {
						std::wstring err = L"Wyst�pi� nieoczekiwany b��d podczas kompaktowania!\nB��d: ";
						err += ex.message();
						MessageBox(err.c_str(), L"Kompaktowanie", MB_ICONERROR);
						__LOG_F(wtw, 1, L"STLG", L"Failed to vacuum DB: %s", ex.message());
					}
				}
				break;
			}
		case 5: // wyswietl konsole sql
			{
				if(pDialog == 0) {
					pDialog = new SqlConsoleDialog;
					pDialog->Create(m_hWnd);
				}
				pDialog->ShowWindow(SW_SHOW);
				break;
			}
	}
	ctrlAction.SetCurSel(0);

	return 0;
}

LRESULT ArchPage::onAddItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	item* i = (item*)wParam;
	if(i == 0) {
		return 0;
	}
	int img = -1;
	int idx;

	Worker::ProtoMap::const_iterator it = worker.protocols.find(i->net);
	if(it != worker.protocols.end()) {
		Worker::IntMatrix::const_iterator it2 = it->second.find(i->netId);
		if(it2 != it->second.end()) {
			img = it2->second[i->status];
		}
		//img = it->second[i->netId][i->status];
	}

	if(lParam) {
		idx = ctrlContact.AddItem(0, 0, i->name.c_str(), img);
		ctrlContact.SetItemText(idx, 1, i->id.c_str());
	} else {
		int img = -1;
		
		Worker::ProtoMap::const_iterator it = worker.protocols.find(i->net);
		if(it != worker.protocols.end()) {
			Worker::IntMatrix::const_iterator it2 = it->second.find(i->netId);
			if(it2 != it->second.end()) {
				img = it2->second[i->status];
			}
			//img = it->second[i->netId][i->status];
		}

		idx = ctrlList.AddItem(ctrlList.GetItemCount(), 0, i->name.c_str(), img);
		ctrlList.SetItemText(idx, 1, i->id.c_str());
		ctrlList.SetItemText(idx, 2, i->time.c_str());
		ctrlList.SetItemText(idx, 3, i->desc.c_str());
	}

	delete i;
	return 0;
}

LRESULT ArchPage::onSetupWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	switch(wParam) {
		case 0:
			{
				ctrlList.EnableWindow(FALSE);
				//ctrlList.SetRedraw(FALSE);
				//if(lParam) {
				//	columnsWidth[1] = ctrlList.GetColumnWidth(1);
				//	ctrlList.SetColumnWidth(1, 0);
				//} else {
				//	ctrlList.SetColumnWidth(1, columnsWidth[1]);
				//}
				ctrlList.DeleteAllItems();
				setControlsState(false);
				break;
			}
		case 1:
			{
				ctrlList.EnableWindow(TRUE);
				//ctrlList.SetRedraw(TRUE);
				//ctrlList.Invalidate();
				//ctrlList.UpdateWindow();
				ctrlList.EnsureVisible(0, TRUE);
				ctrlList.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
				setControlsState(true);
				break;
			}
		case 2:
			{
				ctrlContact.EnableWindow(FALSE);
				ctrlContact.SetRedraw(FALSE);
				ctrlContact.DeleteAllItems();
				setControlsState(false);
				break;
			}
		case 3:
			{
				ctrlContact.EnableWindow(TRUE);
				ctrlContact.SetRedraw(TRUE);
				ctrlContact.Invalidate();
				ctrlContact.UpdateWindow();
				setControlsState(true);
				break;
			}
		default:
			{
			}
	}

	return 0;
}

void ArchPage::setControlsState(bool enabled) {
	BOOL v = enabled ? TRUE : FALSE;
	ctrlList.EnableWindow(v);
	ctrlContact.EnableWindow(v);
	ctrlExecute.EnableWindow(v);
	ctrlLimit.EnableWindow(v);
	ctrlLimitText.EnableWindow(v);
	ctrlFilter.EnableWindow(v);
	ctrlFilterText.EnableWindow(v);
	ctrlAction.EnableWindow(v);
	ctrlStartDate.EnableWindow(v);
	ctrlEndDate.EnableWindow(v);
}

void ArchPage::addSeconds(SYSTEMTIME& st, __int64 seconds) {
	FILETIME ft = { 0 };
	::SystemTimeToFileTime(&st, &ft);
	unsigned __int64 ct = (((unsigned __int64)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	ct += seconds * (__int64)10000000;
	ft.dwLowDateTime  = (DWORD)(ct & 0xFFFFFFFF);
	ft.dwHighDateTime = (DWORD)(ct >> 32);

	::FileTimeToSystemTime(&ft, &st);
}

void ArchPage::setupFilters() {
	{
		wchar_t buf[128] = { 0 };
		ctrlLimit.GetWindowText(buf, sizeof(buf)-1);
		int limit = _wtoi(buf);
		if(wcslen(buf) <= 0 || limit < 0) {
			limit = 100;
		}
		cfgSetInt(L"lastLimit", limit);

		wsprintf(buf, L"%d", limit);
		ctrlLimit.SetWindowText(buf);
		worker.limit = limit;

		ctrlFilter.GetWindowText(buf, sizeof(buf)-1);
		worker.descFilter = buf;
		if(worker.descFilter.length() > 0) {
			worker.descFilter = std::wstring(L"%") + worker.descFilter + std::wstring(L"%");
		}
	}

	std::wstring buf;
	buf.resize(2048);

	for(int i = 0; i < ctrlContact.GetItemCount(); ++i) {
		if(ctrlContact.GetCheckState(i) == TRUE) {
			memset(&buf[0], 0, buf.size() * sizeof(wchar_t));
			ctrlContact.GetItemText(i, 1, &buf[0], buf.size()-1);
			worker.contacts.push_back(buf.c_str());
		}
	}

	{
		SYSTEMTIME cur = { 0 };
		::GetSystemTime(&cur);
		ctrlStartDate.GetSystemTime(&worker.startTime);
		ctrlEndDate.GetSystemTime(&worker.endTime);

		worker.startTime.wHour = worker.endTime.wHour = cur.wHour;
		worker.startTime.wMinute = worker.endTime.wMinute = cur.wMinute;
		worker.startTime.wSecond = worker.endTime.wSecond = cur.wSecond;
	}
}

void ArchPage::addToClipboard(const wchar_t* data) {
	if(OpenClipboard()) {
		EmptyClipboard();

		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, (wcslen(data) + 1) * sizeof(wchar_t));

		wchar_t* cbData;
		cbData = (wchar_t*)GlobalLock(hClipboardData);

		wcscpy(cbData, data);

		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_UNICODETEXT, hClipboardData);
		CloseClipboard();
	}
}

WTW_PTR ArchPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"Statusy");
    wcscpy(info->windowDescrip, L"Historia zmian status�w");
 
	ArchPage* pOptions = (ArchPage*)info->page->ownerData;

    switch(info->action) {
        case WTW_ARCH_PAGE_ACTION_SHOW: {
	        if(!pOptions) {
				pOptions = new ArchPage;
				wtwContactDef* cnt = (wtwContactDef*)lParam;
				if(cnt) {
					pOptions->worker.filter = cnt->id;
				}

				info->page->ownerData = (void*)pOptions;

				CRect rc;
				rc.left = info->x + 5;
				rc.top = info->y + 5;
				rc.right = rc.left + info->cx - 5;
				rc.bottom = rc.top + info->cy - 5;

				pOptions->Create(info->handle, rc, 0, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
			} else {
				pOptions->MoveWindow(info->x + 5, info->y + 5, info->cx - 5, info->cy - 5);
			}
			
			pOptions->ShowWindow(SW_SHOW);
	        break;	 
        }
        case WTW_ARCH_PAGE_ACTION_HIDE: {
			if(pOptions)
				pOptions->ShowWindow(SW_HIDE);
	        break;
        }
		case WTW_ARCH_PAGE_ACTION_MOVE: {
			if(pOptions)
				pOptions->MoveWindow(info->x + 5, info->y + 5, info->cx - 5, info->cy - 5);
			break;
		}
		case WTW_ARCH_PAGE_ACTION_FILTER: {
			wtwContactDef* cnt = (wtwContactDef*)lParam;
			if(lParam) {
				__LOG(wtw, L"filter", L"filter");
				//pOptions->worker.filter = cnt->id;
			} else {
				__LOG(wtw, L"filter", L"show all");
			}
			break;
		}
        case WTW_ARCH_PAGE_ACTION_DESTROY: {
            if(pOptions) {
				pOptions->DestroyWindow();
                info->page->ownerData = 0;
				delete pOptions;
				pOptions = 0;
            }
			break;
        }
    }
	return 0;
}

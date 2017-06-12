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

#ifndef ARCHIVE_PAGE_H
#define ARCHIVE_PAGE_H

#pragma warning(disable:4503)

#define CTRL_CNT_MAP 1
#define CTRL_LIST_MAP 2

#include <string>
#include <list>
#include <unordered_map>

#include "Thread.hpp"

#define ID_BUTTON_EXECUTE 3000
#define ID_EDIT_LIMIT 3002
#define ID_EDIT_FILTER 3003
#define ID_COMBO_ACTION 3004

#define WM_SETUP_WINDOW WM_USER+1
#define WM_ADD_ITEM WM_USER+2

struct item {
	std::wstring id;
	std::wstring name;
	std::wstring desc;
	std::wstring time;
	std::wstring net;
	int netId;
	int status;
};

class SqlConsoleDialog;

class ArchPage : public CWindowImpl<ArchPage> {
public:
	ArchPage();
	~ArchPage();

	BEGIN_MSG_MAP(ArchPage)
		MESSAGE_HANDLER(WM_CREATE, onCreate)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		MESSAGE_HANDLER(WM_SIZE, onSize)
		MESSAGE_HANDLER(WM_SETUP_WINDOW, onSetupWindow)
		MESSAGE_HANDLER(WM_ADD_ITEM, onAddItem)
		COMMAND_ID_HANDLER(ID_BUTTON_EXECUTE, onExecute)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, onColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, onColor)
		MESSAGE_HANDLER(WM_CONTEXTMENU, onMenu)
	END_MSG_MAP()

	static WTW_PTR callback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);

private:
	class Worker : public Thread {
	public:
		typedef std::vector<int> IntArray;
		typedef std::tr1::unordered_map<int, IntArray> IntMatrix;
		typedef std::tr1::unordered_map<std::wstring, IntMatrix> ProtoMap;

		enum {
			TASK_UPDATE_LIST = 0x01,
			TASK_UPDATE_CONTACTS = 0x02,
			TASK_DELETE_SELECTED = 0x04,
			TASK_DELETE_DATETIME = 0x08,
			TASK_DELETE_NO_DESC = 0x10
		};

		Worker() : stop(true), tasks(0) { }

		void addItem(const wchar_t* id, const wchar_t* name, int status, const wchar_t* desc, const wchar_t* time, const wchar_t* net, int netId);
		
		void reloadItems();
		void reloadContacts();
		void deleteContacts();
		void deleteItems(bool noDesc = false);

		void threadMain();

		Semaphore s;
		ArchPage* parent;
		volatile int tasks;
		volatile bool stop;
		volatile int limit;
		std::wstring filter;
		std::wstring descFilter;
		ProtoMap protocols;
		std::list<std::wstring> contacts;
		SYSTEMTIME startTime;
		SYSTEMTIME endTime;

		void shutdown() {
			stop = true;
			s.signal();
		}
	}worker;

	std::wstring getStatusName(int status) const;

	LRESULT onColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) { return (LRESULT)hBrush; }
	LRESULT onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onExecute(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onAddItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onSetupWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	CListViewCtrl ctrlContact;
	CListViewCtrl ctrlList;
	CButton ctrlExecute;
	CStatic ctrlLimitText;
	CEdit ctrlLimit;
	CStatic ctrlFilterText;
	CEdit ctrlFilter;
	CComboBox ctrlAction;
	CDateTimePickerCtrl ctrlStartDate;
	CDateTimePickerCtrl ctrlEndDate;

	void setControlsState(bool enabled);
	void addSeconds(SYSTEMTIME& st, __int64 seconds);
	void setupFilters();
	void addToClipboard(const wchar_t* data);

	int columnsWidth[2];

	CImageList images;
	HFONT hFont;
	HBRUSH hBrush;
	SqlConsoleDialog* pDialog;
};

#endif

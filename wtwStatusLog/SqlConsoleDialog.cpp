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
#include "SqlConsoleDialog.h"
#include "Logger.hpp"
#include "SQLite.hpp"
#include "../utils/windows.hpp"

using std::wstring;

SqlConsoleDialog::SqlConsoleDialog() { 
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hFont = utils::windows::getSystemFont();
}

SqlConsoleDialog::~SqlConsoleDialog() { 
	DeleteObject(hBrush);
	hBrush = 0;
	DeleteObject(hFont);
	hFont = 0;
}

LRESULT SqlConsoleDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	query.Attach(GetDlgItem(IDC_QUERY));
	results.Attach(GetDlgItem(IDC_RESULTS));
	
	results.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP);
	utils::windows::SetExplorerTheme(results.m_hWnd);	

	utils::windows::SetChildFont(m_hWnd, hFont);
	return 0;
}

LRESULT SqlConsoleDialog::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	query.Detach();
	results.Detach();
	return 0;
}

LRESULT SqlConsoleDialog::onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	this->ShowWindow(SW_HIDE);
	return 0;
}

LRESULT SqlConsoleDialog::onExec(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	deleteResults();
	int width;
	{
		CRect rc;
		results.GetClientRect(&rc);
		width = rc.Width() - 20;
	}

	wstring q;
	q.resize(query.GetWindowTextLength() + 1);
	q.resize(query.GetWindowText(&q[0], q.size()));

	if(wcslen(q.c_str()) == 0) {
		setMessage(L"B��d", L"Musisz poda� zapytanie SQL!");
	} else {
		SQLDatabase* db = Logger::getInstance()->getDatabase();

		try {
			SQLStatement st;
			st.create(db->database(), q.c_str());

			int rc = st.step();
			int columns = st.getColumnCount();
			for(int i = 0; i < columns; ++i)
			{
				results.InsertColumn(i, st.getColumnName(i), 0, width / columns);
			}

			while(rc == SQLITE_ROW)
			{
				if(columns > 0) {
					int pos = results.InsertItem(results.GetItemCount(), st.getString(0));

					for(int i = 1; i < columns; ++i)
					{
						results.SetItemText(pos, i, st.getString(i));
					}
				}
				rc = st.step();
			}

			if(columns == 0) {
				wchar_t buf[1024] = { 0 };
				wsprintf(buf, L"Zmieniono %d wierszy", db->changes());
				setMessage(L"Wynik zapytania", buf);
			}
		} catch(const SQLException& ex) {
			setMessage(L"B��d zapytania SQL", ex.message());
		}
	}
	return 0;
}

LRESULT SqlConsoleDialog::onClear(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	deleteResults();
	return 0;
}

void SqlConsoleDialog::deleteResults() {
	results.DeleteAllItems();
	
	while(results.DeleteColumn(0) == TRUE) { }
}

void SqlConsoleDialog::setMessage(const wchar_t* header, const wchar_t* message) {
	CRect rc;
	results.GetClientRect(&rc);

	deleteResults();
	results.InsertColumn(0, header, 0, rc.Width() - 20);
	results.AddItem(0, 0, message);
}

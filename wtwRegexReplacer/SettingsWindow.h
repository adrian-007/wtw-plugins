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

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include "Replacer.hpp"
#include "resource.h"

class SettingsWindow : public CDialogImpl<SettingsWindow> {
public:
	enum { IDD = IDD_PAGE };

	SettingsWindow();
	~SettingsWindow();

	BEGIN_MSG_MAP(SettingsWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, onCreate)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		MESSAGE_HANDLER(WM_SIZE, onSize)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, onColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, onColor)
		NOTIFY_HANDLER(IDC_LIST, NM_DBLCLK, onDbClick)
		COMMAND_ID_HANDLER(IDC_ADD, onAdd)
		COMMAND_ID_HANDLER(IDC_MOD, onModify)
		COMMAND_ID_HANDLER(IDC_DEL, onDelete)
		COMMAND_ID_HANDLER(IDC_UP, onMoveUp)
		COMMAND_ID_HANDLER(IDC_DOWN, onMoveDown)
	END_MSG_MAP()
	
	static WTW_PTR callback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
private:
	void close(bool save) throw();

	LRESULT onColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) { return (LRESULT)hBrush; }
	
	LRESULT onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onAdd(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onModify(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onDelete(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onMoveUp(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onMoveDown(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onDbClick(int /*ctrlId*/, LPNMHDR /*pNMHDR*/, BOOL& /* bHandled */);

	void InsertItem(Replacer::Item* item, int pos = -1);

	CListViewCtrl list;
	CButton btnAdd, btnMod, btnDel, btnUp, btnDown;
	Replacer::ItemList toDelete;

	HBRUSH hBrush;
	HFONT hFont;
};

#endif

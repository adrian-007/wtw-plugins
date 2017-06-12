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

#ifndef ITEM_DIALOG_H
#define ITEM_DIALLG_H

#include "Replacer.hpp"
#include "resource.h"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

class ItemDialog : public CDialogImpl<ItemDialog> {
public:
	enum { IDD = IDD_ITEM };
	ItemDialog() { }
	ItemDialog(const Replacer::Item& i) { item = i; }

	BEGIN_MSG_MAP(ItemDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, onCreate)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		COMMAND_ID_HANDLER(IDOK, onClose)
		COMMAND_ID_HANDLER(IDCANCEL, onClose)
		COMMAND_ID_HANDLER(IDC_TEST, onTest)
		COMMAND_ID_HANDLER(IDC_MODIFY_CONTENT, onModeChange)
		COMMAND_ID_HANDLER(IDC_MODIFY_DISPLAY, onModeChange)
	END_MSG_MAP()

	Replacer::Item item;
private:
	LRESULT onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onClose(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onTest(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onModeChange(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */);

	std::wstring formatError(const char* err) const;

	unsigned short oldDir;
};

#endif

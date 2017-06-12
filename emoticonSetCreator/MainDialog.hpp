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

#ifndef MAIN_DIALOG_HPP
#define MAIN_DIALOG_HPP

#include "resource.h"

class MainDialog : public CDialogImpl<MainDialog> {
public:
	enum { IDD = IDD_MAIN };
	MainDialog() { }

	BEGIN_MSG_MAP(MainDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, onCreateDialog)
		MESSAGE_HANDLER(WM_DESTROY, onDestroyDialog)
		COMMAND_ID_HANDLER(IDOK, onEndDialog)
		COMMAND_ID_HANDLER(IDC_FILE, onFile)
		COMMAND_ID_HANDLER(IDC_GENERATE, onGenerate)
		COMMAND_ID_HANDLER(IDC_ADD, onAdd)
		COMMAND_ID_HANDLER(IDC_REM, onRem)
	END_MSG_MAP()

private:
	LRESULT onCreateDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroyDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onEndDialog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onGenerate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onRem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	std::wstring getText(int id);
	std::string fromWide(const std::wstring& str);

	CListViewCtrl ctrlList;
	CStatic ctrlPath;
	CButton ctrlCase;
};

#endif // MAIN_DIALOG_HPP

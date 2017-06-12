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

#ifndef TEXT_DIALOG
#define TEXT_DIALOG

#include "wtwPastebin.hpp"
#include "resource.h"

class CEditor : public CWindowImpl<CEditor, CEdit>
{
public:
	CEditor();
	virtual ~CEditor();
	HFONT hFont;

private:

	BEGIN_MSG_MAP(CEditor)
		MESSAGE_HANDLER(WM_GETDLGCODE, onGetDlgCode)
	END_MSG_MAP()

	LRESULT onGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = TRUE;
		return DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_WANTTAB;
	}
};

class TextDialog : public CDialogImpl<TextDialog>
{
public:
	enum 
	{ 
		IDD = IDD_TEXT_DIALOG
	};

	std::wstring nick;
	std::wstring id;
	std::wstring netClass;
	int netId;

	TextDialog()
	{ 
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
	}

	virtual ~TextDialog()
	{ 
		DeleteObject(hBrush);
		hBrush = 0;
	}

	BEGIN_MSG_MAP(TextDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, onInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		MESSAGE_HANDLER(WM_CLOSE, onCloseDialog)
		//MESSAGE_HANDLER(WM_CTLCOLORDLG, OnColor)
		//MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColor)
		MESSAGE_HANDLER(WM_SIZE, onSize)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, onMinMaxSize)
		COMMAND_ID_HANDLER(IDOK, onClose)
		COMMAND_ID_HANDLER(IDCANCEL, onClose)
		COMMAND_ID_HANDLER(IDC_LOAD_FILE, onLoadFile)
	END_MSG_MAP()

	LRESULT onInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onLoadFile(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onClose(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onMinMaxSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT onCloseDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	//LRESULT OnColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	//	return (LRESULT)hBrush;
	//}

private:
	void addFormat(CComboBox& c, const wchar_t* name, const wchar_t* format);

	CEditor ctrlEdit;

	HBRUSH hBrush;
	HFONT hFont;
	POINT minWindowSize;
};

#endif

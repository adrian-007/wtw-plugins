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

#ifndef SETTINGS_PAGE
#define SETTINGS_PAGE

#include "resource.h"

class SettingsPage : public CDialogImpl<SettingsPage> {
public:
	enum { IDD = IDD_PAGE };

	SettingsPage() { 
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
	};
	~SettingsPage() { 
		DeleteObject(hBrush);
		hBrush = 0;
		example.Detach();
	}

	BEGIN_MSG_MAP(SettingsPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		COMMAND_HANDLER(IDC_COLORS, CBN_SELCHANGE, onSelChange)
		COMMAND_ID_HANDLER(IDC_PICK_COLOR, onPickColor)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onPickColor(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onSelChange(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);

	LRESULT OnColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		return (LRESULT)hBrush;
	}

	LRESULT OnColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		if((HWND)lParam == example.m_hWnd) {
			CComboBox c;
			c.Attach(GetDlgItem(IDC_COLORS));
			int pos = c.GetCurSel();
			if(pos >= 0) {
				return (LRESULT)CreateSolidBrush(colors[pos]);
			}
		}
		return (LRESULT)hBrush;
	}

	void close(bool save) throw();

	static WTW_PTR callback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
private:
	HBRUSH hBrush;

	enum {
		COLOR_BG = 0,
		COLOR_BG_ACTIVE_UP,
		COLOR_BG_ACTIVE_DOWN,
		COLOR_BG_INACTIVE_UP,
		COLOR_BG_INACTIVE_DOWN,
		COLOR_FONT_ACTIVE,
		COLOR_FONT_INACTIVE,
		COLOR_LAST
	};

	COLORREF colors[COLOR_LAST];
	COLORREF nonStandardColors[16];

	CStatic example;

	//CComboBox ctrlColors;
};

#endif
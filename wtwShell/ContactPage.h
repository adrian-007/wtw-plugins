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

#ifndef CONTACT_PAGE
#define CONTACT_PAGE

#include "wtwShell.hpp"
#include "resource.h"

class Command;

class ContactPage : public CDialogImpl<ContactPage> {
public:
	enum { IDD = IDD_CNT_PAGE };

	ContactPage() : cmd(0) { 
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		NONCLIENTMETRICS ncm = { 0 };
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
		hFont = CreateFontIndirectW(&ncm.lfMessageFont);
	}

	~ContactPage() { 
		DeleteObject(hBrush);
		hBrush = 0;
		DeleteObject(hFont);
		hFont = 0;
	}

	BEGIN_MSG_MAP(ContactPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColor)
	END_MSG_MAP()

	static WTW_PTR callback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
private:
	static BOOL __stdcall enumWndProc(HWND wnd, LPARAM lParam);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		return (LRESULT)hBrush;
	}

	void close(bool save) throw();
	void fillData(Command* c);

	Command* cmd;
	HBRUSH hBrush;
	HFONT hFont;
};

#endif

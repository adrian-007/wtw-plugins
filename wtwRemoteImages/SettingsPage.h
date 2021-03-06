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

#include "wtwRemoteImages.hpp"
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
	}

	BEGIN_MSG_MAP(SettingsPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColor)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		return (LRESULT)hBrush;
	}

	void close(bool save) throw();

	static WTW_PTR callback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
private:

	std::wstring getText(int id) const;
	void setText(int id, LPCWSTR text);

	HBRUSH hBrush;
};

#endif
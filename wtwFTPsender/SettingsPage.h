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

#include "wtwFTPsender.hpp"
#include "resource.h"
#include "../utils/SettingsPageBase.hpp"

class SettingsPage : BASE_INHERIT(SettingsPage) {
public:
	enum { IDD = IDD_PAGE };

	static const wchar_t* windowCaption;
	static const wchar_t* windowDescription;

	BEGIN_MSG_MAP(SettingsPage)
		MESSAGE_BACKGROUND()
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		COMMAND_ID_HANDLER(IDC_FILENAME_FORMAT_HELP, onHelp)
		COMMAND_ID_HANDLER(IDC_MSG_FORMAT_HELP, onHelp)
		CHAIN_MSG_MAP(CScrollImpl<SettingsPage>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onHelp(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */);

	void close(bool save) throw();
private:

	std::wstring getText(int id) const;
	void setText(int id, LPCWSTR text, const wchar_t* def = L"");
};

#endif

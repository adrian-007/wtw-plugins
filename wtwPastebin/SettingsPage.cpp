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

#include "stdinc.hpp"
#include "SettingsPage.h"
#include "wtwPastebin.hpp"

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	setText(IDC_ADDRESS, SETTING_FTP_ADDRESS);
	setText(IDC_USERNAME, SETTING_FTP_USERNAME);
	setText(IDC_PASSWORD, SETTING_FTP_PASSWORD);
	setText(IDC_SERVER_PATH, SETTING_FTP_REMOTE_DIR);
	setText(IDC_SERVER_PATH_URL, SETTING_FTP_REMOTE_DIR_URL);

	SetDlgItemInt(IDC_PORT, cfgGetInt(SETTING_FTP_PORT, 21));

	CButton btn;
	btn.Attach(GetDlgItem(IDC_SHOW_AS_INFO));
	btn.SetCheck(cfgGetInt(SETTING_FTP_SEND_AS_INFO, 0));
	btn.Detach();
	btn.Attach(GetDlgItem(IDC_ADD_TIMESTAMP));
	btn.SetCheck(cfgGetInt(SETTING_FTP_ADD_TIMESTAMP, 1));
	btn.Detach();

	return FALSE;
}

LRESULT SettingsPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	return 0;
}

std::wstring SettingsPage::getText(int id) const {
	std::wstring buf;
	buf.resize(::GetWindowTextLength(GetDlgItem(id))+1);
	memset(&buf[0], 0, buf.size());

	buf.resize(::GetWindowText(GetDlgItem(id), &buf[0], buf.size()));
	return buf;
}

void SettingsPage::setText(int id, LPCWSTR text) {
	std::wstring buf;
	cfgGetStr(buf, text);
	::SetWindowText(GetDlgItem(id), buf.c_str());
}

void SettingsPage::close(bool save) throw() {
	if(save) {
		cfgSetStr(SETTING_FTP_ADDRESS, getText(IDC_ADDRESS).c_str());
		cfgSetStr(SETTING_FTP_USERNAME, getText(IDC_USERNAME).c_str());
		cfgSetStr(SETTING_FTP_PASSWORD, getText(IDC_PASSWORD).c_str());
		cfgSetStr(SETTING_FTP_REMOTE_DIR, getText(IDC_SERVER_PATH).c_str());
		cfgSetStr(SETTING_FTP_REMOTE_DIR_URL, getText(IDC_SERVER_PATH_URL).c_str());

		cfgSetInt(SETTING_FTP_PORT, GetDlgItemInt(IDC_PORT));

		CButton btn;
		btn.Attach(GetDlgItem(IDC_SHOW_AS_INFO));
		cfgSetInt(SETTING_FTP_SEND_AS_INFO, btn.GetCheck());
		btn.Detach();
		btn.Attach(GetDlgItem(IDC_ADD_TIMESTAMP));
		cfgSetInt(SETTING_FTP_ADD_TIMESTAMP, btn.GetCheck());
		btn.Detach();
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"wtwPastebin");
    wcscpy(info->windowDescrip, L"");
 
	SettingsPage* pOptions = (SettingsPage*)info->page->ownerData;

    switch(info->action) {
        case WTW_OPTIONS_PAGE_ACTION_SHOW: {
	        if(!pOptions) {
				pOptions = new SettingsPage;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);
			}
			pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, 1);
			pOptions->ShowWindow(SW_SHOW);
	        break;	 
        }
        case WTW_OPTIONS_PAGE_ACTION_HIDE: {
			if(pOptions)
				pOptions->ShowWindow(SW_HIDE);
	        break;
        }
		case WTW_OPTIONS_PAGE_ACTION_MOVE: {
			if(pOptions)
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, 1);
			break;
		}
        case WTW_OPTIONS_PAGE_ACTION_APPLY: {
			if(pOptions)
				pOptions->close(true);
			break;
        }
		case WTW_OPTIONS_PAGE_ACTION_OK:
        case WTW_OPTIONS_PAGE_ACTION_CANCEL: {
            if(pOptions) {
				pOptions->close(info->action == WTW_OPTIONS_PAGE_ACTION_OK);
				pOptions->DestroyWindow();
                info->page->ownerData = 0;
				delete pOptions;
				pOptions = 0;
            }
			break;
        }
    }
	return 0;
}

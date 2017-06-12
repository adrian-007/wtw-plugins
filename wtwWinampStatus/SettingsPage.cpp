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
#include "SettingsPage.h"
#include "../utils/windows.hpp"

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CEdit edit;
	edit.Attach(GetDlgItem(IDC_FORMAT));
	std::wstring format;
	cfgGetStr(format, L"winamp.format", L"%songTitle% - wtwWinampStatus");
	edit.SetWindowText(format.c_str());

	CButton btn;
	btn.Attach(GetDlgItem(IDC_SET_AT_PAUSE));
	btn.SetCheck(cfgGetInt(L"winamp.setAtPause", 0));

	hFont = utils::windows::getSystemFont();
	utils::windows::SetChildFont(m_hWnd, hFont);
	return 0;
}

void SettingsPage::close(bool save) {
	if(save) {
		CEdit edit;
		edit.Attach(GetDlgItem(IDC_FORMAT));

		std::wstring format;
		format.resize(4096);
		format.resize(edit.GetWindowText(&format[0], format.size()));
		cfgSetStr(L"winamp.format", format.c_str());

		CButton btn;
		btn.Attach(GetDlgItem(IDC_SET_AT_PAUSE));
		cfgSetInt(L"winamp.setAtPause", btn.GetCheck());
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
    wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;
    
	wcscpy(info->windowCaption, L"wtwWinampStatus");
    wcscpy(info->windowDescrip, L"Szczeg�y wkurzania os�b na li�cie aktualnie s�uchanym utworem...");
 
	SettingsPage* pOptions = (SettingsPage*)info->page->ownerData;

    switch(info->action) {
        case WTW_OPTIONS_PAGE_ACTION_SHOW: {
	        if(!pOptions) {
				pOptions = new SettingsPage;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);

				CRect rc;
				pOptions->GetClientRect(&rc);
				pOptions->MoveWindow(info->x, info->y, rc.Width(), rc.Height(), 1);
			}
			pOptions->ShowWindow(SW_SHOW);
	        break;	 
        }
        case WTW_OPTIONS_PAGE_ACTION_HIDE: {
			if(pOptions)
				pOptions->ShowWindow(SW_HIDE);
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
                info->page->ownerData = NULL;
				delete pOptions;
				pOptions = NULL;
            }
			break;
        }
    }
	return 0;
}

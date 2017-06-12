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
#include "Changer.h"

#include "SettingsPage.h"
#include "shellapi.h"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SetDlgItemInt(IDC_TICK, cfgGetInt(L"tick", 120));
	SET_CHECK(IDC_EXCLUDE_HIDDEN, cfgGetInt(L"noHidden", 1));
	return FALSE;
}

LRESULT SettingsPage::onOpenFile(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	HANDLE hFile = CreateFile(changer->getFilePath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}
	::ShellExecute(m_hWnd, NULL, changer->getFilePath().c_str(), 0, 0, SW_SHOWNORMAL);
	return 0;
}

void SettingsPage::close(bool save) {
	if(save) {
		int t = GetDlgItemInt(IDC_TICK);
		if(t < 5)
			t = 5;
		SetDlgItemInt(IDC_TICK, t);
		cfgSetInt(L"tick", t);
		cfgSetInt(L"noHidden", GET_CHECK(IDC_EXCLUDE_HIDDEN));
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
    wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;
    
	wcscpy(info->windowCaption, L"wtwAutoDescription");
    wcscpy(info->windowDescrip, L"Automatyczna zmiana opis�w - z pliku");
 
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

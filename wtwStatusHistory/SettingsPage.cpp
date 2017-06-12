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
#include "wtwStatusHistory.hpp"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SetDlgItemInt(IDC_COUNT, cfgGetInt(L"itemCount", 20));
	return 0;
}

LRESULT SettingsPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {

	return 0;
}

LRESULT SettingsPage::onClear(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	int cnt = cfgGetInt(L"itemCound", 20);
	wchar_t name[256] = { 0 };
	for(int i = 0; i < cnt; ++i) {
		wsprintf(name, L"item.%d", i);
		cfgSetStr(name, L"");
	}
	wtwStatusHistoryManager::instance().clearList();
	return 0;
}

void SettingsPage::close(bool save) throw() {
	if(save) {
		int cnt = GetDlgItemInt(IDC_COUNT);
		if(cnt <= 0) {
			cnt = 20;
		}
		cfgSetInt(L"itemCount", cnt);
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"wtwStatusHistory");
    wcscpy(info->windowDescrip, L"Historia ostatnio u�ywanych status�w");
	info->iconId = L"wtwStatusHistory/icon32";

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
                info->page->ownerData = 0;
				delete pOptions;
				pOptions = 0;
            }
			break;
        }
    }
	return 0;
}

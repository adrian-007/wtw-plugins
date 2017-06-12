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
#include "wtwClipboard.hpp"

#include "../utils/windows.hpp"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

using std::wstring;

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	hFont = utils::windows::getSystemFont();

	list.Attach(GetDlgItem(IDC_LIST));
	line.Attach(GetDlgItem(IDC_LINE));

	utils::windows::SetExplorerTheme(list.m_hWnd);

	const WStringList& lst = wtwClipboard::instance()->getList();
	for(WStringList::const_iterator i = lst.begin(); i != lst.end(); ++i)
	{
		list.AddString((*i).c_str());
	}

	utils::windows::SetChildFont(m_hWnd, hFont);
	return FALSE;
}

LRESULT SettingsPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	list.Detach();
	line.Detach();
	return 0;
}

LRESULT SettingsPage::onAdd(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	wstring buf;
	buf.resize(line.GetWindowTextLength()+1);
	buf.resize(line.GetWindowText(&buf[0], buf.size()));

	if(buf.size() != 0) 
	{
		if(wtwClipboard::instance()->add(buf)) 
		{
			list.AddString(buf.c_str());
			line.SetWindowText(L"");
		}
	}
	return 0;
}

LRESULT SettingsPage::onRemove(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	int index = list.GetCurSel();
	
	if(index != -1) 
	{
		wstring buf;
		buf.resize(list.GetTextLen(index)+1);
		buf.resize(list.GetText(index, &buf[0]));

		if(wtwClipboard::instance()->rem(buf))
			list.DeleteString(index);
	}
	return 0;
}

LRESULT SettingsPage::onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CRect rc;
	GetClientRect(&rc);
	rc.right -= 20;
	rc.bottom -= 20;

	list.MoveWindow(0, 0, rc.Width(), rc.Height() - 140);
	line.MoveWindow(0, rc.Height() - 130, rc.Width(), 90);
	CWindow btn;

	btn = GetDlgItem(IDC_ADD);
	btn.MoveWindow(0, rc.Height() - 24, 70, 24);

	btn = GetDlgItem(IDC_REMOVE);
	btn.MoveWindow(80, rc.Height() - 24, 70, 24);

	return 0;
}

void SettingsPage::close(bool save) throw() {
	if(save)
	{

	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"wtwClipboard");
    wcscpy(info->windowDescrip, L"Schowek WTW - automagicznie wkleja tekst w oknie rozmowy");
	info->iconId = L"wtwClipboard/icon32";

	SettingsPage* pOptions = (SettingsPage*)info->page->ownerData;

    switch(info->action) {
        case WTW_OPTIONS_PAGE_ACTION_SHOW: {
	        if(!pOptions) {
				pOptions = new SettingsPage;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);

				CRect rc;
				pOptions->GetClientRect(&rc);
			}
			pOptions->MoveWindow(info->x, info->y, info->cx, info->cy);
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
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy);
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

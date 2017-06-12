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
#include "TabControl.h"

#include <atldlgs.h>

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SET_CHECK(IDC_USE_SYSTEM_TABS, cfgGetInt(SETTING_USE_SYSTEM_TABS, 1));

	int i = cfgGetInt(SETTING_TAB_POS, 0);
	switch(i) {
		case 1: SET_CHECK(IDC_TAB_DOWN, 1); break;
		case 2: SET_CHECK(IDC_TAB_LEFT, 1); break;
		default: SET_CHECK(IDC_TAB_UP, 1);
	}

	example.Attach(GetDlgItem(IDC_EXAMPLE));

	CComboBox c;
	c.Attach(GetDlgItem(IDC_COLORS));
	c.AddString(L"T�o");
	c.AddString(L"T�o (karta aktywna - g�ra)");
	c.AddString(L"T�o (karta aktywna - d�)");
	c.AddString(L"T�o (karta nieaktywna - g�ra)");
	c.AddString(L"T�o (karta nieaktywna - d�)");
	c.AddString(L"Czcionka (karta aktywna)");
	c.AddString(L"Czcionka (karta nieaktywna)");
	c.SetCurSel(0);
	c.Detach();

	memset(colors, 0, sizeof(COLORREF) * COLOR_LAST);

	colors[COLOR_BG] =					(COLORREF)cfgGetInt(SETTING_COLOR_BG,					DEFAULT_COLOR_BG);
	colors[COLOR_BG_ACTIVE_UP] =		(COLORREF)cfgGetInt(SETTING_COLOR_BG_ACTIVE_UP,			DEFAULT_COLOR_BG_ACTIVE);
	colors[COLOR_BG_ACTIVE_DOWN] =		(COLORREF)cfgGetInt(SETTING_COLOR_BG_ACTIVE_DOWN,		DEFAULT_COLOR_BG_ACTIVE);
	colors[COLOR_BG_INACTIVE_UP] =		(COLORREF)cfgGetInt(SETTING_COLOR_BG_INACTIVE_UP,		DEFAULT_COLOR_BG_INACTIVE);
	colors[COLOR_BG_INACTIVE_DOWN] =	(COLORREF)cfgGetInt(SETTING_COLOR_BG_INACTIVE_DOWN,		DEFAULT_COLOR_BG_INACTIVE);
	colors[COLOR_FONT_ACTIVE] =			(COLORREF)cfgGetInt(SETTING_COLOR_FONT_ACTIVE,			DEFAULT_COLOR_FONT_ACTIVE);
	colors[COLOR_FONT_INACTIVE] =		(COLORREF)cfgGetInt(SETTING_COLOR_FONT_INACTIVE,		DEFAULT_COLOR_FONT_INACTIVE);

	memset(nonStandardColors, RGB(255, 255, 255), sizeof(nonStandardColors));

	return FALSE;
}

LRESULT SettingsPage::onSelChange(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	example.RedrawWindow();
	example.UpdateWindow();
	return 0;
}

void SettingsPage::close(bool save) throw() {
	if(save) {
		std::wstring buf;
		buf.resize(::GetWindowTextLength(GetDlgItem(IDC_DEFAULT_TAB_NAME)) + 1);
		buf.resize(::GetWindowText(GetDlgItem(IDC_DEFAULT_TAB_NAME), &buf[0], buf.size()));

		cfgSetInt(SETTING_USE_SYSTEM_TABS, GET_CHECK(IDC_USE_SYSTEM_TABS));

		int i = TAB_POS_UP;
		if(GET_CHECK(IDC_TAB_DOWN))
			i = TAB_POS_DOWN;
		else if(GET_CHECK(IDC_TAB_LEFT))
			i = TAB_POS_LEFT;

		cfgSetInt(SETTING_TAB_POS, i);

		cfgSetInt(SETTING_COLOR_BG,					colors[COLOR_BG]);
		cfgSetInt(SETTING_COLOR_BG_ACTIVE_UP,		colors[COLOR_BG_ACTIVE_UP]);
		cfgSetInt(SETTING_COLOR_BG_ACTIVE_DOWN,		colors[COLOR_BG_ACTIVE_DOWN]);
		cfgSetInt(SETTING_COLOR_BG_INACTIVE_UP,		colors[COLOR_BG_INACTIVE_UP]);
		cfgSetInt(SETTING_COLOR_BG_INACTIVE_DOWN,	colors[COLOR_BG_INACTIVE_DOWN]);
		cfgSetInt(SETTING_COLOR_FONT_ACTIVE,		colors[COLOR_FONT_ACTIVE]);
		cfgSetInt(SETTING_COLOR_FONT_INACTIVE,		colors[COLOR_FONT_INACTIVE]);

		TabControl::getInstance()->ReloadSettings();
	}
}

LRESULT SettingsPage::onPickColor(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	CComboBox c;
	c.Attach(GetDlgItem(IDC_COLORS));
	int pos = c.GetCurSel();
	c.Detach();

	CHOOSECOLOR cc = { 0 };
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.rgbResult = colors[pos];
	cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_SOLIDCOLOR;
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = nonStandardColors;

	if(::ChooseColor(&cc)) {
		colors[pos] = cc.rgbResult;
	}

	example.RedrawWindow();
	example.UpdateWindow();

	return 0;
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"wtwListTabs");
	wcscpy(info->windowDescrip, L"Grupy jako karty na li�cie kontakt�w");
	info->iconId = L"wtwListTabs/icon32";

	SettingsPage* pOptions = (SettingsPage*)info->page->ownerData;

    switch(info->action) {
        case WTW_OPTIONS_PAGE_ACTION_SHOW: {
	        if(!pOptions) {
				pOptions = new SettingsPage;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);
			}
			pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, TRUE);
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
		case WTW_OPTIONS_PAGE_ACTION_MOVE: {
			if(pOptions) {
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, TRUE);
			}
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

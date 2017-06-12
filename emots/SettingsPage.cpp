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
#include <Commdlg.h>
#include "shellapi.h"
#include "../utils/windows.hpp"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	hFont = utils::windows::getSystemFont();
	utils::windows::SetChildFont(m_hWnd, hFont);

	cfgGetStr(emoSet, L"EMOTXmlFile");
	xmls.Attach(GetDlgItem(IDC_XMLS));

	findXmls();

	SetDlgItemText(IDC_AUTHOR, EmotsManager::instance().getAuthor().c_str());
	SetDlgItemText(IDC_DESC, EmotsManager::instance().getDescription().c_str());
	SetDlgItemInt(IDC_CNT, EmotsManager::instance().getCount());
	SET_CHECK(IDC_IGNORE_SPECIAL, cfgGetInt(L"ignoreSpecial", 0));
	SET_CHECK(IDC_USE_PREVIEW, cfgGetInt(L"usePreview", 0));
	SetDlgItemInt(IDC_PREVIEW_X, cfgGetInt(L"preview_x", 3));
	SetDlgItemInt(IDC_PREVIEW_Y, cfgGetInt(L"preview_y", 2));

	int pos = 0;
	if(!emoSet.empty()) {
		pos = xmls.FindString(1, emoSet.c_str());
		if(pos < 1)
			pos = 0;
	}
	xmls.SetCurSel(pos);
	return FALSE;
}

LRESULT SettingsPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	xmls.Detach();
	return 0;
}

LRESULT SettingsPage::onSelChange(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	std::wstring buf;

	if(xmls.GetCurSel() > 0) {
		buf.resize(xmls.GetLBTextLen(xmls.GetCurSel()));
		xmls.GetLBText(xmls.GetCurSel(), &buf[0]);
	}

	int code = EmotsManager::instance().reload(buf);
	if(code != 0) {
		switch(code) {
			case 1: break; //no file
			case 2: MessageBox(L"B��d podczas wczytywania pliku!", L"Emots", MB_ICONERROR | MB_OK); break;
			case 3: MessageBox(L"B��d podczas parsowania pliku!", L"Emots", MB_ICONERROR | MB_OK); break;
			case 4: MessageBox(L"U�ywasz niekompatybilnego zestawu! Je�eli autor zestawu jeszcze go nie zaktualizowa� - zmu� go!", L"Emots", MB_ICONERROR | MB_OK); break;
		}
		xmls.SetCurSel(0);
	}

	SetDlgItemText(IDC_AUTHOR, EmotsManager::instance().getAuthor().c_str());
	SetDlgItemText(IDC_DESC, EmotsManager::instance().getDescription().c_str());
	SetDlgItemInt(IDC_CNT, EmotsManager::instance().getCount());

	return 0;
}

LRESULT SettingsPage::onRootDir(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	::ShellExecute(m_hWnd, NULL, EmotsManager::instance().getRootDir().c_str(), 0, 0, SW_SHOWNORMAL);
	return 0;
}

void SettingsPage::findXmls() {
	xmls.ResetContent();

	xmls.AddString(L"Brak zestawu - emotikony nieaktywne");

	WIN32_FIND_DATA data;
	HANDLE hFind;

	hFind = FindFirstFile(std::wstring(EmotsManager::instance().getRootDir() + L"*.xml").c_str(), &data);
	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			xmls.AddString(data.cFileName);
		} while(FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

void SettingsPage::close(bool save) throw() {
	bool reload = true;

	std::wstring buf;
	if(xmls.GetCurSel() > 0) {
		buf.resize(xmls.GetLBTextLen(xmls.GetCurSel()));
		xmls.GetLBText(xmls.GetCurSel(), &buf[0]);
		reload = wcscmp(buf.c_str(), emoSet.c_str()) != 0;
	}

	if(save) {
		emoSet = buf;

		cfgSetInt(L"ignoreSpecial", GET_CHECK(IDC_IGNORE_SPECIAL));
		cfgSetInt(L"usePreview", GET_CHECK(IDC_USE_PREVIEW));

		int i = GetDlgItemInt(IDC_PREVIEW_X);
		if(i < 2 || i > 11)
			i = 2;
		cfgSetInt(L"preview_x", i);
		SetDlgItemInt(IDC_PREVIEW_X, i);

		i = GetDlgItemInt(IDC_PREVIEW_Y);
		if(i < 1 || i > 11)
			i = 1;
		cfgSetInt(L"preview_y", i);
		SetDlgItemInt(IDC_PREVIEW_Y, i);
	}

	if(reload) {
		int code = EmotsManager::instance().reload(emoSet);
		if(code != 0) {
			switch(code) {
				case 1: break; //no file
				case 2: MessageBox(L"B��d podczas wczytywania pliku!", L"Emots", MB_ICONERROR | MB_OK); break;
				case 3: MessageBox(L"B��d podczas parsowania pliku!", L"Emots", MB_ICONERROR | MB_OK); break;
				case 4: MessageBox(L"U�ywasz niekompatybilnego zestawu! Je�eli autor zestawu jeszcze go nie zaktualizowa� - zmu� go!", L"Emots", MB_ICONERROR | MB_OK); break;
			}
			xmls.SetCurSel(0);
		}
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"Emotikony");
    wcscpy(info->windowDescrip, L"Ustawienia Emotikon");
	info->iconId = L"emots/icon32";

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

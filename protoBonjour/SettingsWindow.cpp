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
#include "SettingsWindow.h"
#include "protoBonjour.hpp"

#include "../utils/windows.hpp"

#include <IPHlpApi.h>
#pragma comment(lib, "iphlpapi.lib")

SettingsWindow::SettingsWindow() {
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hFont = utils::windows::getSystemFont();
}

SettingsWindow::~SettingsWindow() {
	DeleteObject(hBrush);
	DeleteObject(hFont);
	hBrush = 0;
	hFont = 0;
}

LRESULT SettingsWindow::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	utils::windows::SetChildFont(m_hWnd, hFont);

	SetDlgItemInt(IDC_PORT, cfgGetInt(SETTING_PORT, 8079));
	
	std::wstring buf;

	CComboBox combo;
	combo.Attach(GetDlgItem(IDC_ADDRESS));
	{
		IP_ADAPTER_INFO* AdapterInfo = NULL;
		DWORD dwBufLen = NULL;

		DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
		if(dwStatus == ERROR_BUFFER_OVERFLOW)
		{
			AdapterInfo = (IP_ADAPTER_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufLen);
			dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);		
		}

		if(dwStatus == ERROR_SUCCESS) 
		{
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
			while (pAdapterInfo)
			{
				IP_ADDR_STRING* pIpList = &(pAdapterInfo->IpAddressList);
				while (pIpList) 
				{
					combo.AddString(utils::text::toWide(pIpList->IpAddress.String).c_str());
					pIpList = pIpList->Next;
				}
				pAdapterInfo = pAdapterInfo->Next;
			}
		}
		
		if(AdapterInfo)
		{
			HeapFree(GetProcessHeap(), 0, AdapterInfo);	
		}

		cfgGetStr(buf, SETTING_ADDRESS, L"0.0.0.0");

		combo.SetCurSel(combo.FindString(0, buf.c_str()));
		
		if(combo.GetCurSel() == -1) {
			combo.AddString(buf.c_str());
			combo.SetCurSel(combo.FindString(0, buf.c_str()));
		}
	}
	combo.SetEditSel(0, 0);
	combo.Detach();

	cfgGetStr(buf, SETTING_GROUP, WTW_DEFAULT_CT_GROUP);
	combo.Attach(GetDlgItem(IDC_GROUP));
	wtw_t::api::get()->call(WTW_CTL_ENUM_GROUPS_HWND, combo.m_hWnd);

	int pos = combo.FindString(0, buf.c_str());
	if(pos >= 0)
	{
		combo.SetCurSel(pos);
	}
	combo.SetEditSel(0, 0);
	combo.Detach();

	{
		cfgGetStr(buf, SETTING_NAME);
		SetDlgItemText(IDC_NAME, buf.c_str());

		cfgGetStr(buf, SETTING_SURNAME);
		SetDlgItemText(IDC_SURNAME, buf.c_str());

		cfgGetStr(buf, SETTING_NICK);
		SetDlgItemText(IDC_NICK, buf.c_str());

		cfgGetStr(buf, SETTING_JABBERID);
		SetDlgItemText(IDC_JABBERID, buf.c_str());

		cfgGetStr(buf, SETTING_MAIL);
		SetDlgItemText(IDC_MAIL, buf.c_str());
	}

	SetFocus();

	return 0;
}

LRESULT SettingsWindow::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	return 0;
}

LRESULT SettingsWindow::onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) {
	return 0;
}


void SettingsWindow::close(bool save) throw() {
	if(save) {
		CEdit w;
		std::wstring buf;
		
		w.Attach(GetDlgItem(IDC_ADDRESS));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();
		cfgSetStr(SETTING_ADDRESS, buf.c_str());

		w.Attach(GetDlgItem(IDC_GROUP));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();

		cfgSetStr(SETTING_GROUP, buf.c_str());

		int port = GetDlgItemInt(IDC_PORT);
		if(port <= 0)
		{
			port = (rand() % 50000) + 10000;
		}

		cfgSetInt(SETTING_PORT, port);

		{
			w.Attach(GetDlgItem(IDC_NAME));
			buf.resize(w.GetWindowTextLength() + 1);
			buf.resize(w.GetWindowText(&buf[0], buf.size()));
			w.Detach();
			cfgSetStr(SETTING_NAME, buf.c_str());

			w.Attach(GetDlgItem(IDC_SURNAME));
			buf.resize(w.GetWindowTextLength() + 1);
			buf.resize(w.GetWindowText(&buf[0], buf.size()));
			w.Detach();
			cfgSetStr(SETTING_SURNAME, buf.c_str());

			w.Attach(GetDlgItem(IDC_NICK));
			buf.resize(w.GetWindowTextLength() + 1);
			buf.resize(w.GetWindowText(&buf[0], buf.size()));
			w.Detach();
			cfgSetStr(SETTING_NICK, buf.c_str());

			w.Attach(GetDlgItem(IDC_JABBERID));
			buf.resize(w.GetWindowTextLength() + 1);
			buf.resize(w.GetWindowText(&buf[0], buf.size()));
			w.Detach();
			cfgSetStr(SETTING_JABBERID, buf.c_str());

			w.Attach(GetDlgItem(IDC_MAIL));
			buf.resize(w.GetWindowTextLength() + 1);
			buf.resize(w.GetWindowText(&buf[0], buf.size()));
			w.Detach();
			cfgSetStr(SETTING_MAIL, buf.c_str());

			protoBonjour::getInstance()->updateData();
		}
	}
}

WTW_PTR SettingsWindow::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"Bonjour (LAN)");
    wcscpy(info->windowDescrip, L"Rozmowy w sieci lokalnej");
 
	SettingsWindow* pOptions = (SettingsWindow*)info->page->ownerData;

    switch(info->action) {
        case WTW_OPTIONS_PAGE_ACTION_SHOW: {
	        if(!pOptions) {
				pOptions = new SettingsWindow;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);
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

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

#include "Account.h"

#include "../utils/text.hpp"
#include "../utils/windows.hpp"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

SettingsPage::SettingsPage(Account* acc) : account(acc)
{
	hFont = utils::windows::getSystemFont();
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
}

SettingsPage::~SettingsPage()
{ 
	DeleteObject(hBrush);
	hBrush = 0;
	DeleteObject(hFont);
	hFont = 0;
}

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetDlgItemText(IDC_NAME, account->name.c_str());
	SetDlgItemText(IDC_SERVER, account->server.c_str());
	SetDlgItemInt(IDC_PORT, account->port);
	SetDlgItemText(IDC_NICK, account->nick.c_str());
	SetDlgItemText(IDC_PASS, account->passwd.c_str());
	SetDlgItemText(IDC_USER, account->user.c_str());
	SetDlgItemText(IDC_QUIT_MSG, account->quitMessage.c_str());

	utils::windows::SetChildFont(m_hWnd, hFont);

	return 0;
}

LRESULT SettingsPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void SettingsPage::close(bool save) throw()
{
	if(save)
	{
		std::wstring buf;
		CEdit w;
		w.Attach(GetDlgItem(IDC_NAME));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();

		account->name = buf.c_str();

		w.Attach(GetDlgItem(IDC_SERVER));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();

		account->server = buf.c_str();
		account->port = GetDlgItemInt(IDC_PORT);

		w.Attach(GetDlgItem(IDC_NICK));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();

		account->nick = buf.c_str();

		w.Attach(GetDlgItem(IDC_USER));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();

		account->user = buf.c_str();

		w.Attach(GetDlgItem(IDC_PASS));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();

		account->passwd = buf.c_str();

		w.Attach(GetDlgItem(IDC_QUIT_MSG));
		buf.resize(w.GetWindowTextLength() + 1);
		buf.resize(w.GetWindowText(&buf[0], buf.size()));
		w.Detach();

		account->quitMessage = buf.c_str();

		account->updateInfo();
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*)
{
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"Protok� Internet Relay Chat");
	wcscpy(info->windowDescrip, L"Ustawienia po��czenia z serwerem IRC");

	SettingsPage* pOptions = (SettingsPage*)info->page->ownerData;

	switch(info->action)
	{
	case WTW_OPTIONS_PAGE_ACTION_SHOW:
		{
			if(!pOptions)
			{
				pOptions = new SettingsPage((Account*)info->page->cbData);
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);

				CRect rc;
				pOptions->GetClientRect(&rc);
				pOptions->SetScrollSize(0, rc.Height());
			}
			pOptions->ShowWindow(SW_SHOW);
		}
	case WTW_OPTIONS_PAGE_ACTION_MOVE:
		{
			if(pOptions)
			{
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, 1);
			}
			break;
		}
	case WTW_OPTIONS_PAGE_ACTION_HIDE:
		{
			if(pOptions)
				pOptions->ShowWindow(SW_HIDE);
			break;
		}
	case WTW_OPTIONS_PAGE_ACTION_APPLY:
		{
			if(pOptions)
				pOptions->close(true);
			break;
		}
	case WTW_OPTIONS_PAGE_ACTION_OK:
	case WTW_OPTIONS_PAGE_ACTION_CANCEL:
		{
			if(pOptions)
			{
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

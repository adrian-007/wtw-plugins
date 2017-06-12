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
#include "wtwRemoteImages.hpp"

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CButton btn;
	btn.Attach(GetDlgItem(IDC_SHOW_AS_INFO));
	btn.SetCheck(cfgGetInt(SETTING_REMOTE_IMAGES_SEND_AS_INFO, 0));
	btn.Detach();

	return FALSE;
}

LRESULT SettingsPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void SettingsPage::close(bool save) throw()
{
	if(save)
	{
		CButton btn;
		btn.Attach(GetDlgItem(IDC_SHOW_AS_INFO));
		cfgSetInt(SETTING_REMOTE_IMAGES_SEND_AS_INFO, btn.GetCheck());
		btn.Detach();
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*)
{
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"wtwRemoteImages");
	wcscpy(info->windowDescrip, L"Wysy�anie obrazk�w do serwisu imgur.com");
	info->iconId = L"wtwRemoteImages/icon32";

	SettingsPage* pOptions = (SettingsPage*)info->page->ownerData;

	switch(info->action)
	{
	case WTW_OPTIONS_PAGE_ACTION_SHOW:
		{
			if(!pOptions)
			{
				pOptions = new SettingsPage;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);
			}
			pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, 1);
			pOptions->ShowWindow(SW_SHOW);
			break;	 
		}
	case WTW_OPTIONS_PAGE_ACTION_HIDE:
		{
			if(pOptions)
			{
				pOptions->ShowWindow(SW_HIDE);
			}
			break;
		}
	case WTW_OPTIONS_PAGE_ACTION_MOVE:
		{
			if(pOptions)
			{
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, 1);
			}
			break;
		}
	case WTW_OPTIONS_PAGE_ACTION_APPLY:
		{
			if(pOptions)
			{
				pOptions->close(true);
			}
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

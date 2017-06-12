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
#include "ContactPage.h"
#include "Account.h"
#include "Channel.h"

#include "../utils/windows.hpp"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

LRESULT ContactPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	core->fnCall(WTW_CTL_ENUM_GROUPS_HWND, reinterpret_cast<WTW_PARAM>(GetDlgItem(IDC_CNT_GROUP).m_hWnd), 0);

	hFont = utils::windows::getSystemFont();
	utils::windows::SetChildFont(m_hWnd, hFont);
	return 0;
}

void ContactPage::fillData(Channel* ch) {
	SetDlgItemText(IDC_CNT_ID, ch->channel.c_str());
	SetDlgItemText(IDC_CNT_NAME, ch->name.c_str());
	SetDlgItemText(IDC_CNT_GROUP, ch->group.c_str());
	SetDlgItemText(IDC_CNT_PASS, ch->passwd.c_str());
	SendMessage(GetDlgItem(IDC_CNT_ID), EM_SETREADONLY, TRUE, 0);
	SET_CHECK(IDC_CNT_AUTOJOIN, ch->isSet(Channel::FLAG_AUTOJOIN) ? 1 : 0);
}

void ContactPage::close(bool save) throw() {
	if(save) {
		std::wstring chan, group, name, pass;
		CEdit e;
		e.Attach(GetDlgItem(IDC_CNT_ID));
		chan.resize(e.GetWindowTextLength()+1);
		chan.resize(e.GetWindowText(&chan[0], chan.size()));
		e.Detach();

		e.Attach(GetDlgItem(IDC_CNT_NAME));
		name.resize(e.GetWindowTextLength()+1);
		name.resize(e.GetWindowText(&name[0], name.size()));
		e.Detach();

		e.Attach(GetDlgItem(IDC_CNT_GROUP));
		group.resize(e.GetWindowTextLength()+1);
		group.resize(e.GetWindowText(&group[0], group.size()));
		e.Detach();

		e.Attach(GetDlgItem(IDC_CNT_PASS));
		pass.resize(e.GetWindowTextLength()+1);
		pass.resize(e.GetWindowText(&pass[0], pass.size()));
		e.Detach();

		acc->setChannel(L"", chan.c_str(), group.c_str(), name.c_str(), pass.c_str(), GET_CHECK(IDC_CNT_AUTOJOIN) ? true : false);
	}
}

WTW_PTR ContactPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwPropertiesPageShowInfo* info = (wtwPropertiesPageShowInfo*)wParam;
	ContactPage* pOptions = (ContactPage*)info->page->ownerData;

    switch(info->action) {
        case WTW_PP_CB_EVENT_SHOW: {
			wcscpy(info->wndCaption, L"IRC");
			wcscpy(info->wndDescrip, L"Ustawienia kana�u IRC");
	        if(!pOptions) {
				pOptions = new ContactPage;
				pOptions->acc = (Account*)info->page->cbData;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);

				CRect rc;
				pOptions->GetClientRect(&rc);
				pOptions->MoveWindow(info->x, info->y, rc.Width(), rc.Height(), 1);

				if(info->info->pContactData) {
					Channel* c = pOptions->acc->findChannel(info->info->pContactData->id);
					if(c)
						pOptions->fillData(c);
				}
			}
			pOptions->ShowWindow(SW_SHOW);
	        break;	 
        }
        case WTW_PP_CB_EVENT_HIDE: {
			if(pOptions)
				pOptions->ShowWindow(SW_HIDE);
	        break;
        }
		case WTW_PP_CB_EVENT_OK:
        case WTW_PP_CB_EVENT_APPLY: {
			if(pOptions)
				pOptions->close(true);
			break;
        }
        case WTW_PP_CB_EVENT_DESTROY: {
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

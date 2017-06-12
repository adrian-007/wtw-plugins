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
#include "Command.h"
#include "wtwShell.hpp"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

BOOL ContactPage::enumWndProc(HWND wnd, LPARAM lParam) {
	::SendMessage(wnd, WM_SETFONT, (WPARAM)lParam, 0);
	return 0;
}

LRESULT ContactPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	wtw->fnCall(WTW_CTL_ENUM_GROUPS_HWND, reinterpret_cast<WTW_PARAM>(GetDlgItem(IDC_CNT_GROUP).m_hWnd), 0);

	CComboBox cb;
	int i = 0;
	cb.Attach(GetDlgItem(IDC_CNT_STATUS));
	cb.AddString(L"Dost�pny");
	cb.SetItemData(i++, WTW_PRESENCE_ONLINE);
	cb.AddString(L"Porozmawiajmy");
	cb.SetItemData(i++, WTW_PRESENCE_CHAT);
	cb.AddString(L"Jestem zaj�ty");
	cb.SetItemData(i++, WTW_PRESENCE_DND);
	cb.AddString(L"Zaraz wracam");
	cb.SetItemData(i++, WTW_PRESENCE_AWAY);
	cb.AddString(L"Wr�c� p�niej");
	cb.SetItemData(i++, WTW_PRESENCE_XA);
	cb.AddString(L"Niewidoczny");
	cb.SetItemData(i++, WTW_PRESENCE_INV);
	cb.AddString(L"Niedost�pny");
	cb.SetItemData(i++, WTW_PRESENCE_OFFLINE);

	cb.SetCurSel(i-1);
	cb.Detach();

	cb.Attach(GetDlgItem(IDC_CNT_CMD_TYPE));
	cb.AddString(L"Komenda");
	cb.AddString(L"Aplikacja");
	cb.AddString(L"Folder");
	cb.AddString(L"Link");
	cb.SetCurSel(0);
	cb.Detach();

	EnumChildWindows(m_hWnd, enumWndProc, (LPARAM)hFont);
	return 0;
}

void ContactPage::fillData(Command* c) {
	cmd = c;
	SetDlgItemText(IDC_COMMAND, c->command.c_str());
	SetDlgItemText(IDC_CNT_NAME, c->name.c_str());
	SetDlgItemText(IDC_CNT_GROUP, c->group.c_str());
	SetDlgItemText(IDC_CNT_DESCRIPTION, c->description.c_str());
	SendMessage(GetDlgItem(IDC_CNT_ID), EM_SETREADONLY, TRUE, 0);

	CComboBox cb;
	cb.Attach(GetDlgItem(IDC_CNT_STATUS));
	for(int i = 0; i < cb.GetCount(); ++i) {
		if(static_cast<int>(cb.GetItemData(i)) == c->presence) {
			cb.SetCurSel(i);
			break;
		}
	}
	cb.Detach();

	cb.Attach(GetDlgItem(IDC_CNT_CMD_TYPE));
	cb.SetCurSel(c->type);
	cb.Detach();
}

void ContactPage::close(bool save) throw() {
	if(save) {
		std::wstring group, name, command, description;
		int type, presence;

		CEdit e;
		e.Attach(GetDlgItem(IDC_CNT_NAME));
		name.resize(e.GetWindowTextLength()+1);
		name.resize(e.GetWindowText(&name[0], name.size()));
		e.Detach();

		e.Attach(GetDlgItem(IDC_CNT_GROUP));
		group.resize(e.GetWindowTextLength()+1);
		group.resize(e.GetWindowText(&group[0], group.size()));
		e.Detach();

		e.Attach(GetDlgItem(IDC_COMMAND));
		command.resize(e.GetWindowTextLength()+1);
		command.resize(e.GetWindowText(&command[0], command.size()));
		e.Detach();

		e.Attach(GetDlgItem(IDC_CNT_DESCRIPTION));
		description.resize(e.GetWindowTextLength()+1);
		description.resize(e.GetWindowText(&description[0], description.size()));
		e.Detach();

		CComboBox cb;
		cb.Attach(GetDlgItem(IDC_CNT_STATUS));
		presence = static_cast<int>(cb.GetItemData(cb.GetCurSel()));
		cb.Detach();

		cb.Attach(GetDlgItem(IDC_CNT_CMD_TYPE));
		type = cb.GetCurSel();
		cb.Detach();

		if(cmd == 0) {
			cmd = new Command(L"", name.c_str(), group.c_str(), command.c_str(), type, presence, description);
			wtwShell::instance().addCommand(cmd);
		}

		cmd->update(name.c_str(), group.c_str(), command.c_str(), type, presence, description);
	}
}

WTW_PTR ContactPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwPropertiesPageShowInfo* info = (wtwPropertiesPageShowInfo*)wParam;
	ContactPage* pOptions = (ContactPage*)info->page->ownerData;

    switch(info->action) {
        case WTW_PP_CB_EVENT_SHOW: {
			wcscpy(info->wndCaption, L"Windows Shell");
			wcscpy(info->wndDescrip, L"Ustawienia komendy pow�oki Windows");
	        if(!pOptions) {
				pOptions = new ContactPage;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);

				CRect rc;
				pOptions->GetClientRect(&rc);
				pOptions->MoveWindow(info->x, info->y, rc.Width(), rc.Height(), 1);

				if(info->info->pContactData) {
					Command* c = wtwShell::instance().getCommand(info->info->pContactData->id);
					if(c) {
						pOptions->fillData(c);
					}
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

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
#include "Sounds.hpp"

#include "SettingsPage.h"
#include <commdlg.h>
#include "../utils/windows.hpp"

#define SET_CHECK(ctrl, val) SendMessage(GetDlgItem(ctrl), BM_SETCHECK, val, 0L)
#define GET_CHECK(ctrl) SendMessage(GetDlgItem(ctrl), BM_GETCHECK, 0, 0L) != 0 ? 1 : 0

const wchar_t* SettingsPage::visibleNames[SoundsManager::PRESENCE_LAST] = 
{
	L"Dost�pny",
	L"Porozmawiajmy",
	L"Jestem zaj�ty",
	L"Zaraz wracam",
	L"Wr�c� p�niej",
	L"Niewidoczny",
	L"Niedost�pny",
	L"B��d",
	L"Zablokowany"
};

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) 
{
	hFont = utils::windows::getSystemFont();
	utils::windows::SetChildFont(m_hWnd, hFont);

	{
		cfgGetStr(emoSet, L"sounds.xml.set");
		xmls.Attach(GetDlgItem(IDC_XML_SET));
		ctrlVolume.Attach(GetDlgItem(IDC_VOLUME));

		findXmls();

		int pos = 0;
		if(!emoSet.empty())
		{
			pos = xmls.FindString(1, emoSet.c_str());
			if(pos < 1)
			{
				pos = 0;
			}
		}

		xmls.SetCurSel(pos);
		SetDlgItemText(IDC_AUTHOR, SoundsManager::instance().getAuthor().c_str());
		SetDlgItemText(IDC_DESC, SoundsManager::instance().getDescription().c_str());
		SetDlgItemInt(IDC_BASS_FREQ, cfgGetInt(L"bass.freq", 44100));

		SET_CHECK(IDC_MUTE_ON_AVAILABLE, cfgGetInt(L"sounds.muteOnAvail", 0));
		SET_CHECK(IDC_MUTE_ON_DND, cfgGetInt(L"sounds.muteOnDnd", 0));
		SET_CHECK(IDC_ALWAYS_NOTIFY_ON_MSG, cfgGetInt(L"sounds.alwaysNotifyOnMsg", 1));
		SET_CHECK(IDC_DISABLE_ON_FULLSCREEN, cfgGetInt(L"sounds.disableOnFullscreen", 0));

		ctrlVolume.AddString(L"10 %");
		ctrlVolume.AddString(L"20 %");
		ctrlVolume.AddString(L"30 %");
		ctrlVolume.AddString(L"40 %");
		ctrlVolume.AddString(L"50 %");
		ctrlVolume.AddString(L"60 %");
		ctrlVolume.AddString(L"70 %");
		ctrlVolume.AddString(L"80 %");
		ctrlVolume.AddString(L"90 %");
		ctrlVolume.AddString(L"100 %");

		volume = SoundsManager::instance().getVolume();

		ctrlVolume.SetCurSel(volume);
	}

	{
		cPresenceCombo.Attach(GetDlgItem(IDC_PRESENCE_COMBO));
		CRect rc;

		cList.Attach(GetDlgItem(IDC_LIST));
		cList.GetClientRect(rc);
		cList.InsertColumn(0, _T("Zdarzenia"), LVCFMT_LEFT, (rc.Width() - GetSystemMetrics(SM_CXVSCROLL) - 1), 0);
		cList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
		cList.EnableGroupView(TRUE);
		utils::windows::SetExplorerTheme(cList.m_hWnd);

		cIgnoreToList.Attach(GetDlgItem(IDC_PRESENCE_TO_LIST));
		cIgnoreToList.GetClientRect(rc);
		cIgnoreToList.InsertColumn(0, _T("Status"), LVCFMT_LEFT, (rc.Width() - GetSystemMetrics(SM_CXVSCROLL) - 1), 0);
		cIgnoreToList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
		utils::windows::SetExplorerTheme(cIgnoreToList.m_hWnd);

		LVGROUP gr = { 0 };
		gr.cbSize = sizeof(LVGROUP);
		gr.mask = LVGF_GROUPID | LVGF_HEADER;

		gr.pszHeader = L"Wiadomo�ci";
		gr.cchHeader = wcslen(gr.pszHeader);
		gr.iGroupId = 0;
		cList.AddGroup(&gr);

		gr.pszHeader = L"Zmiany statusu";
		gr.cchHeader = wcslen(gr.pszHeader);
		gr.iGroupId = 1;
		cList.AddGroup(&gr);

		gr.pszHeader = L"Rozmowy Audio/Wideo";
		gr.cchHeader = wcslen(gr.pszHeader);
		gr.iGroupId = 2;
		cList.AddGroup(&gr);

		gr.pszHeader = L"R�ne";
		gr.cchHeader = wcslen(gr.pszHeader);
		gr.iGroupId = 3;
		cList.AddGroup(&gr);

		LVITEM lvItem = { 0 };
		lvItem.mask = LVIF_GROUPID;

		for(int i = 0; i < SoundsManager::SoundEvents::EVENT_LAST; ++i)
		{
			cList.AddItem(i, 0, SoundsManager::SoundEvents::getEventName(i).c_str());
			cList.SetCheckState(i, cfgGetInt(SoundsManager::SoundEvents::getEventSettingName(i).c_str(), 1));

			lvItem.iGroupId = SoundsManager::SoundEvents::getGroupIndex(i);
			if(lvItem.iGroupId < 0)
			{
				__LOG(wtw, L"sounds", L"no group, wtf");
				continue;
			}

			lvItem.iItem = i;
			cList.SetItem(&lvItem);
		}

		wchar_t nameBuf[256] = { 0 };
		for(int i = 0; i < SoundsManager::PRESENCE_LAST; ++i)
		{
			for(int j = 0; j < SoundsManager::PRESENCE_LAST; ++j)
			{
				wsprintf(nameBuf, L"sounds.presence.ignore.%s.%s", SoundsManager::PresenceSettingNames[i], SoundsManager::PresenceSettingNames[j]);
				presenceCache[i][j] = cfgGetInt(nameBuf, 0);
			}

			cPresenceCombo.AddString(visibleNames[i]);
			cIgnoreToList.AddItem(i, 0, visibleNames[i]);
			cIgnoreToList.SetCheckState(i, presenceCache[0][i]);
		}

		lastSelection = 0;
		cPresenceCombo.SetCurSel(lastSelection);
		BOOL tmp;
		onSelChange(0, 0, 0, tmp);

		SET_CHECK(IDC_USE_BEEP, cfgGetInt(L"sounds.use.sysBeep", 0));
		SET_CHECK(IDC_FILTER_MESSAGE, cfgGetInt(L"sounds.filter.message", 0));
		SET_CHECK(IDC_FILTER_STATUS, cfgGetInt(L"sounds.filter.status", 0));
		SET_CHECK(IDC_FILTER_MISC, cfgGetInt(L"sounds.filter.misc", 0));

		SetDlgItemInt(IDC_MESSAGE_DELAY, cfgGetInt(L"sounds.delay.message", 1));
		SetDlgItemInt(IDC_STATUS_DELAY, cfgGetInt(L"sounds.delay.status", 1));
		SetDlgItemInt(IDC_MISC_DELAY, cfgGetInt(L"sounds.delay.misc", 1));
	}
	{
		CRect rc;

		cNetworkList.Attach(GetDlgItem(IDC_NETWORKS));
		cNetworkList.GetClientRect(rc);
		rc.right -= GetSystemMetrics(SM_CXVSCROLL) - 1;

		cNetworkList.InsertColumn(0, _T("Nazwa"), LVCFMT_LEFT, rc.Width() - 60, 0);
		cNetworkList.InsertColumn(1, _T("ID"), LVCFMT_LEFT, 60, 0);
		cNetworkList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
		utils::windows::SetExplorerTheme(cNetworkList.m_hWnd);

		std::wstring nets;
		cfgGetStr(nets, L"sounds.networks");

		int size = (int)wtw->fnCall(WTW_PROTO_FUNC_ENUM, 0, -1);

		if(size > 0)
		{
			wtwProtocolInfo* nfo = new wtwProtocolInfo[size];

			if(wtw->fnCall(WTW_PROTO_FUNC_ENUM, (WTW_PARAM)nfo, (WTW_PARAM)size) != 0)
			{
				for(int i = 0; i < size; ++i)
				{
					if(nfo[i].guid == 0 || wcslen(nfo[i].guid) < 10)
					{
						continue;
					}

					if(nfo[i].flags & WTW_PROTO_FLAG_PSEUDO)
					{
						continue;
					}

					int index = cNetworkList.AddItem(cNetworkList.GetItemCount(), 0, nfo[i].name);
					cNetworkList.SetItemText(index, 1, nfo[i].guid);

					BOOL enabled = nets.find(nfo[i].guid) != std::wstring::npos;
					cNetworkList.SetCheckState(index, enabled);
				}
			}

			delete[] nfo;
			nfo = 0;
		}

		if(nets.length() <= 0) 
		{
			for(int i = 0; i < cNetworkList.GetItemCount(); ++i)
			{
				cNetworkList.SetCheckState(i, TRUE);
			}
		}
	}
	return FALSE;
}

LRESULT SettingsPage::onSelChange(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) 
{
	std::wstring buf;

	if(xmls.GetCurSel() > 0) 
	{
		buf.resize(xmls.GetLBTextLen(xmls.GetCurSel()));
		xmls.GetLBText(xmls.GetCurSel(), &buf[0]);
	}

	try 
	{
		SoundsManager::instance().reload(buf);
	}
	catch(int code)
	{
		switch(code)
		{
		case 1: MessageBox(L"B��d podczas wczytywania pliku!", L"Sounds", MB_ICONERROR | MB_OK); break;
		case 2: MessageBox(L"B��d podczas parsowania pliku!", L"Sounds", MB_ICONERROR | MB_OK); break;
		default: break;
		}
		xmls.SetCurSel(0);
	}

	SetDlgItemText(IDC_AUTHOR, SoundsManager::instance().getAuthor().c_str());
	SetDlgItemText(IDC_DESC, SoundsManager::instance().getDescription().c_str());

	return 0;
}

LRESULT SettingsPage::onPresenceSelChange(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) 
{
	int newSel = cPresenceCombo.GetCurSel();

	for(int i = 0; i < SoundsManager::PRESENCE_LAST; ++i) 
	{
		presenceCache[lastSelection][i] = cIgnoreToList.GetCheckState(i);
		cIgnoreToList.SetCheckState(i, presenceCache[newSel][i]);
	}
	lastSelection = newSel;
	return 0;
}

LRESULT SettingsPage::onVolume(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */) 
{
	SoundsManager::instance().setVolume(ctrlVolume.GetCurSel());
	return FALSE;
}

void SettingsPage::findXmls()
{
	xmls.ResetContent();

	xmls.AddString(L"Brak zestawu - d�wi�ki nieaktywne");

	WIN32_FIND_DATA data;
	HANDLE hFind;

	hFind = FindFirstFile(std::wstring(SoundsManager::instance().getPlayer()->getRootPath() + L"*.xml").c_str(), &data);

	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			xmls.AddString(data.cFileName);
		}
		while(FindNextFile(hFind, &data));

		FindClose(hFind);
	}
}

void SettingsPage::close(bool save)
{
	if(save) 
	{
		emoSet.clear();
		if(xmls.GetCurSel() > 0) 
		{
			emoSet.resize(xmls.GetLBTextLen(xmls.GetCurSel()));
			xmls.GetLBText(xmls.GetCurSel(), &emoSet[0]);
		}
		volume = ctrlVolume.GetCurSel();

		cfgSetInt(L"sounds.muteOnAvail", GET_CHECK(IDC_MUTE_ON_AVAILABLE));
		cfgSetInt(L"sounds.muteOnDnd", GET_CHECK(IDC_MUTE_ON_DND));
		cfgSetInt(L"sounds.alwaysNotifyOnMsg", GET_CHECK(IDC_ALWAYS_NOTIFY_ON_MSG));
		cfgSetInt(L"sounds.disableOnFullscreen", GET_CHECK(IDC_DISABLE_ON_FULLSCREEN));

		for(int i = 0; i < SoundsManager::SoundEvents::EVENT_LAST; ++i)
		{
			cfgSetInt(SoundsManager::SoundEvents::getEventSettingName(i).c_str(), cList.GetCheckState(i));
		}

		cfgSetInt(L"sounds.use.sysBeep", GET_CHECK(IDC_USE_BEEP));

		cfgSetInt(L"sounds.filter.message", GET_CHECK(IDC_FILTER_MESSAGE));
		cfgSetInt(L"sounds.filter.status", GET_CHECK(IDC_FILTER_STATUS));
		cfgSetInt(L"sounds.filter.misc", GET_CHECK(IDC_FILTER_MISC));

		cfgSetInt(L"sounds.delay.message", GetDlgItemInt(IDC_MESSAGE_DELAY));
		cfgSetInt(L"sounds.delay.status", GetDlgItemInt(IDC_STATUS_DELAY));
		cfgSetInt(L"sounds.delay.misc", GetDlgItemInt(IDC_MISC_DELAY));

		lastSelection = cPresenceCombo.GetCurSel();
		BOOL tmp;
		onSelChange(0, 0, 0, tmp);
		onPresenceSelChange(0, 0, 0, tmp);

		wchar_t nameBuf[256] = { 0 };
		for(int i = 0; i < SoundsManager::PRESENCE_LAST; ++i)
		{
			for(int j = 0; j < SoundsManager::PRESENCE_LAST; ++j)
			{
				wsprintf(nameBuf, L"sounds.presence.ignore.%s.%s", SoundsManager::PresenceSettingNames[i], SoundsManager::PresenceSettingNames[j]);
				cfgSetInt(nameBuf, presenceCache[i][j]);
			}
		}

		std::wstring nets;
		for(int i = 0; i < cNetworkList.GetItemCount(); ++i)
		{
			if(cNetworkList.GetCheckState(i)) 
			{
				cNetworkList.GetItemText(i, 1, nameBuf, sizeof(nameBuf) - 1);
				nets += nameBuf;
				nets += L" ";
			}
		}
		cfgSetStr(L"sounds.networks", nets.c_str());
		//__LOG(wtw, L"snd", nets.c_str());
	}

	SoundsManager::instance().setVolume(volume);

	try 
	{
		SoundsManager::instance().reload(emoSet);
	} 
	catch(int code)
	{
		switch(code) 
		{
		case 1: ::MessageBox(m_hWnd, L"B��d podczas wczytywania pliku!", L"Sounds", MB_ICONERROR | MB_OK); break;
		case 2: ::MessageBox(m_hWnd, L"B��d podczas parsowania pliku!", L"Sounds", MB_ICONERROR | MB_OK); break;
		default: break;
		}
	}
}

WTW_PTR SettingsPage::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*)
{
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"Sounds");
	wcscpy(info->windowDescrip, L"Ustawienia d�wi�k�w");

	SettingsPage* pOptions = (SettingsPage*)(WTW_PARAM)info->page->ownerData;

	switch(info->action) 
	{
	case WTW_OPTIONS_PAGE_ACTION_SHOW: 
		{
			if(!pOptions)
			{
				pOptions = new SettingsPage;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);

				CRect rc;
				pOptions->GetClientRect(&rc);
				pOptions->SetScrollSize(rc.Width(), rc.Height());
			}
			pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, TRUE);
			pOptions->ShowWindow(SW_SHOW);
			break;	 
		}
	case WTW_OPTIONS_PAGE_ACTION_MOVE:
		{
			if(pOptions)
			{
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, TRUE);
			}
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

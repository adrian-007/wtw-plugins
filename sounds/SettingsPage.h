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

#ifndef SETTINGS_PAGE
#define SETTINGS_PAGE

#include "resource.h"

class SettingsPage : public CDialogImpl<SettingsPage>, public CScrollImpl<SettingsPage> {
public:
	enum 
	{ 
		IDD = IDD_PAGE
	};

	SettingsPage()
	{ 
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
	}

	~SettingsPage()
	{ 
		DeleteObject(hBrush);
		DeleteObject(hFont);
	}

	BEGIN_MSG_MAP(SettingsPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, onClose)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColor)
		COMMAND_HANDLER(IDC_XML_SET, CBN_SELCHANGE, onSelChange)
		COMMAND_HANDLER(IDC_PRESENCE_COMBO, CBN_SELCHANGE, onPresenceSelChange)
		COMMAND_HANDLER(IDC_VOLUME, CBN_SELCHANGE, onVolume)
		CHAIN_MSG_MAP(CScrollImpl<SettingsPage>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onSelChange(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onPresenceSelChange(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */);
	LRESULT onVolume(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */);

	LRESULT OnColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		return (LRESULT)hBrush;
	}

	LRESULT onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) 
	{
		xmls.Detach();
		ctrlVolume.Detach();
		return 0;
	}

	void close(bool save);

	static WTW_PTR callback(WTW_PARAM wParam, WTW_PARAM lParam, void*);

	static const wchar_t* visibleNames[SoundsManager::PRESENCE_LAST];

private:
	int presenceCache[SoundsManager::PRESENCE_LAST][SoundsManager::PRESENCE_LAST];

	void findXmls();
	std::wstring emoSet;
	int volume;

	HBRUSH hBrush;
	HFONT hFont;
	int lastSelection;

	CComboBox xmls;
	CComboBox ctrlVolume;
	CListViewCtrl cList;
	CComboBox cPresenceCombo;
	CListViewCtrl cIgnoreToList;
	CListViewCtrl cNetworkList;
};

#endif

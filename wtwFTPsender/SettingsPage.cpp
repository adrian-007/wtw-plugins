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
#include "wtwFTPsender.hpp"

const wchar_t* SettingsPage::windowCaption = L"wtwFTPsender";
const wchar_t* SettingsPage::windowDescription = L"Klient FTP wstawiaj�cy linki w oknie chata";

LRESULT SettingsPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	setText(IDC_ADDRESS, SETTING_FTP_ADDRESS);
	setText(IDC_USERNAME, SETTING_FTP_USERNAME);
	setText(IDC_PASSWORD, SETTING_FTP_PASSWORD);
	setText(IDC_SERVER_PATH, SETTING_FTP_REMOTE_DIR);
	setText(IDC_SERVER_PATH_URL, SETTING_FTP_REMOTE_DIR_URL);
	setText(IDC_MSG_FORMAT, SETTING_FTP_MSG_FORMAT, L"Link do pliku: #url#");
	setText(IDC_FILENAME_FORMAT, SETTING_FTP_FILENAME_FORMAT, L"%d.%m.%y_%H.%M.%S_#filename#");

	SetDlgItemInt(IDC_PORT, cfgGetInt(SETTING_FTP_PORT, 21));
	SetDlgItemInt(IDC_TIMEOUT, cfgGetInt(SETTING_FTP_TIMEOUT, 30));

	CButton btn;
	btn.Attach(GetDlgItem(IDC_SHOW_AS_INFO));
	btn.SetCheck(cfgGetInt(SETTING_FTP_SEND_AS_INFO, 0));
	btn.Detach();
	btn.Attach(GetDlgItem(IDC_PASSIVE));
	btn.SetCheck(cfgGetInt(SETTING_FTP_PASSIVE, 1));
	btn.Detach();
	btn.Attach(GetDlgItem(IDC_SHOW_BUTTON));
	btn.SetCheck(cfgGetInt(SETTING_FTP_SHOW_BUTTON, 1));
	btn.Detach();
	btn.Attach(GetDlgItem(IDC_SHOW_NOTIFY));
	btn.SetCheck(cfgGetInt(SETTING_FTP_SHOW_NOTIFY, 1));
	btn.Detach();

	this->ApplyFont();

	return 0;
}

LRESULT SettingsPage::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	return 0;
}

LRESULT SettingsPage::onHelp(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	std::wstring help;
	if(wID == IDC_FILENAME_FORMAT_HELP) {
		help = L"Mo�emy tu u�ywa� zmiennych do okre�lenia czasu (standardowo dodany jest znacznik czasowy).\n" \
			L"#filename# okre�la nazw� pliku znajduj�cego si� na komputerze.\n" \
			L"Zabronione jest u�ywanie znaku /";

	} else if(wID == IDC_MSG_FORMAT_HELP) {
		help = L"Mo�emy okre�li� format wiadomo�ci kt�ra zostanie wys�ana do kontaktu.\n" \
			L"Dost�pna jest jedna zmienna - #url# okre�laj�ca link do pliku wys�anego na serwer";
	} else {
		return 0;
	}

	MessageBox(help.c_str(), L"Pomoc", MB_ICONINFORMATION);
	return 0;
}

std::wstring SettingsPage::getText(int id) const {
	std::wstring buf;
	buf.resize(::GetWindowTextLength(GetDlgItem(id))+1);
	memset(&buf[0], 0, buf.size());

	buf.resize(::GetWindowText(GetDlgItem(id), &buf[0], buf.size()));
	return buf;
}

void SettingsPage::setText(int id, LPCWSTR text, const wchar_t* def /*= L""*/) {
	std::wstring buf;
	cfgGetStr(buf, text, def);
	::SetWindowText(GetDlgItem(id), buf.c_str());
}

void SettingsPage::close(bool save) throw() {
	if(save) {
		cfgSetStr(SETTING_FTP_ADDRESS, getText(IDC_ADDRESS).c_str());
		cfgSetStr(SETTING_FTP_USERNAME, getText(IDC_USERNAME).c_str());
		cfgSetStr(SETTING_FTP_PASSWORD, getText(IDC_PASSWORD).c_str());
		cfgSetStr(SETTING_FTP_REMOTE_DIR, getText(IDC_SERVER_PATH).c_str());
		cfgSetStr(SETTING_FTP_REMOTE_DIR_URL, getText(IDC_SERVER_PATH_URL).c_str());
		{
			std::wstring fnFormat = getText(IDC_FILENAME_FORMAT);
			if(fnFormat.find(L"#filename#") == std::wstring::npos) {
				fnFormat += L"#filename#";
			}

			std::wstring::size_type i = 0;
			while((i = fnFormat.find(L"/")) != std::wstring::npos) {
				fnFormat.erase(i, 1);
			}

			cfgSetStr(SETTING_FTP_FILENAME_FORMAT, fnFormat.c_str());
		}

		{
			std::wstring msgFormat = getText(IDC_MSG_FORMAT);
			if(msgFormat.find(L"#url#") == std::wstring::npos) {
				msgFormat = L"#url#";
			}

			cfgSetStr(SETTING_FTP_MSG_FORMAT, msgFormat.c_str());

		}
		cfgSetInt(SETTING_FTP_PORT, GetDlgItemInt(IDC_PORT));
		int timeout = GetDlgItemInt(IDC_TIMEOUT);
		if(timeout < 10)
			timeout = 10;
		cfgSetInt(SETTING_FTP_TIMEOUT, timeout);

		CButton btn;
		btn.Attach(GetDlgItem(IDC_SHOW_AS_INFO));
		cfgSetInt(SETTING_FTP_SEND_AS_INFO, btn.GetCheck());
		btn.Detach();
		btn.Attach(GetDlgItem(IDC_PASSIVE));
		cfgSetInt(SETTING_FTP_PASSIVE, btn.GetCheck());
		btn.Detach();
		btn.Attach(GetDlgItem(IDC_SHOW_BUTTON));
		cfgSetInt(SETTING_FTP_SHOW_BUTTON, btn.GetCheck());
		btn.Detach();
		btn.Attach(GetDlgItem(IDC_SHOW_NOTIFY));
		cfgSetInt(SETTING_FTP_SHOW_NOTIFY, btn.GetCheck());
		btn.Detach();

		wtwFTPsender::instance()->updateServerInfo();
	}
}

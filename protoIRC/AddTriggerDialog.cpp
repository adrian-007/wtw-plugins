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
#include "AddTriggerDialog.hpp"

#include <regex>

#include "../utils/text.hpp"
#include "../utils/windows.hpp"

AddTriggerDialog::AddTriggerDialog(HFONT font, HBRUSH brush) : hFont(font), hBrush(brush), stopParsing(false)
{
}

AddTriggerDialog::~AddTriggerDialog(void)
{
}

LRESULT AddTriggerDialog::onInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	utils::windows::SetChildFont(m_hWnd, hFont);
	return 0;
}

LRESULT AddTriggerDialog::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT AddTriggerDialog::onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT AddTriggerDialog::onOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(wID == IDOK)
	{
		CEdit edit;

		edit.Attach(GetDlgItem(IDC_TRIGGER));
		trigger.resize(edit.GetWindowTextLength()+1);
		trigger.resize(edit.GetWindowText(&trigger[0], trigger.size()));
		edit.Detach();

		edit.Attach(GetDlgItem(IDC_COMMAND));
		command.resize(edit.GetWindowTextLength()+1);
		command.resize(edit.GetWindowText(&command[0], command.size()));
		edit.Detach();

		if(wcslen(trigger.c_str()) <= 0)
		{
			MessageBox(L"Musisz poda� wyzwalacz!", L"B��d", MB_ICONERROR);
			return 0;
		}
		else if(wcslen(command.c_str()) <= 0)
		{
			MessageBox(L"Musisz poda� wyzwalacz!", L"B��d", MB_ICONERROR);
			return 0;
		}

		try
		{
			const std::tr1::wregex rx(trigger);
		}
		catch(const std::tr1::regex_error& err)
		{
			MessageBox(utils::text::toWide(err.what()).c_str(), L"B��d wyra�enia regularnego", MB_ICONERROR);
			return 0;
		}

		CButton btn;
		btn.Attach(GetDlgItem(IDC_STOP_PARSING));
		stopParsing = btn.GetCheck() != 0;
		btn.Detach();
	}

	EndDialog(wID);

	return 0;
}


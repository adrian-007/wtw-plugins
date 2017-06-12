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
#include "ItemDialog.h"

#pragma warning(disable:4244)
#include <boost/regex.hpp>
#pragma warning(default:4244)

LRESULT ItemDialog::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SetDlgItemText(IDC_REGEX, item.regex.c_str());
	SetDlgItemText(IDC_FORMAT, item.format.c_str());
	SET_CHECK(IDC_ICASE, item.ignoreCase ? 1 : 0);

	if(item.displayOnly) {
		SET_CHECK(IDC_MODIFY_DISPLAY, TRUE);
	} else {
		SET_CHECK(IDC_MODIFY_CONTENT, TRUE);
	}

	switch(item.direction) {
		case Replacer::Item::DIR_IN: {
			SET_CHECK(IDC_DIR_IN, 1);
			break;
		}
		case Replacer::Item::DIR_OUT: {
			SET_CHECK(IDC_DIR_OUT, 1);
			break;
		}
		default: {
			SET_CHECK(IDC_DIR_BOTH, 1);
		}
	}

	oldDir = item.direction;

	BOOL dummy;
	onModeChange(0, 0, 0, dummy);
	return 0;
}

LRESULT ItemDialog::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	return 0;
}

LRESULT ItemDialog::onClose(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	if(wID == IDOK) {
		std::wstring buf;

		buf.resize(::GetWindowTextLength(GetDlgItem(IDC_REGEX)) + 1);
		memset(&buf[0], 0, buf.size());

		buf.resize(GetDlgItemText(IDC_REGEX, &buf[0], buf.size()));
		item.regex = buf.c_str();

		try {
			const boost::wregex reg(buf);
		} catch(const boost::regex_error& e) {
			MessageBox(formatError(e.what()).c_str() , L"Z�e dane", MB_ICONERROR);
			return 0;
		}

		buf.resize(::GetWindowTextLength(GetDlgItem(IDC_FORMAT)) + 1);
		memset(&buf[0], 0, buf.size());

		buf.resize(GetDlgItemText(IDC_FORMAT, &buf[0], buf.size()));
		item.format = buf.c_str();

		item.ignoreCase = GET_CHECK(IDC_ICASE);
		item.displayOnly = GET_CHECK(IDC_MODIFY_DISPLAY);

		if(GET_CHECK(IDC_DIR_IN)) {
			item.direction = Replacer::Item::DIR_IN;
		} else if(GET_CHECK(IDC_DIR_OUT)) {
			item.direction = Replacer::Item::DIR_OUT;
		} else {
			item.direction = Replacer::Item::DIR_BOTH;
		}
	}
	EndDialog(wID);
	return 0;
}

LRESULT ItemDialog::onTest(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	std::wstring regex, format, message;

	regex.resize(::GetWindowTextLength(GetDlgItem(IDC_REGEX)) + 1);
	regex.resize(GetDlgItemText(IDC_REGEX, &regex[0], regex.size()));

	format.resize(::GetWindowTextLength(GetDlgItem(IDC_FORMAT)) + 1);
	format.resize(GetDlgItemText(IDC_FORMAT, &format[0], format.size()));

	message.resize(::GetWindowTextLength(GetDlgItem(IDC_TEST_MSG)) + 1);
	message.resize(GetDlgItemText(IDC_TEST_MSG, &message[0], message.size()));

	bool icase = GET_CHECK(IDC_ICASE);

	try {
		const boost::wregex reg(regex, icase ? boost::regex_constants::ECMAScript | boost::regex_constants::icase : boost::regex_constants::ECMAScript);
		message = boost::regex_replace(message, reg, format);
		MessageBox(message.c_str(), L"Wynik testu", MB_ICONINFORMATION);
	} catch(const boost::regex_error& e) {
		MessageBox(formatError(e.what()).c_str() , L"Wynik testu", MB_ICONERROR);
	}
	return 0;
}

std::wstring ItemDialog::formatError(const char* err) const {
	std::wstring msg;
	std::wstring buf;
	buf.resize(strlen(err));
	buf.resize(MultiByteToWideChar(CP_UTF8, 0, err, buf.size(), &buf[0], buf.size()));

	msg += L"B��d regex: ";
	msg += buf;
	return msg;
}

LRESULT ItemDialog::onModeChange(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */) {
/*	BOOL val = GET_CHECK(IDC_MODIFY_DISPLAY);

	if(val == false) {
		if(GET_CHECK(IDC_DIR_IN)) {
			oldDir = Replacer::Item::DIR_IN;
		} else if(GET_CHECK(IDC_DIR_OUT)) {
			oldDir = Replacer::Item::DIR_OUT;
		} else {
			oldDir = Replacer::Item::DIR_BOTH;
		}
		
		SET_CHECK(IDC_DIR_OUT, TRUE);
		SET_CHECK(IDC_DIR_IN, FALSE);
		SET_CHECK(IDC_DIR_BOTH, FALSE);
	} else {
		SET_CHECK(IDC_DIR_OUT, FALSE);
		switch(oldDir) {
			case Replacer::Item::DIR_IN: {
				SET_CHECK(IDC_DIR_IN, 1);
				break;
			}
			case Replacer::Item::DIR_OUT: {
				SET_CHECK(IDC_DIR_OUT, 1);
				break;
			}
			default: {
				SET_CHECK(IDC_DIR_BOTH, 1);
			}
		}
	}

	GetDlgItem(IDC_DIR_BOTH).EnableWindow(val);
	GetDlgItem(IDC_DIR_IN).EnableWindow(val);*/

	return 0;
}

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

#ifndef MAIN_DIALOG_HPP
#define MAIN_DIALOG_HPP

#include "resource.h"

class MainDialog : public CDialogImpl<MainDialog>, public CDialogResize<MainDialog> {
public:
	enum { IDD = IDD_MAIN };
	MainDialog() { }

	BEGIN_DLGRESIZE_MAP(MainDialog)
		DLGRESIZE_CONTROL(IDC_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_SET_DESC, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_DESTINATION, DLSZ_SIZE_X)

		//DLGRESIZE_CONTROL(IDC_REGEX_OPTIONS_GROUP, DLSZ_SIZE_X)
		//DLGRESIZE_CONTROL(IDC_ROOT_PATH, DLSZ_SIZE_X)
		//DLGRESIZE_CONTROL(IDC_BROWSE_ROOT_PATH, DLSZ_MOVE_X)
		//DLGRESIZE_CONTROL(IDC_PATTERN, DLSZ_SIZE_X)
		//DLGRESIZE_CONTROL(IDC_FORMAT, DLSZ_SIZE_X)

		DLGRESIZE_CONTROL(IDC_SOUND_TYPE, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_FILE, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_REM, DLSZ_MOVE_Y)
 		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_Y)

 		DLGRESIZE_CONTROL(IDC_GENERATE, DLSZ_MOVE_X)
 		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(MainDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, onCreateDialog)
		MESSAGE_HANDLER(WM_DESTROY, onDestroyDialog)
		MESSAGE_HANDLER(WM_CLOSE, onClose)
		COMMAND_ID_HANDLER(IDOK, onEndDialog)
		COMMAND_ID_HANDLER(IDC_FILE, onFile)
		COMMAND_ID_HANDLER(IDC_GENERATE, onGenerate)
		COMMAND_ID_HANDLER(IDC_REM, onRem)
		CHAIN_MSG_MAP(CDialogResize<MainDialog>)
	END_MSG_MAP()

private:
	LRESULT onCreateDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroyDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		BOOL dummy;
		onEndDialog(0, 0, 0, dummy);
		return 0;
	}

	LRESULT onEndDialog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onGenerate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onRem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	std::wstring getText(int id);
	std::string fromWide(const std::wstring& str);

	CListViewCtrl ctrlList;
	HFONT hFont;
};

#endif // MAIN_DIALOG_HPP

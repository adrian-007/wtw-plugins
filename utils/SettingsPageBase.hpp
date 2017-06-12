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

#ifndef SETTINGS_PAGE_BASE_HPP
#define SETTINGS_PAGE_BASE_HPP

#include "windows.hpp"

#define BASE_INHERIT(cls) public CDialogImpl<cls>, public CScrollImpl<cls>, public SettingsPageBase<cls>
#define MESSAGE_BACKGROUND() MESSAGE_HANDLER(WM_CTLCOLORDLG, OnBaseColor) \
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnBaseColor)

template<typename T>
class SettingsPageBase {
public:
	typedef SettingsPageBase<T> BasePage;

	SettingsPageBase() : hBrush(0), hFont(0) { 
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		hFont = utils::windows::getSystemFont();
	};

	~SettingsPageBase() {
		if(hBrush)
			DeleteObject(hBrush);

		if(hFont)
			DeleteObject(hFont);
	}

	virtual void close(bool save) = 0;

	static WTW_PTR callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
		wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;
	    
		wcscpy(info->windowCaption, T::windowCaption);
		wcscpy(info->windowDescrip, T::windowDescription);
	 
		T* pOptions = reinterpret_cast<T*>(info->page->ownerData);

		switch(info->action) {
			case WTW_OPTIONS_PAGE_ACTION_SHOW: {
				if(!pOptions) {
					pOptions = new T;
					info->page->ownerData = reinterpret_cast<void*>(pOptions);

					pOptions->Create(info->handle);

					CRect rc;
					pOptions->GetClientRect(&rc);
					pOptions->SetScrollSize(rc.Width(), rc.Height());
				}
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, TRUE);
				pOptions->ShowWindow(SW_SHOW);
				break;	 
			}
			case WTW_OPTIONS_PAGE_ACTION_MOVE: {
				if(pOptions) {
					pOptions->MoveWindow(info->x, info->y, info->cx, info->cy, TRUE);
				}
				break;
			}
			case WTW_OPTIONS_PAGE_ACTION_HIDE: {
				if(pOptions) {
					pOptions->ShowWindow(SW_HIDE);
				}
				break;
			}
			case WTW_OPTIONS_PAGE_ACTION_APPLY: {
				if(pOptions) {
					pOptions->close(true);
				}
				break;
			}
			case WTW_OPTIONS_PAGE_ACTION_OK:
			case WTW_OPTIONS_PAGE_ACTION_CANCEL: {
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

protected:
	LRESULT OnBaseColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { 
		return (LRESULT)hBrush;
	}

	void ApplyFont() {
		utils::windows::SetChildFont(static_cast<T*>(this)->m_hWnd, hFont);
	}

	HBRUSH hBrush;
	HFONT hFont;
};

#endif // SETTINGS_PAGE_BASE_HPP

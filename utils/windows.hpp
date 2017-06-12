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

#ifndef WINDOWS_HELPER_HPP
#define WINDOWS_HELPER_HPP

namespace utils {
	namespace windows {
		class SetChildFont {
		public:
			SetChildFont(HWND parent, HFONT font) {
				::SendMessage(parent, WM_SETFONT, (WPARAM)font, 0);
				HWND child = ::GetWindow(parent, GW_CHILD);
				while(child) {
					::SendMessage(child, WM_SETFONT, (WPARAM)font, 0);
					child = ::GetNextWindow(child, GW_HWNDNEXT);
				}
			}
		};

		static HFONT getSystemFont() {
			NONCLIENTMETRICS ncm = { 0 };
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
			return CreateFontIndirectW(&ncm.lfMessageFont);
		}

#ifdef __ATLAPP_H__
		// only when we included wtl

		static void SetExplorerTheme(HWND wnd, LPCTSTR name = _T("explorer"), LPCTSTR list = 0) {
			if(RunTimeHelper::IsVista() && RunTimeHelper::IsThemeAvailable()) {
				typedef HRESULT (CALLBACK* LPFUNC)(HWND, LPCWSTR, LPCWSTR);
				HMODULE uxdll = LoadLibrary(_T("uxtheme.dll"));
				if(uxdll) {
					LPFUNC _SetWindowTheme = (LPFUNC)GetProcAddress(uxdll, "SetWindowTheme");
					if(_SetWindowTheme) {
						_SetWindowTheme(wnd, name, list);
					}
					FreeLibrary(uxdll);
				}
			}
		}

#endif // __ATLAPP_H__

	} // namespace windows
} // namespace utils

#endif

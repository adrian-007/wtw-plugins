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

#define WIN32_LEAN_AND_MEAN
#define STRICT

#define _WIN32_WINNT	0x0501
#define _WIN32_IE		0x0501
#define WINVER			0x0501

#include <windows.h>
#include <commctrl.h>

#include <plInterface.h>

#include <cmath>

#include "SnapWindow.h"
#include "../pluginInfo.h"

WTWPLUGINFO plugInfo = {
	sizeof(WTWPLUGINFO),
	L"wtf_dock",
	L"Dokowanie okna głównego",
	__COPYRIGHTS(2009),
	__AUTHOR_NAME,
	__AUTHOR_CONTACT,
	__AUTHOR_WEBSITE,
	__AUTOUPDATE_URL(L"wtf_dock.xml"),									
	PLUGIN_API_VERSION,
	MAKE_QWORD(0, 2, 0, 3),
	WTW_CLASS_UTILITY,
	0,
	L"{333AA9BE-C584-4726-AEB6-BAF47F89496D}",
	0,
	0, 0, 0, 0
};

CSnapWindow sh;
HWND hMainWnd = 0;
WTWFUNCTIONS* wtw = 0;

HANDLE hChatWndCreate = 0;
HANDLE hChatWndDestroy = 0;

LRESULT CALLBACK SubclassFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	if(uIdSubclass == 0) {
		switch(uMsg) {
			case WM_MOVING:
				return sh.OnSnapMoving(hWnd, uMsg, wParam, lParam);
			case WM_ENTERSIZEMOVE:
				return sh.OnSnapEnterSizeMove(hWnd, uMsg, wParam, lParam);
		}
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

extern "C" {
	WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) { return &plugInfo; }

	int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS *fC) {
		wtw = fC;

		hMainWnd = FindWindowW(L"{B993D471-D465-43f2-BBA5-DEEA18A1789E}", 0);
		if(!hMainWnd) {
			MessageBoxW(NULL, L"Nie odnaleziono głównego okna programu!", L"wtf_dock", MB_OK);
			return 1;
		}

		if(SetWindowSubclass(hMainWnd, SubclassFunc, 0, 0) == FALSE) {
			MessageBoxW(hMainWnd, L"Nie udało się podczepić procedury pod okno główne, co oznacza, że wtyczka jest na tym systemie bezużyteczna :(", L"wtf_dock", MB_ICONERROR);
			return 1;
		}

		return 0;
	}

	int __stdcall pluginUnload(DWORD callReason) {
		if(hMainWnd) {
			RemoveWindowSubclass(hMainWnd, SubclassFunc, 0);
		}
		wtw = 0;
		return 0;
	}
} // extern "C"

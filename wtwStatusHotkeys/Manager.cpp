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
#include "Manager.h"

Manager::Manager(HINSTANCE hInst) : hInstance(hInst) 
{
	statuses[0] = static_cast<int>(WTW_PRESENCE_OFFLINE);
	statuses[1] = static_cast<int>(WTW_PRESENCE_ONLINE);
	statuses[2] = static_cast<int>(WTW_PRESENCE_CHAT);
	statuses[3] = static_cast<int>(WTW_PRESENCE_DND);
	statuses[4] = static_cast<int>(WTW_PRESENCE_AWAY);
	statuses[5] = static_cast<int>(WTW_PRESENCE_XA);
	statuses[6] = static_cast<int>(WTW_PRESENCE_INV);

	this->Create(FindWindowW(L"{B993D471-D465-43f2-BBA5-DEEA18A1789E}", 0));

	char c = '0';
	wchar_t wc = L'0';

	std::wstring atomName;

	for(int i = 0; i < ARRAY_SIZE; ++i)
	{
		atomName = L"wtwStatusHotkeys_hotkey_";
		atomName += wc++;

		keys[i] = ::GlobalAddAtom(atomName.c_str());

		::RegisterHotKey(this->m_hWnd, keys[i], MOD_CONTROL | MOD_SHIFT, c++);
	}

	atomName = L"wtwStatusHotkeys_changeDesc";
	descriptionKey = ::GlobalAddAtom(atomName.c_str());
	::RegisterHotKey(this->m_hWnd, descriptionKey, MOD_CONTROL | MOD_SHIFT, '8');
}

Manager::~Manager()
{
	for(int i = 0; i < ARRAY_SIZE; ++i)
	{
		::UnregisterHotKey(this->m_hWnd, keys[i]);
		::GlobalDeleteAtom(keys[i]);
	}

	::UnregisterHotKey(this->m_hWnd, descriptionKey);
	::GlobalDeleteAtom(descriptionKey);

	this->DestroyWindow();
}

LRESULT Manager::onHotkey(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ATOM a = (ATOM)wParam;
	wtwPresenceDef p;
	initStruct(p);
	wtw->fnCall(WTW_PF_WTW_STATUS_GET, reinterpret_cast<WTW_PARAM>(&p), 0);

	if(a == descriptionKey)
	{
		HWND hMainWnd = FindWindowW(L"{B993D471-D465-43f2-BBA5-DEEA18A1789E}", 0);
		HWND hStatusWnd = FindWindowExW(hMainWnd, 0, L"22CE91C3-9FD2-4a71-BD10-64C1F9474760", 0);

		if(hStatusWnd)
		{
			// pokaz okno
			::ShowWindow(hMainWnd, SW_SHOW);
			::SetActiveWindow(hMainWnd);
			::SetForegroundWindow(hMainWnd);

			// hack, to jest z WM_NCHITTEST
			::SendMessage(hStatusWnd, WM_MOUSEACTIVATE, (WPARAM)hMainWnd, 0x2010001); 
			
			// to jest pozycja klikniecia, cos a'la 10, 60
			LPARAM pos = 0x1000EA;
			::SendMessage(hStatusWnd, WM_LBUTTONDOWN, MK_LBUTTON, pos);
			::SendMessage(hStatusWnd, WM_LBUTTONUP, 0x00, pos);
		}
	} 
	else
	{
		for(int i = 0; i < ARRAY_SIZE; ++i)
		{
			if(keys[i] == a)
			{
				p.curStatus = statuses[i];
				wtw->fnCall(WTW_PF_WTW_STATUS_SET, reinterpret_cast<WTW_PARAM>(&p), 0);
				break;
			}
		}
	}

	return 0;
}

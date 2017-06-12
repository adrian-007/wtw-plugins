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
#include "wtwStatusHistory.hpp"
#include "resource.h"

#include "../utils/windows.hpp"

#define COMBO_EDIT 1001

#define SUBCLASS_ID_MAIN_WND 0
#define SUBCLASS_ID_STATUS_WND 1
#define SUBCLASS_ID_EDIT_WND 2
#define SUBCLASS_ID_LISTVIEW_WND 3

wtwStatusHistoryManager* wtwStatusHistoryManager::inst = 0;

wtwStatusHistoryManager::wtwStatusHistoryManager(HINSTANCE hInst) : hInstance(hInst), lastTipItem(-1), lockList(false) {
	hMainWnd = FindWindowW(L"{B993D471-D465-43f2-BBA5-DEEA18A1789E}", 0);
	hStatusWnd = FindWindowExW(hMainWnd, 0, L"22CE91C3-9FD2-4a71-BD10-64C1F9474760", 0);
	hStatusEditWnd = FindWindowExW(hStatusWnd, 0, L"Edit", 0);

	SetWindowSubclass(hMainWnd, subclassFunc_static, SUBCLASS_ID_MAIN_WND, (DWORD_PTR)this);
	SetWindowSubclass(hStatusWnd, subclassFunc_static, SUBCLASS_ID_STATUS_WND, (DWORD_PTR)this);
	SetWindowSubclass(hStatusEditWnd, subclassFunc_static, SUBCLASS_ID_EDIT_WND, (DWORD_PTR)this);

	hFont = utils::windows::getSystemFont();

	ctrlList.Create(hStatusWnd, 0, 0, WS_POPUP | WS_VSCROLL | LVS_REPORT | LVS_NOCOLUMNHEADER, WS_EX_CLIENTEDGE | WS_EX_TOPMOST);
	ctrlList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	ctrlList.InsertColumn(1, L"");
	ctrlList.SetFont(hFont);
	utils::windows::SetExplorerTheme(ctrlList.m_hWnd);

    hToolTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_NOFADE | TTS_BALLOON,		
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, ctrlList.m_hWnd, NULL, hInstance, 0);
    SetWindowPos(hToolTip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	SendMessage(hToolTip, TTM_SETMAXTIPWIDTH, 0, 200);
	SendMessage(hToolTip, WM_SETFONT, (WPARAM)hFont, 0);

	SetWindowSubclass(ctrlList.m_hWnd, subclassFunc_static, SUBCLASS_ID_LISTVIEW_WND, (DWORD_PTR)this);

	std::wstring buf;
	wchar_t name[1024];
	int cnt = cfgGetInt(L"itemCount", 20);
	for(int i = 0; i < cnt; ++i) {
		wsprintf(name, L"item.%d", i);
		cfgGetStr(buf, name);
		if(wcslen(buf.c_str()) > 0)
			ctrlList.AddItem(ctrlList.GetItemCount(), 0, buf.c_str());
		cfgSetStr(name, L"");
	}

	hook = wtw->evHook(WTW_ON_BEFORE_STATUS_SET, wtwStatusHistoryManager::onDescChange, (void*)this);
}

wtwStatusHistoryManager::~wtwStatusHistoryManager() {
	wtw->evUnhook(hook);

	wchar_t buf[4096];
	wchar_t name[1024];
	int cnt = cfgGetInt(L"itemCount", 20);

	for(int i = 0; i < cnt && i < ctrlList.GetItemCount(); ++i) {
		ctrlList.GetItemText(i, 0, buf, sizeof(buf)-1);
		wsprintf(name, L"item.%d", i);
		cfgSetStr(name, buf);
	}

	RemoveWindowSubclass(ctrlList.m_hWnd, subclassFunc_static, SUBCLASS_ID_LISTVIEW_WND);
	ctrlList.DestroyWindow();
	DestroyWindow(hToolTip);
	
	RemoveWindowSubclass(hMainWnd, subclassFunc_static, SUBCLASS_ID_MAIN_WND);
	RemoveWindowSubclass(hStatusWnd, subclassFunc_static, SUBCLASS_ID_STATUS_WND);
	RemoveWindowSubclass(hStatusEditWnd, subclassFunc_static, SUBCLASS_ID_EDIT_WND);

	::DeleteObject(hFont);
}

void wtwStatusHistoryManager::onDescriptionChange(wtwNewGlobalStatus* gs) {
	if(gs->newStatus.curDescription == 0 || wcslen(gs->newStatus.curDescription) == 0)
		return;

	int n = ctrlList.GetItemCount();
	wchar_t buf[4096];
	bool found = false;
	for(int i = 0; i < n; ++i) {
		memset(buf, 0, sizeof(buf));

		ctrlList.GetItemText(i, 0, buf, sizeof(buf)-1);
		if(wcsncmp(gs->newStatus.curDescription, buf, sizeof(buf)-1) == 0) {
			ctrlList.DeleteItem(i);
			ctrlList.AddItem(0, 0, buf);
			found = true;
		}
	}
	if(!found) {
		ctrlList.AddItem(0, 0, gs->newStatus.curDescription);
	}
}

void wtwStatusHistoryManager::CreateToolTip(const wchar_t* text) {
    SetWindowPos(hToolTip, HWND_TOPMOST, 0, 0, 0, 0,  SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    TOOLINFO ti = { 0 };
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS | TTF_PARSELINKS;
    ti.hwnd = ctrlList.m_hWnd;
	ti.hinst = hInstance;
    ti.lpszText = (LPWSTR)text;
    ctrlList.GetClientRect(&ti.rect);

	if(text)
		SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);	
	else
		SendMessage(hToolTip, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
} 

void wtwStatusHistoryManager::showUpdatedList() {
	int n = ctrlList.GetItemCount();
	if(n > 0) {
		CWindow edit(hStatusEditWnd);
		CRect rc;
		edit.GetWindowRect(&rc);
		int tmp = 5;
		{
			CRect rcItem;
			ctrlList.GetItemRect(0, &rcItem, LVIR_BOUNDS);
			if(n > 15) {
				tmp += (rcItem.Height()) * 15;
			} else {
				tmp += (rcItem.Height()) * n;
			}
			ctrlList.SelectItem(0);
		}
		ctrlList.SetWindowPos(0, rc.left, rc.bottom+5, rc.Width(), tmp, SWP_NOZORDER);
		tmp = 5;
		if(n > 15) {
			tmp += GetSystemMetrics(SM_CXVSCROLL);
		}
		
		ctrlList.SetColumnWidth(0, rc.Width()-tmp);
		ctrlList.SetFocus();
	}
}

LRESULT wtwStatusHistoryManager::subclassFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
	if(uIdSubclass == SUBCLASS_ID_MAIN_WND) {

	} else if(uIdSubclass == SUBCLASS_ID_STATUS_WND) {
		switch(uMsg) {
 			case WM_NOTIFY: {
				LPNMHDR nmhdr = (LPNMHDR)lParam;
				if(nmhdr->hwndFrom == ctrlList.m_hWnd) {
					switch(nmhdr->code) {
						case NM_RETURN:
						case NM_DBLCLK: {
							int i = ctrlList.GetSelectedIndex();
							if(i >= 0) {
								wchar_t buf[4096] = { 0 };
								ctrlList.GetItemText(i, 0, buf, sizeof(buf)-1);
								
								CEdit edit(hStatusEditWnd);
								edit.SetWindowText(buf);
								edit.SetFocus();
								edit.SetSelNone();
							}
							break;
						}
					}
				}
				break;
			}
		}
	} else if(uIdSubclass == SUBCLASS_ID_EDIT_WND) {
		switch(uMsg) {
			case WM_MBUTTONDOWN:
			case WM_CHAR: {
				if(uMsg == WM_MBUTTONDOWN || (wParam == L' ' && ((GetKeyState(VK_LCONTROL) & 0xff00) != 0 || (GetKeyState(VK_RCONTROL) & 0xff00) != 0))) {
					showUpdatedList();
					return TRUE;
				}
				break;
			}
		}
	} else if(uIdSubclass == SUBCLASS_ID_LISTVIEW_WND) {
		switch(uMsg) {
			case WM_SETFOCUS: {
				if(lockList == false)
					ctrlList.ShowWindow(SW_SHOW);
				break;
			}
			case WM_KILLFOCUS: {
				if(lockList == false)
					ctrlList.ShowWindow(SW_HIDE);
				break;
			}
			case WM_KEYDOWN: {
				bool wasPressed = (lParam & 0x40000000) != 0;
				switch(wParam) {
					case VK_DELETE: {
						if(!wasPressed && ctrlList.GetItemCount() > 0) {
							int i = ctrlList.GetSelectedIndex();
							if(i >= 0) {
								lockList = true;
								if(MessageBox(ctrlList.m_hWnd, L"Czy napewno chcesz usun�� wybrany element?", L"wtwStatusHistory - historia opis�w", MB_ICONQUESTION | MB_YESNO) == IDYES) {
									ctrlList.DeleteItem(i);
								}
								lockList = false;
								ctrlList.ShowWindow(SW_HIDE);
								showUpdatedList();
							}
						}
						break;
					}
					case VK_CLEAR: {
						if(!wasPressed && ctrlList.GetItemCount() > 0) {
							lockList = true;
							if(MessageBox(ctrlList.m_hWnd, L"Czy napewno chcesz usun�� wszystkie elementy?", L"wtwStatusHistory - historia opis�w", MB_ICONQUESTION | MB_YESNO) == IDYES) {
								ctrlList.DeleteAllItems();
								CWindow wnd(hStatusEditWnd);
								wnd.SetFocus();
							}
							lockList = false;
							ctrlList.ShowWindow(SW_HIDE);
							showUpdatedList();
						}
						break;
					}
					case VK_ESCAPE: {
						CWindow(hStatusEditWnd).SetFocus();
						break;
					}
				}
				break;
			}
			case WM_MOUSEMOVE: {
				int i = ctrlList.GetHotItem();
				if(i >= 0) {
					if(lastTipItem != i) {
						CreateToolTip(0);
						memset(toolBuffer, 0, sizeof(toolBuffer));
						ctrlList.GetItemText(i, 0, toolBuffer, sizeof(toolBuffer)-1);
						CreateToolTip(toolBuffer);
						lastTipItem = i;
					}
				} else {
					CreateToolTip(0);
					lastTipItem = -1;
				}
				break;
			}
		}
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

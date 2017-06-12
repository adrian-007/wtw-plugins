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
#include "resource.h"

#include "TabControl.h"

#include "../utils/wtw.hpp"
#include "../utils/windows.hpp"

#pragma comment(lib, "gdiplus.lib")

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SHIFT_MENU_INDEX 1

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TabControl* TabControl::__instance = 0;

void TabControl::init()
{
	if(__instance == 0)
	{
		__instance = new TabControl;
	}
}

void TabControl::cleanup()
{
	if(__instance)
	{
		delete __instance;
		__instance = 0;
	}
}

TabControl* TabControl::getInstance()
{
	return __instance;
}

TabControl::TabControl(void)
{
	wtwWnd.main = ::FindWindowW(L"{B993D471-D465-43f2-BBA5-DEEA18A1789E}", 0);
	wtwWnd.list = ::FindWindowExW(wtwWnd.main, 0, L"1D60EF61-DE44-4eb8-A672-AE0461C85830", 0);
	wtwWnd.status = ::FindWindowExW(wtwWnd.main, 0, L"22CE91C3-9FD2-4a71-BD10-64C1F9474760", 0);
	
	wtwPanelDef p;
	p.panelId = L"wtwListTabs/panel";
	p.panelIcon = L"wtwListTabs/icon16";
	p.callback = panelCallback;
	p.cbData = this;
	p.panelHint = L"Karty";

	wtw->fnCall(WTW_PANEL_ADD, (WTW_PARAM)&p, 0);

	colors.reload();

	::SetWindowSubclass(wtwWnd.main, SubclassFunc, 0, reinterpret_cast<DWORD_PTR>(this));
	//::SetWindowSubclass(wtwWnd.list, SubclassFunc, 1, reinterpret_cast<DWORD_PTR>(this));

	font = utils::windows::getSystemFont();

	wndHook = ::SetWindowsHookEx(WH_KEYBOARD, TabControl::KeyboardProc, 0, GetCurrentThreadId());
}

TabControl::~TabControl(void)
{
	if(wndHook != 0)
	{
		UnhookWindowsHookEx(wndHook);
	}

	::RemoveWindowSubclass(wtwWnd.main, SubclassFunc, 0);
	//::RemoveWindowSubclass(wtwWnd.list, SubclassFunc, 1);	

	wtwPanelDef p;
	p.panelId = L"wtwListTabs/panel";
	wtw->fnCall(WTW_PANEL_DEL, (WTW_PARAM)&p, 0);

	cfgSetInt(SETTING_LAST_TAB_POS, this->GetCurSel());
	DestroyWindow();

	::DeleteObject(font);
	font = 0;
}

void TabControl::Create()
{
	Base::Create(wtwWnd.main, 0, 0, WS_CHILD | WS_VISIBLE | TCS_MULTILINE | TCS_FOCUSNEVER /*| TCS_OWNERDRAWFIXED*/);
//	ModifyStyle(TCS_RAGGEDRIGHT, 0);

	ReloadSettings();
}

TabControl::Colors::~Colors()
{
	clear();
}

void TabControl::Colors::reload()
{
	clear();

	background.SetFromCOLORREF(cfgGetInt(SETTING_COLOR_BG, DEFAULT_COLOR_BG));

	activeTabBackgroundUp.SetFromCOLORREF(cfgGetInt(SETTING_COLOR_BG_ACTIVE_UP, DEFAULT_COLOR_BG_ACTIVE));
	activeTabBackgroundDown.SetFromCOLORREF(cfgGetInt(SETTING_COLOR_BG_ACTIVE_DOWN, DEFAULT_COLOR_BG_ACTIVE));
	activeTabTextColor.SetFromCOLORREF(cfgGetInt(SETTING_COLOR_FONT_ACTIVE, DEFAULT_COLOR_FONT_ACTIVE));

	inactiveTabBackgroundUp.SetFromCOLORREF(cfgGetInt(SETTING_COLOR_BG_INACTIVE_UP, DEFAULT_COLOR_BG_INACTIVE));
	inactiveTabBackgroundDown.SetFromCOLORREF(cfgGetInt(SETTING_COLOR_BG_INACTIVE_DOWN, DEFAULT_COLOR_BG_INACTIVE));
	inactiveTabTextColor.SetFromCOLORREF(cfgGetInt(SETTING_COLOR_FONT_INACTIVE, DEFAULT_COLOR_FONT_INACTIVE));
}

void TabControl::Colors::clear()
{

}

void TabControl::ReloadSettings()
{
	colors.reload();
	int style = this->GetStyle();
	int newStyle = style | WS_TABSTOP | TCS_TABS;

	HFONT hFont = utils::windows::getSystemFont();

	LOGFONT lf = { 0 };
	::GetObject(hFont, sizeof(LOGFONT), &lf);

	{
		int tabPos = cfgGetInt(SETTING_TAB_POS, 0);
		int tfFlags = textFormat.GetFormatFlags();

		switch(tabPos)
		{
		case TAB_POS_DOWN:
			{
				tfFlags &= ~(Gdiplus::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlagsDirectionRightToLeft);
				newStyle &= ~(TCS_VERTICAL | TCS_RIGHT);
				newStyle |= TCS_BOTTOM;
				break;
			}
		case TAB_POS_LEFT:
			{
				tfFlags |= Gdiplus::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlagsDirectionRightToLeft;
				newStyle &= ~(TCS_BOTTOM | TCS_RIGHT);
				newStyle |= TCS_VERTICAL;
				//lf.lfEscapement = lf.lfOrientation = -2700;
				break;
			}
		/*case TAB_POS_RIGHT:
			{
				tfFlags |= Gdiplus::StringFormatFlagsDirectionVertical;
				this->ModifyStyle(0, TCS_VERTICAL | TCS_RIGHT);
				break;
			}*/
		default:
			{
				tfFlags &= ~(Gdiplus::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlagsDirectionRightToLeft);
				newStyle &= ~(TCS_BOTTOM | TCS_VERTICAL | TCS_RIGHT);
			}
		}

		textFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		textFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		textFormat.SetFormatFlags(tfFlags);
	}

	if(cfgGetInt(SETTING_USE_SYSTEM_TABS, 1))
	{
		newStyle &= ~(TCS_OWNERDRAWFIXED);
	}
	else
	{
		newStyle |= TCS_OWNERDRAWFIXED;
	}

	SetFont(NULL);
	::DeleteObject(font);
	font = ::CreateFontIndirect(&lf);

	ModifyStyle(style, newStyle);
	SetFont(font);

	if((newStyle & TCS_VERTICAL) && (newStyle & TCS_OWNERDRAWFIXED) == 0)
	{
		utils::windows::SetExplorerTheme(m_hWnd, L"", L"");
	}
	else
	{
		utils::windows::SetExplorerTheme(m_hWnd);
	}

	ActivatePanel();
}

void TabControl::ActivatePanel()
{
	wtwPanelDef p;
	p.panelId = L"wtwListTabs/panel";
	wtw->fnCall(WTW_PANEL_SWITCH, (WTW_PARAM)p.panelId, WPS_NO_WINDOW_ACTIVATE);
}

void TabControl::RepositionTabs(bool old)
{
	if(old)
	{
		CRect rc;
		GetWindowRect(&rc);
		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;
		::ScreenToClient(wtwWnd.main, (LPPOINT)&rc);
		MoveWindow(rc.left, rc.top, cx, cy, TRUE);
	}
	else
	{
		CRect rc;
		::GetWindowRect(wtwWnd.list, &rc);
		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;
		::ScreenToClient(wtwWnd.main, (LPPOINT)&rc);
		::MoveWindow(wtwWnd.list, rc.left, rc.top, cx, cy, TRUE);
	}
}

void TabControl::RepaintTabs() 
{
	CRect rc;
	GetClientRect(&rc);
	InvalidateRect(&rc, TRUE);
	UpdateWindow();
}

void TabControl::RebuildTabs()
{
	this->DeleteAllItems();

	MenuIDList ids = getMenuIDs();

	if(ids.size() > SHIFT_MENU_INDEX)
	{
		for(size_t i = SHIFT_MENU_INDEX + 1; i < ids.size(); ++i)
		{
			AddItem(ids[i].second.c_str());
		}
	}

	SelectTab(0);
	RepaintTabs();
}

void TabControl::SelectTab(int pos)
{
	MenuIDList ids = getMenuIDs();
	WPARAM tmp = 0;
	
	if(pos < 0)
	{
		tmp = LOWORD(ids[SHIFT_MENU_INDEX].first) + HIWORD(0);
	} 
	else 
	{
		size_t tabPos = pos + SHIFT_MENU_INDEX + 1;
		
		if(pos < this->GetItemCount() && ids.size() > tabPos)
		{
			tmp = LOWORD(ids[tabPos].first) + HIWORD(0);
		}
	}

	SendMessage(wtwWnd.main, WM_COMMAND, tmp, 0);
}

void TabControl::DrawTab(Gdiplus::Graphics& g, LPDRAWITEMSTRUCT ds, bool active)
{
	using namespace Gdiplus;
	std::wstring text;

	{
		text.resize(2048);

		TCITEM tci = { 0 };
		tci.mask = TCIF_TEXT;
		tci.pszText = &text[0];
		tci.cchTextMax = text.size() - 1;

		this->GetItem(ds->itemID, &tci);

		text = tci.pszText;
	}

	RectF bounds((REAL)ds->rcItem.left, (REAL)ds->rcItem.top, (REAL)(ds->rcItem.right - ds->rcItem.left), (REAL)(ds->rcItem.bottom - ds->rcItem.top));
	
	int tabPos = cfgGetInt(SETTING_TAB_POS, 0);
	Gdiplus::LinearGradientMode grMode = Gdiplus::LinearGradientModeVertical;

	switch(tabPos)
	{
	case TAB_POS_UP:
	case TAB_POS_DOWN:
		{
			grMode = Gdiplus::LinearGradientModeVertical;
			break;
		}
	case TAB_POS_LEFT:
		{
			grMode = Gdiplus::LinearGradientModeHorizontal;
			break;
		}
	}

	HDC hDC = g.GetHDC();
	Font f(hDC, font);
	g.ReleaseHDC(hDC);

	Gdiplus::LinearGradientBrush* brBackground;
	Gdiplus::SolidBrush* brFont;

	if(active)
	{
		brBackground = new Gdiplus::LinearGradientBrush(bounds, colors.activeTabBackgroundUp, colors.activeTabBackgroundDown, grMode);
		brFont = new SolidBrush(colors.activeTabTextColor);
	}
	else
	{
		brBackground = new Gdiplus::LinearGradientBrush(bounds, colors.inactiveTabBackgroundUp, colors.inactiveTabBackgroundDown, grMode);
		brFont = new SolidBrush(colors.inactiveTabTextColor);
	}

	g.FillRectangle(brBackground, bounds);
	g.DrawString(text.c_str(), text.length(), &f, bounds, &textFormat, brFont);

	delete brBackground;
	delete brFont;
}

MenuIDList TabControl::getMenuIDs()
{
	MenuIDList ids;
	HMENU menu = ::GetMenu(wtwWnd.main);

	if(menu)
	{
		HMENU viewMenu = ::GetSubMenu(menu, 1);
		
		if(viewMenu)
		{			
			std::wstring buf;	
			buf.resize(2048);		

			MENUITEMINFO mii = { 0 };
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_STRING | MIIM_ID | MIIM_FTYPE;
			
			for(int i = 0; ; ++i)
			{
				mii.cch = buf.size();
				mii.dwTypeData = &buf[0];
				mii.fType = 0;

				memset(&buf[0], 0, buf.size());

				if(::GetMenuItemInfo(viewMenu, i, TRUE, &mii))
				{
					if(mii.fType & MFT_SEPARATOR)
					{
						continue;
					}

					ids.push_back(std::make_pair(mii.wID, std::wstring(buf)));
				}
				else
				{
					break;
				}
			}
		}
	}

	MenuIDList::iterator it = ids.begin();

	while(it != ids.end())
	{
		std::wstring& s = (*it).second;

		if(s.find(L"Zarz�dzaj filtrami") != std::string::npos)
		{
			++it;
			continue;
		}

		if(s.find(L"Brak filtra") != std::string::npos)
		{
			break;
		}
		it = ids.erase(it++);
	}

	return ids;
}

LRESULT TabControl::onMainWindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{
	case WM_COMMAND: 
		{
			if(HIWORD(wParam) == 0)
			{
				MenuIDList ids = getMenuIDs();

				for(size_t i = 0; i < ids.size(); ++i) 
				{
					if(ids[i].first == LOWORD(wParam))
					{
						if(i == 0)
						{
							// zarzadzanie filtrami
							LRESULT ret = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
							RebuildTabs();
							RepaintTabs();
							return ret;
						}
						else if(i >= SHIFT_MENU_INDEX)
						{
							int n = i - SHIFT_MENU_INDEX;

							if(n == 0)
							{
								LRESULT ret = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);

								if(this->IsWindow() && this->IsWindowVisible()) 
								{
									SelectTab(0);
									RepaintTabs();
								}
								return ret;
							}
							n -= 1;
							this->SetCurSel(n);
							RepaintTabs();
						}
						break;
					}
				}
			}
			break;
		}
	case WM_NOTIFY: 
		{
			LPNMHDR hdr = (LPNMHDR)lParam;

			if(hdr->hwndFrom == this->m_hWnd && hdr->code == TCN_SELCHANGE) 
			{
				SelectTab(this->GetCurSel());
				RepaintTabs();
			}
			break;
		}
	}

	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT TabControl::onStatusWindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT TabControl::onListWindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT TabControl::onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	using namespace Gdiplus;
	
	if(this->GetStyle() & TCS_OWNERDRAWFIXED)
	{
		CRect rc;
		GetClientRect(&rc);

		Bitmap bmp(rc.Width(), rc.Height());
		Graphics g(&bmp);

		SolidBrush sb(colors.background);
		g.FillRectangle(&sb, rc.left, rc.top, rc.Width(), rc.Height());

		DRAWITEMSTRUCT dis;
		dis.CtlType = ODT_TAB;
		dis.CtlID = 0;
		dis.hwndItem = m_hWnd;
		//dis.hDC = dc;
		dis.itemAction = ODA_DRAWENTIRE;

		int nTabs = this->GetItemCount();
		int activeTab = this->GetCurSel();

		if(nTabs > 0)
		{
			while(nTabs--)
			{
				dis.itemID = nTabs;

				this->GetItemRect(nTabs, &dis.rcItem);
				DrawTab(g, &dis, nTabs == activeTab);
			}
		}

		CPaintDC paint(m_hWnd);
		Graphics(m_hWnd).DrawImage(&bmp, 0, 0);
		return 0;
	}

	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT TabControl::onEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(this->GetStyle() & TCS_OWNERDRAWFIXED)
	{
		return TRUE;
	}

	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT TabControl::onDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	//__LOG(wtw, L"tabs", L"onDrawItem");
	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT TabControl::onMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	//__LOG(wtw, L"tabs", L"onMeasureItem");
	return DefWindowProc(uMsg, wParam, lParam);
}

WTW_PTR TabControl::panelCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwPanelEvent* pe = (wtwPanelEvent*)wParam;
	TabControl* tc = reinterpret_cast<TabControl*>(ptr);

	if(tc && pe && wcscmp(pe->panelId, L"wtwListTabs/panel") == 0)
	{
		switch(pe->event)
		{
		case WTW_PANEL_EVENT_SHOW:
		case WTW_PANEL_EVENT_MOVE:
			{
				CRect rc(pe->rc);
				HDWP hdwp;
				
				hdwp = ::BeginDeferWindowPos(1);
				hdwp = ::DeferWindowPos(hdwp, tc->m_hWnd, tc->wtwWnd.list, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
				::EndDeferWindowPos(hdwp);
				
				tc->AdjustRect(FALSE, &rc);
				
				hdwp = ::BeginDeferWindowPos(1);
				hdwp = ::DeferWindowPos(hdwp, tc->wtwWnd.list, 0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
				::EndDeferWindowPos(hdwp);
				
				tc->SelectTab(tc->GetCurSel());
				tc->RepaintTabs();
				break;
			}
		case WTW_PANEL_EVENT_HIDE:
			{
				CRect rc(pe->rc);

				HDWP hdwp;
				
				hdwp = ::BeginDeferWindowPos(1);
				hdwp = ::DeferWindowPos(hdwp, tc->wtwWnd.list, tc->m_hWnd, rc.left, rc.top, rc.Width(), rc.Height(), 0);
				::EndDeferWindowPos(hdwp);

				tc->ShowWindow(SW_HIDE);
				::ShowWindow(tc->wtwWnd.list, SW_HIDE);
				tc->SelectTab(-1);
				break;
			}
		}
	}
	return 0;
}

LRESULT CALLBACK TabControl::KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(code >= 0 && GetKeyState(VK_LCONTROL) < 0 && wParam == VK_OEM_3 && (lParam & (1 << 31)))
	{
		TabControl* ctrl = TabControl::getInstance();
		ctrl->SelectTab((ctrl->GetCurSel() + 1) % ctrl->GetItemCount());
	}

	return CallNextHookEx(0, code, wParam, lParam);
}

LRESULT TabControl::SubclassFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	TabControl* tc = reinterpret_cast<TabControl*>(dwRefData);
	if(tc)
	{
		switch(uIdSubclass)
		{
		case 0:
			{
				return tc->onMainWindowSubclass(hWnd, uMsg, wParam, lParam);
			}
		case 1:
			{
				return tc->onListWindowSubclass(hWnd, uMsg, wParam, lParam);
			}
		case 2:
			{
				return tc->onStatusWindowSubclass(hWnd, uMsg, wParam, lParam);
			}
		}
	}

	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

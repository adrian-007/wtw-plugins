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

#pragma once

#include <gdiplus.h>

#include <deque>
#include <string>

typedef std::deque<std::pair<int, std::wstring> > MenuIDList;

class TabControl : public CWindowImpl<TabControl, CTabCtrl>
{
	typedef CWindowImpl<TabControl, CTabCtrl> Base;

public:
	BEGIN_MSG_MAP(TabControl)
		MESSAGE_HANDLER(WM_PAINT, onPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, onEraseBackground)
		MESSAGE_HANDLER(WM_DRAWITEM, onDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM, onMeasureItem)
	END_MSG_MAP()

	static void init();
	static void cleanup();
	static TabControl* getInstance();

	TabControl(void);
	~TabControl(void);

	void Create();

	void ReloadSettings();

	void SelectTab(int pos);
	void RepaintTabs();
	void RebuildTabs();
	void RepositionTabs(bool old);
	void DrawTab(Gdiplus::Graphics& g, LPDRAWITEMSTRUCT lpdis, bool active);

	void ActivatePanel();

protected:
	LRESULT onPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	static LRESULT CALLBACK SubclassFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);

	static WTW_PTR panelCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*);

	MenuIDList getMenuIDs();

	LRESULT onMainWindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT onStatusWindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT onListWindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	struct Colors
	{
		~Colors();

		Gdiplus::Color background;
		Gdiplus::Color activeTabBackgroundUp;
		Gdiplus::Color activeTabBackgroundDown;
		Gdiplus::Color activeTabTextColor;
		Gdiplus::Color inactiveTabBackgroundUp;
		Gdiplus::Color inactiveTabBackgroundDown;
		Gdiplus::Color inactiveTabTextColor;

		void reload();
		void clear();
	}colors;

	struct WTWWindows 
	{
		HWND main;
		HWND list;
		HWND status;
	}wtwWnd;

	HHOOK wndHook;
	HFONT font;
	Gdiplus::StringFormat textFormat;

	static TabControl* __instance;
};

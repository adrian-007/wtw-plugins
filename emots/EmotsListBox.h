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

#ifndef EMOTS_LIST_BOX_H
#define EMOTS_LIST_BOX_H

#include <unordered_map>

#include "Thread.hpp"

#define IDC_EMOTS_LIST 2000
typedef std::tr1::unordered_map<std::wstring, std::wstring> StringMap;

class EmotsListBox : public CWindowImpl<EmotsListBox>, private Thread {
public:
	BEGIN_MSG_MAP(EmotsListBox)
		MESSAGE_HANDLER(WM_CREATE, onCreate)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		MESSAGE_HANDLER(WM_ACTIVATE, onKillFocus)
		NOTIFY_HANDLER(IDC_EMOTS_LIST, NM_DBLCLK, onDoubleClick)
		//REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	__declspec(noinline) 
	void setPreviewImages(StringMap* img)
	{
		join();
		images = img;
		runThread();
	}

	__forceinline 
	void clear() 
	{
		m_List.DeleteAllItems();
	}

	
	__forceinline void hide()
	{
		ShowWindow(SW_HIDE);
	}
	
	void show(const CPoint& pt, HWND edit);
private:
	__declspec(noinline) 
	void addItem(const wchar_t* str)
	{
		int pos = m_List.GetItemCount();
		m_List.AddItem(pos, 0, str, pos);
	}

	__declspec(noinline)
	void updateImages()
	{
		m_List.SetImageList(m_Images, LVSIL_NORMAL);
		m_List.SetImageList(m_Images, LVSIL_SMALL);

		CRect rc;
		GetClientRect(&rc);
		SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	LRESULT onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDoubleClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	
	__declspec(noinline)
	LRESULT onKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(LOWORD(wParam) == 0)
		{
			hide();
			if(hEdit)
			{
				::SetFocus(hEdit);
			}
		}
		bHandled = FALSE;
		return 0;
	}

	void threadMain();

	CListViewCtrl m_List;
	CImageList m_Images;
	StringMap* images;

	HWND hEdit;
	HFONT font;
};

#endif

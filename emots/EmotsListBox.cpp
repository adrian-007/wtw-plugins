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
#include "Emots.hpp"
#include "EmotsListBox.h"
#include "../utils/windows.hpp"
#include "../utils/text.hpp"

#include <map>
#include <gdiplus.h>
using namespace Gdiplus;

#define SCI_ADDTEXT 2001

LRESULT EmotsListBox::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rc;
	GetClientRect(&rc);
	m_List.Create(m_hWnd, rc, 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_ICON | LVS_SINGLESEL, WS_EX_CLIENTEDGE, IDC_EMOTS_LIST);

	font = utils::windows::getSystemFont();
	utils::windows::SetExplorerTheme(m_List.m_hWnd);

	m_List.SetFont(font);
	SetFont(font);
	return 0;
}

LRESULT EmotsListBox::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	join();
	m_List.DestroyWindow();
	m_Images.Destroy();
	DeleteObject(font);
	return 0;
}

LRESULT EmotsListBox::onDoubleClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	int i = m_List.GetSelectedIndex();
	
	if(i >= 0) 
	{
		std::wstring buf;
		buf.resize(2048);
		buf.resize(m_List.GetItemText(i, 0, &buf[0], buf.size()));

		std::string text = utils::text::fromWide(buf);
		SendMessage(hEdit, SCI_ADDTEXT, text.size(), (LPARAM)text.c_str());
	}

	hide();
	::SetFocus(hEdit);
	return 0;
}

void EmotsListBox::show(const CPoint& pt, HWND edit)
{
	if(!edit)
	{
		return;
	}

	hEdit = edit;

	CRect rc;
	int cx = 0;
	int cy = 0;

	for(int i = 0; i < m_List.GetItemCount(); ++i)
	{
		if(m_List.GetItemRect(i, &rc, LVIR_BOUNDS))
		{
			cx = max(cx, rc.Width());
			cy = max(cy, rc.Height());
		}
	}

	m_List.SetIconSpacing(cx, cy);

	cx *= cfgGetInt(L"preview_x", 3);
	cy *= cfgGetInt(L"preview_y", 2);

	cx += GetSystemMetrics(SM_CXVSCROLL) + 5;

	SetWindowPos(0, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
	
	GetWindowRect(&rc);
	SetWindowPos(0, pt.x - (rc.Width() / 2) + 16, pt.y - 32 - rc.Height(), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	
	m_List.SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	m_List.Arrange(LVA_ALIGNTOP);

	SetFocus();
	m_List.SelectItem(-1);
}

void EmotsListBox::threadMain()
{
	typedef std::map<std::wstring, Image*> ImageMap;
	m_List.DeleteAllItems();
	m_Images.Destroy();

	if(images == 0)
	{
		return;
	}

	if(images->empty()) 
	{
		delete images;
		images = 0;
		return;
	}

	ImageMap map;
	unsigned int cx = 0, cy = 0;

	for(StringMap::const_iterator i = images->begin(); i != images->end(); ++i)
	{
		Image* img = Image::FromFile(i->second.c_str());

		cx = max(cx, img->GetWidth());
		cy = max(cy, img->GetHeight());

		map.insert(std::make_pair(i->first, img));
	}

	this->m_Images.Create(cx, cy, ILC_MASK | ILC_COLOR32, map.size(), 1);

	SIZE ss;
	m_Images.GetIconSize(ss);

	//__LOG_F(wtw, 1, L"imagelist", L"iconsize = %dx%d", ss.cx, ss.cy);

	float xOffset;
	float yOffset;
	HBITMAP hBitmap;

	for(ImageMap::iterator i = map.begin(); i != map.end(); ++i)
	{
		Image* img = i->second;
		
		xOffset = (float)(cx - img->GetWidth());
		yOffset = (float)(cy - img->GetHeight());

		Bitmap dst(cx, cy);
		Graphics gs(&dst);
		gs.DrawImage(img, xOffset / 2.0f, yOffset / 2.0f);

		hBitmap = 0;
		Status status = dst.GetHBITMAP(Color(255,255,255), &hBitmap);

		if(status == Ok)
		{
			this->m_Images.Add(hBitmap);
		}
	}

	this->updateImages();
	this->m_List.EnableWindow(FALSE);
	this->m_List.SetRedraw(FALSE);

	for(ImageMap::iterator i = map.begin(); i != map.end(); ++i)
	{
		addItem(i->first.c_str());
		delete i->second;
	}

	this->m_List.SetRedraw(TRUE);
	this->m_List.EnableWindow(TRUE);
	this->m_List.Invalidate();
	this->m_List.UpdateWindow();

	delete images;
	images = 0;
}

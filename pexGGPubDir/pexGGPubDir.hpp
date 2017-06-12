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

#ifndef PEX_GG_PUB_DIR
#define PEX_GG_PUB_DIR

#include "resource.h"
#define WTW_MENU_ITEM_GGPUBDIR L"pexGGPubDir/show"

class pexGGPubDir : public CDialogImpl<pexGGPubDir>
{
	static pexGGPubDir* inst;
public:
	enum { IDD = IDD_DIALOG };
	
	pexGGPubDir(HINSTANCE hInstance);
	~pexGGPubDir();

	static void create(pexGGPubDir* m);
	static void destroy();
	static pexGGPubDir* instance();

private:
	BEGIN_MSG_MAP(pexGGPubDir)
		MESSAGE_HANDLER(WM_INITDIALOG, onCreate)
		MESSAGE_HANDLER(WM_CLOSE, onClose)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		COMMAND_ID_HANDLER(IDC_SEARCH, onSearch)
		COMMAND_ID_HANDLER(IDC_PREV, onPrev)
		COMMAND_ID_HANDLER(IDC_NEXT, onNext)
		NOTIFY_HANDLER(IDC_LIST, NM_DBLCLK, onItemDoubleClick)
	END_MSG_MAP()

	LRESULT onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT onItemDoubleClick(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

	void show();
	void setControlState(bool enabled);
	std::wstring getEditContent(int ctrlId) const;

	CListViewCtrl ctrlList;
	CComboBox ctrlRegion;
	CComboBox ctrlSex;

	static const wchar_t* regions[17];
	HFONT hFont;

	// WTW methods
	void doRequest();

	void parseData();
	void onProcessData(const wchar_t* fileId, wtwRawDataDef* data);
	void onRequestComplete(const wchar_t* fileId, wtwHttpStatus* status);

	static WTW_PTR onHttpService_callback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
	{
		wtwHttpEvent* event = (wtwHttpEvent*)wParam;
		pexGGPubDir* p = (pexGGPubDir*)ptr;

		if(event == 0 || p == 0)
		{
			return 0;
		}

		switch(event->event)
		{
		case WTW_HTTP_EVENT_DATA:
			{
				p->onProcessData(event->fileId, (wtwRawDataDef*)lParam);
				break;
			}
		case WTW_HTTP_EVENT_COMPLETE:
			{
				p->onRequestComplete(event->fileId, (wtwHttpStatus*)lParam);
				break;
			}
		default: 
			{
			}
		}

		return 0;
	}

	static WTW_PTR onMenu_callback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) 
	{
		wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;
		wtwMenuItemDef* menu = (wtwMenuItemDef*)wParam;
		pexGGPubDir* p = (pexGGPubDir*)ptr;

		if(nfo == 0 || menu == 0 || p == 0)
		{
			return 0;
		}

		p->show();

		return 0;
	}

	std::wstring dataBuffer;
	int offset;

	HINSTANCE hInstance;
};

#endif

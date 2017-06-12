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
#include "SettingsWindow.h"
#include "ItemDialog.h"
#include "../utils/windows.hpp"

#define PERCENT(value, percent) (value * percent / 100)

SettingsWindow::SettingsWindow() {
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hFont = utils::windows::getSystemFont();
}

SettingsWindow::~SettingsWindow() {
	DeleteObject(hBrush);
	DeleteObject(hFont);
	hBrush = 0;
	hFont = 0;
}

LRESULT SettingsWindow::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	list.Attach(GetDlgItem(IDC_LIST));
	list.InsertColumn(0, L"Regex", LVCFMT_LEFT, -1, 0);
	list.InsertColumn(1, L"Format", LVCFMT_LEFT, -1, 0);
	list.InsertColumn(2, L"Typ wiadomo�ci", LVCFMT_LEFT, 90, 0);
	list.InsertColumn(3, L"Ignoruj wielko�� znak�w", LVCFMT_LEFT, 35, 0);
	list.InsertColumn(4, L"Nie zmieniaj tre�ci wiadomo�ci", LVCFMT_LEFT, 35, 0);
	list.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	btnAdd.Attach(GetDlgItem(IDC_ADD));
	btnMod.Attach(GetDlgItem(IDC_MOD));
	btnDel.Attach(GetDlgItem(IDC_DEL));
	btnUp.Attach(GetDlgItem(IDC_UP));
	btnDown.Attach(GetDlgItem(IDC_DOWN));

	Replacer::ItemList items = Replacer::getInstance()->getItems();
	for(Replacer::ItemList::const_iterator i = items.begin(); i != items.end(); ++i) {
		InsertItem(*i);
	}	
	
	utils::windows::SetExplorerTheme(list.m_hWnd);
	utils::windows::SetChildFont(m_hWnd, hFont);

	return 0;
}

LRESULT SettingsWindow::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	list.Detach();
	btnAdd.Detach();
	btnMod.Detach();
	btnDel.Detach();
	btnUp.Detach();
	btnDown.Detach();
	return 0;
}

LRESULT SettingsWindow::onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) {
	CRect rc;
	list.GetClientRect(rc);
	list.SetRedraw(FALSE);

	int cxBtn = 80;
	int cyBtn = 24;
	const int cxList = LOWORD(lParam) - 20;
	const int cyList = HIWORD(lParam) - 40;

	int offset = 0;

	int x = rc.left;
	int y = rc.top;

	list.MoveWindow(x, y, cxList, cyList);

	const int cyOffset = y + cyList + 5;

	btnAdd.MoveWindow(x + (offset++ * (cxBtn + 5)), cyOffset, cxBtn, cyBtn);
	btnMod.MoveWindow(x + (offset++ * (cxBtn + 5)), cyOffset, cxBtn, cyBtn);
	btnDel.MoveWindow(x + (offset++ * (cxBtn + 5)), cyOffset, cxBtn, cyBtn);

	offset += 2;
	cxBtn -= 30;

	btnUp.MoveWindow(x + (offset++ * (cxBtn + 5)), cyOffset, cxBtn, cyBtn);
	btnDown.MoveWindow(x + (offset++ * (cxBtn + 5)), cyOffset, cxBtn, cyBtn);

	list.SetColumnWidth(0, PERCENT(cxList, 30));
	list.SetColumnWidth(1, PERCENT(cxList, 35));
	list.SetRedraw(TRUE);
	list.UpdateWindow();

	return 0;
}

LRESULT SettingsWindow::onAdd(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	ItemDialog dlg;
	if(dlg.DoModal(m_hWnd) == IDOK) {
		Replacer::Item* i = new Replacer::Item(dlg.item);
		InsertItem(i);
	}
	return 0;
}

LRESULT SettingsWindow::onModify(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	int pos = list.GetSelectedIndex();
	if(pos >= 0) {
		Replacer::Item& i = *(Replacer::Item*)list.GetItemData(pos);

		ItemDialog dlg(i);
		if(dlg.DoModal(m_hWnd) == IDOK) {
			i = dlg.item;
			list.DeleteItem(pos);
			InsertItem(&i, pos);
		}
	}
	return 0;
}

LRESULT SettingsWindow::onDelete(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	int pos = list.GetSelectedIndex();
	if(pos >= 0) {
		toDelete.push_back((Replacer::Item*)list.GetItemData(pos));
		list.DeleteItem(pos);
	}
	return 0;
}

LRESULT SettingsWindow::onMoveUp(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	int pos = list.GetSelectedIndex();
	if(pos > 0) {
		Replacer::Item* i = (Replacer::Item*)list.GetItemData(pos);
		list.DeleteItem(pos);
		InsertItem(i, pos-1);
	}
	return 0;
}

LRESULT SettingsWindow::onMoveDown(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	int pos = list.GetSelectedIndex();
	if(pos >= 0 && pos < list.GetItemCount()) {
		Replacer::Item* i = (Replacer::Item*)list.GetItemData(pos);
		list.DeleteItem(pos);
		InsertItem(i, pos+1);
	}
	return 0;
}

LRESULT SettingsWindow::onDbClick(int /*ctrlId*/, LPNMHDR /*pNMHDR*/, BOOL& bHandled) {
	onModify(0, 0, 0, bHandled);
	return 0;
}

void SettingsWindow::InsertItem(Replacer::Item* item, int pos /*= -1*/) {
	if(pos == -1)
		pos = list.GetItemCount();

	int i = list.InsertItem(pos, item->regex.c_str());
	list.SetItemText(i, 1, item->format.c_str());
	switch(item->direction) {
		case Replacer::Item::DIR_IN: {
			list.SetItemText(i, 2, L"Przychodz�ce");
			break;
		}
		case Replacer::Item::DIR_OUT: {
			list.SetItemText(i, 2, L"Wychodz�ce");
			break;
		}
		default: {
			list.SetItemText(i, 2, L"Wszystkie");
		}
	}
	list.SetItemText(i, 3, item->ignoreCase ? L"Tak" : L"Nie");
	list.SetItemText(i, 4, item->displayOnly ? L"Tak" : L"Nie");

	list.SetItemData(i, reinterpret_cast<DWORD_PTR>(item));
	list.SelectItem(i);
}

void SettingsWindow::close(bool save) throw() {
	Replacer::ItemList items;

	if(save) {
		for(int i = 0; i < list.GetItemCount(); ++i) {
			items.push_back((Replacer::Item*)list.GetItemData(i));
		}
		Replacer::getInstance()->setItems(items);
	}

	Replacer::getInstance()->reload();

	list.DeleteAllItems();
	for(Replacer::ItemList::const_iterator i = toDelete.begin(); i != toDelete.end(); ++i)
		delete *i;
	toDelete.clear();

	items = Replacer::getInstance()->getItems();
	for(Replacer::ItemList::const_iterator i = items.begin(); i != items.end(); ++i) {
		InsertItem(*i);
	}

}

WTW_PTR SettingsWindow::callback(WTW_PARAM wParam, WTW_PARAM lParam, void*) {
	wtwOptionPageShowInfo* info = (wtwOptionPageShowInfo*)wParam;

	wcscpy(info->windowCaption, L"wtwRegexReplacer");
    wcscpy(info->windowDescrip, L"Modyfikacja wiadomo�ci na podstawie wyra�e� regularnych");
	info->iconId = L"wtwRegexReplacer/icon32";

	SettingsWindow* pOptions = (SettingsWindow*)info->page->ownerData;

    switch(info->action) {
        case WTW_OPTIONS_PAGE_ACTION_SHOW: {
	        if(!pOptions) {
				pOptions = new SettingsWindow;
				info->page->ownerData = (void*)pOptions;

				pOptions->Create(info->handle);
			}
			pOptions->MoveWindow(info->x, info->y, info->cx, info->cy);
			pOptions->ShowWindow(SW_SHOW);
	        break;	 
        }
        case WTW_OPTIONS_PAGE_ACTION_HIDE: {
			if(pOptions)
				pOptions->ShowWindow(SW_HIDE);
	        break;
        }
		case WTW_OPTIONS_PAGE_ACTION_MOVE: {
			if(pOptions)
				pOptions->MoveWindow(info->x, info->y, info->cx, info->cy);
			break;
		}
        case WTW_OPTIONS_PAGE_ACTION_APPLY: {
			if(pOptions)
				pOptions->close(true);
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

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
#include "pexGGPubDir.hpp"

#include "../utils/text.hpp"
#include "../utils/windows.hpp"
#include "../utils/utils.hpp"

pexGGPubDir* pexGGPubDir::inst = 0;

void pexGGPubDir::create(pexGGPubDir* m)
{
	if(pexGGPubDir::inst != 0)
	{
		delete pexGGPubDir::inst;
	}

	pexGGPubDir::inst = m;
}

void pexGGPubDir::destroy()
{
	if(pexGGPubDir::inst != 0)
	{
		delete pexGGPubDir::inst;
		pexGGPubDir::inst = 0;
	}
}

pexGGPubDir* pexGGPubDir::instance()
{
	return pexGGPubDir::inst;
}

const wchar_t* pexGGPubDir::regions[17] =
{
	L"Nie podano",
	L"Dolno�l�skie",
	L"Kujawsko-pomorskie",
	L"Lubelskie",
	L"Lubuskie",
	L"��dzkie",
	L"Ma�opolskie",
	L"Mazowieckie",
	L"Opolskie",
	L"Podkarpackie",
	L"Podlaskie",
	L"Pomorskie",
	L"�l�skie",
	L"�wi�tokrzyskie",
	L"Warmi�sko-mazurskie",
	L"Wielkopolskie",
	L"Zachodniopomorskie"
};

pexGGPubDir::pexGGPubDir(HINSTANCE hInstance) : hFont(0)
{
	this->hInstance = hInstance;

	{
		wtwMenuItemDef mi;
		initStruct(mi);
		mi.menuID		= WTW_MENU_ID_MAIN_OPT;
		mi.hModule		= hInstance;
		mi.callback		= pexGGPubDir::onMenu_callback;
		mi.cbData		= this;

		mi.itemId		= WTW_MENU_ITEM_GGPUBDIR;
		mi.menuCaption	= L"Katalog publiczny GG";
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
	}
}

pexGGPubDir::~pexGGPubDir()
{
	wtw->fnCall(WTW_MENU_ITEM_CLEAR, reinterpret_cast<WTW_PARAM>(hInstance), 0);
	
	if(IsWindow())
	{
		DestroyWindow();
	}
}

LRESULT pexGGPubDir::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(hFont == 0)
	{
		hFont = utils::windows::getSystemFont();
	}

	ctrlList.Attach(GetDlgItem(IDC_LIST));
	ctrlList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	utils::windows::SetExplorerTheme(ctrlList.m_hWnd);

	ctrlList.InsertColumn(0, L"Numer", LVCFMT_LEFT, 100, 0);
	ctrlList.InsertColumn(1, L"Nazwa", LVCFMT_LEFT, 200, 0);
	ctrlList.InsertColumn(2, L"Wiek", LVCFMT_LEFT, 80, 0);
	ctrlList.InsertColumn(3, L"P�e�", LVCFMT_LEFT, 80, 0);
	ctrlList.InsertColumn(4, L"Miejscowo��", LVCFMT_LEFT, 200, 0);

	ctrlRegion.Attach(GetDlgItem(IDC_REGION));
	
	for(unsigned i = 0; i < 17; ++i)
	{
		ctrlRegion.AddString(regions[i]);
	}

	ctrlSex.Attach(GetDlgItem(IDC_SEX));
	ctrlSex.AddString(L"Nie podano");
	ctrlSex.AddString(L"Kobieta");
	ctrlSex.AddString(L"M�czyzna");

	ctrlRegion.SetCurSel(0);
	ctrlSex.SetCurSel(0);

	///////////////////////////////////////////////////////
	utils::windows::SetChildFont(m_hWnd, hFont);
	return 0;
}

LRESULT pexGGPubDir::onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DestroyWindow();
	return 0;
}

LRESULT pexGGPubDir::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ctrlList.Detach();
	ctrlRegion.Detach();
	ctrlSex.Detach();

	if(hFont != 0)
	{
		DeleteObject(hFont);
		hFont = 0;
	}

	return 0;
}

LRESULT pexGGPubDir::onSearch(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */)
{
	offset = 0;
	doRequest();
	return 0;
}

LRESULT pexGGPubDir::onPrev(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */)
{
	offset -= 15;

	if(offset < 0)
	{
		offset = 0;
	}

	doRequest();
	return 0;
}

LRESULT pexGGPubDir::onNext(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */)
{
	offset += 15;
	doRequest();
	return 0;
}

void pexGGPubDir::show()
{
	if(IsWindow() == TRUE)
	{
		SetActiveWindow();
	}
	else
	{
		Create(::GetDesktopWindow());
		ShowWindow(SW_SHOW);
	}
}

std::wstring pexGGPubDir::getEditContent(int ctrlId) const
{
	std::wstring ret;
	CEdit edit;
	edit.Attach(GetDlgItem(ctrlId));
	ret.resize(edit.GetWindowTextLength() + 1);
	ret.resize(edit.GetWindowText(&ret[0], ret.size()));
	return ret;
}

void pexGGPubDir::setControlState(bool enabled)
{
	if(IsWindow() == FALSE)
	{
		return;
	}

	BOOL state = enabled ? 1 : 0;

	GetDlgItem(IDC_SEARCH).EnableWindow(state);
	GetDlgItem(IDC_NEXT).EnableWindow(state);
	GetDlgItem(IDC_PREV).EnableWindow(state);
}

void pexGGPubDir::doRequest()
{
	if(IsWindow() == FALSE)
	{
		return;
	}
	setControlState(false);
	dataBuffer.clear();
	ctrlList.DeleteAllItems();

//http://ipubdir.gadu-gadu.pl/ngg/?name=&surname=&province_option_id=0&city=&street=&gender=0&ageFrom=&ageTo=&uin=9273815&x=22&y=9&form_type=2

	std::string get;
	std::wstring buf;

	{
		get += "name=";
		buf = getEditContent(IDC_NAME);
		get += utils::text::urlEscape(utils::text::fromWide(buf));
	}
	{
		get += "&surname=";
		buf = getEditContent(IDC_SURNAME);
		get += utils::text::urlEscape(utils::text::fromWide(buf));
	}
	{
		get += "&province_option_id=" + utils::conv::toString((unsigned int)ctrlRegion.GetCurSel());
	}
	{
		get += "&city=";
		buf = getEditContent(IDC_CITY);
		get += utils::text::urlEscape(utils::text::fromWide(buf));
	}
	{
		get += "&street=";
		buf = getEditContent(IDC_STREET);
		get += utils::text::urlEscape(utils::text::fromWide(buf));
	}
	{
		get += "&gender=" + utils::conv::toString((unsigned int)ctrlSex.GetCurSel());
	}
	{
		unsigned int age = GetDlgItemInt(IDC_AGE_MIN);
		get += "&ageFrom=";
		if(age > 0)
		{
			get += utils::conv::toString(age);
		}
	}
	{
		unsigned int age = GetDlgItemInt(IDC_AGE_MAX);
		get += "&ageTo=";
		if(age > 0)
		{
			get += utils::conv::toString(age);
		}	
	}
	{
		unsigned int uin = GetDlgItemInt(IDC_GGNUMBER);
		get += "&uin=";

		if(uin > 0)
		{
			get += utils::conv::toString(uin);
		}
	}
	{
		get += "&form_type=2";
	}
	{
		if(offset >= 0)
		{
			get += "&offset=" + utils::conv::toString((unsigned int)offset);
		}
	}

	wtwHttpGetFile file;
	file.callback = pexGGPubDir::onHttpService_callback;
	file.cbData = this;
	file.fileId = L"pubDir";
	file.flags = WTW_HTTP_SERVICE_WRITE_TO_CB | WTW_HTTP_SERVICE_ASYNCHRONOUS | WTW_HTTP_SERVICE_NO_URI_ESCAPE;

	std::wstring url = L"http://ipubdir.gadu-gadu.pl/ngg/?";
	url += utils::text::toWide(get);

	file.uri = url.c_str();
	file.headers = L"Content-Type: text/html; charset=utf-8\r\n";
	//file.methodData = get.c_str();
	//file.methodDataSize = get.size();

	wtw->fnCall(WTW_HTTP_GET_FILE, file, 0);
}

void pexGGPubDir::onProcessData(const wchar_t* fileId, wtwRawDataDef* data)
{
	if(data && data->pDataLen > 0)
	{
		dataBuffer += utils::text::toWide(std::string(data->pData, data->pDataLen));
//		::OutputDebugStringA(std::string(data->pData, data->pDataLen).c_str());
//		::OutputDebugStringA("\n");
	}
}

void pexGGPubDir::onRequestComplete(const wchar_t* fileId, wtwHttpStatus* status)
{
	__LOG_F(wtw, WTW_LOG_LEVEL_DEBUG, L"PXPD", L"status = 0x%04X, response = 0x%04X", status->status, status->responseCode);

	setControlState(true);
	parseData();

	int cnt = ctrlList.GetItemCount();

	if(cnt < 15)
	{
		GetDlgItem(IDC_NEXT).EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_NEXT).EnableWindow(TRUE);
	}

	if(offset < 15)
	{
		GetDlgItem(IDC_PREV).EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_PREV).EnableWindow(TRUE);
	}
}

void pexGGPubDir::parseData()
{
	if(dataBuffer.empty())
	{
		return;
	}

	typedef std::wstring::size_type size_type;

	size_type startTable, endTable, rowStart, rowEnd, colStart, colEnd;
	
	startTable = dataBuffer.find(L"<table");
	if(startTable == std::wstring::npos)
	{
		return;
	}

	endTable = dataBuffer.find(L"</table>", startTable);
	if(endTable == std::wstring::npos)
	{
		return;
	}

	std::wstring table = dataBuffer.substr(startTable, endTable - startTable);

	rowStart = 0;
	rowEnd = table.find(L"</tr>");

	while((rowStart = table.find(L"<tr", rowEnd)) != std::wstring::npos && (rowEnd = table.find(L"</tr>", rowStart)) != std::wstring::npos)
	{
		std::wstring row = table.substr(rowStart, rowEnd - rowStart);
		std::wstring col;

		colStart = 0;
		colEnd = 0;

		int pos = ctrlList.AddItem(ctrlList.GetItemCount(), 0, 0);

		{ // numer w linku
			colStart = row.find(L"<td", colEnd);
			colStart = row.find(L">", colStart);
			++colStart;
			colEnd = row.find(L"</td>", colStart);

			col = row.substr(colStart, colEnd - colStart);
			utils::text::trimSpaces(col);
			
			size_type tmp = colEnd;

			colStart = col.find(L"<a href=\"gg:");
			colStart += 12;
			colEnd = col.find(L"\"", colStart);
			
			col = col.substr(colStart, colEnd - colStart);
			utils::text::urlUnescapeW(col);

			ctrlList.SetItemText(pos, 0, col.c_str());
			colEnd = tmp;
		}

		{ // nazwa
			colStart = row.find(L"<td", colEnd);
			colStart = row.find(L">", colStart);
			++colStart;
			colEnd = row.find(L"</td>", colStart);

			col = row.substr(colStart, colEnd - colStart);
			utils::text::trimSpaces(col);
			utils::text::urlUnescapeW(col);
			ctrlList.SetItemText(pos, 1, col.c_str());
		}

		{ // avatar
			colStart = row.find(L"<td", colEnd);
			colStart = row.find(L">", colStart);
			++colStart;
			colEnd = row.find(L"</td>", colStart);

			col = row.substr(colStart, colEnd - colStart);
			utils::text::trimSpaces(col);
			//ctrlList.SetItemText(pos, 2, col.c_str());
		}
		{ // wiek
			colStart = row.find(L"<td", colEnd);
			colStart = row.find(L">", colStart);
			++colStart;
			colEnd = row.find(L"</td>", colStart);

			col = row.substr(colStart, colEnd - colStart);
			utils::text::trimSpaces(col);
			utils::text::urlUnescapeW(col);
			ctrlList.SetItemText(pos, 2, col.c_str());
		}
		{ // plec
			colStart = row.find(L"<td", colEnd);
			colStart = row.find(L">", colStart);
			++colStart;
			colEnd = row.find(L"</td>", colStart);

			col = row.substr(colStart, colEnd - colStart);
			utils::text::trimSpaces(col);

			if(col == L"K")
			{
				col = L"Kobieta";
			}
			else if(col == L"M")
			{
				col = L"M�czyzna";
			}
			else
			{
				col = L"Nie podano";
			}

			ctrlList.SetItemText(pos, 3, col.c_str());
		}
		{ // miejscowosc
			colStart = row.find(L"<td", colEnd);
			colStart = row.find(L">", colStart);
			++colStart;
			colEnd = row.find(L"</td>", colStart);

			col = row.substr(colStart, colEnd - colStart);
			utils::text::trimSpaces(col);
			utils::text::urlUnescapeW(col);
			ctrlList.SetItemText(pos, 4, col.c_str());
		}
	}
}

LRESULT pexGGPubDir::onItemDoubleClick(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	return 0;
}

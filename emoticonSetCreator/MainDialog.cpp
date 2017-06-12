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
#include "MainDialog.hpp"

#include "XMLWriter.hpp"
#include "WinApiFile.hpp"

LRESULT MainDialog::onCreateDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	ctrlList.Attach(GetDlgItem(IDC_LIST));
	ctrlPath.Attach(GetDlgItem(IDC_PATH));
	ctrlCase.Attach(GetDlgItem(IDC_ICASE));

	ctrlList.ModifyStyle(0, LVS_SINGLESEL);
	ctrlList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	ctrlList.InsertColumn(0, L"regex", LVCFMT_LEFT, 100, 0);
	ctrlList.InsertColumn(1, L"match", LVCFMT_LEFT, 100, 0);
	ctrlList.InsertColumn(2, L"desc", LVCFMT_LEFT, 100, 0);
	ctrlList.InsertColumn(3, L"icase", LVCFMT_LEFT, 40, 0);
	ctrlList.InsertColumn(4, L"file", LVCFMT_LEFT, 160, 0);

	return 0;
}

LRESULT MainDialog::onDestroyDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	ctrlList.Detach();
	ctrlPath.Detach();
	ctrlCase.Detach();
	return 0;
}

LRESULT MainDialog::onEndDialog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}

LRESULT MainDialog::onFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	std::wstring filePath;

	if(WTL::RunTimeHelper::IsVista()) {
		IFileDialog* pfd;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if(SUCCEEDED(hr)) {
			DWORD opt;
			hr = pfd->GetOptions(&opt);
			if(SUCCEEDED(hr)) {
				pfd->SetOptions(opt | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
			}
			
			COMDLG_FILTERSPEC files;
			files.pszName = L"Obrazy";
			files.pszSpec = L"*.jpg;*.jpeg;*.png;*.gif";

			pfd->SetFileTypes(1, &files);

			hr = pfd->Show(0);
			if (SUCCEEDED(hr)) {
				IShellItem *psiResult;
				hr = pfd->GetResult(&psiResult);
				if (SUCCEEDED(hr)) {
					LPOLESTR pwsz = NULL;

					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
					if (SUCCEEDED(hr)) {
						filePath = pwsz;
						CoTaskMemFree(pwsz);
					}
					psiResult->Release();
				}
			}
			pfd->Release();
		}
	} else {
		CFileDialog fd(TRUE, 0, 0, 4 | 2, L"Obrazy\0*.jpg;*.jpeg;*.png;*.gif\0");
		if(fd.DoModal() == IDOK) {
			filePath = fd.m_szFileName;
		}
	}

	if(filePath.size()) {
		ctrlPath.SetWindowText(filePath.c_str());
	}
	return 0;
}

LRESULT MainDialog::onGenerate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	using std::wstring;

	if(ctrlList.GetItemCount() == 0) {
		MessageBox(L"Lista emotikon jest pusta!");
		return 0;
	}
	
	wstring dirName = getText(IDC_DESTINATION);
	wstring name = getText(IDC_SET_NAME);
	wstring desc = getText(IDC_SET_DESC);

	{
		wstring::size_type i = 0;
		while((i = dirName.find_first_of(L" -/\\;.,+=_()*&^%$#@!:\"'\n\t\r", i)) != wstring::npos) {
			dirName.erase(i, 1);
		}
	}

	if(dirName.empty()) {
		MessageBox(L"Musisz poda� nazw� folderu docelowego!");
		return 0;
	}

	wstring rootPath;
	wstring dirPath;

	{
		wchar_t buf[MAX_PATH+1] = { 0 };
		GetModuleFileName(NULL, buf, MAX_PATH);
		rootPath = buf;

		wstring::size_type i = rootPath.rfind(L"\\");
		rootPath = rootPath.substr(0, i+1);
		dirPath = rootPath + dirName;
	}

	::CreateDirectory(dirPath.c_str(), NULL);

	xml::writer::element e("wtwEmots");
	e.addAttrib("author", fromWide(name));
	e.addAttrib("desc", fromWide(desc));
	
	wchar_t buf[4096] = { 0 };
	wstring file;

	for(int i = 0; i < ctrlList.GetItemCount(); ++i) {
		xml::writer::element& c = e.getChild("item");
		
		ctrlList.GetItemText(i, 0, buf, 4095);
		c.addAttrib("regex", fromWide(buf));

		ctrlList.GetItemText(i, 1, buf, 4095);
		c.addAttrib("match", fromWide(buf));

		ctrlList.GetItemText(i, 2, buf, 4095);
		c.addAttrib("desc", fromWide(buf));

		ctrlList.GetItemText(i, 3, buf, 4095);
		c.addAttrib("icase", fromWide(buf));

		ctrlList.GetItemText(i, 4, buf, 4095);
		file = buf;
		{
			wstring::size_type j = file.rfind(L"\\");
			file = file.substr(j);
		}
		c.addAttrib("file", fromWide(dirName + file));
		file = rootPath + dirName + file;
		::CopyFile(buf, file.c_str(), FALSE);
	}

	file = rootPath + dirName + L".xml";

	WinApiFile f;
	f.open(file, WinApiFile::WRITE_MODE, WinApiFile::ALWAYS_CREATE);
	unsigned char smarker[3];
	smarker[0] = 0xEF;
	smarker[1] = 0xBB;
	smarker[2] = 0xBF;
	f.write(smarker, sizeof(unsigned char) * sizeof(smarker));
	
	std::string xmlFile = e.getUtf8Header();
	e.print(xmlFile);
	f << xmlFile;
	f.close();

	MessageBox(L"Poprawnie wygenerowano zestaw!");
	return 0;
}

LRESULT MainDialog::onAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	using std::wstring;
	wstring regex = getText(IDC_REGEX);
	wstring match = getText(IDC_MATCH);
	wstring desc = getText(IDC_DESC);
	wstring icase = ctrlCase.GetCheck() ? L"1" : L"0";
	wstring path = getText(IDC_PATH);

	if(regex.empty()) {
		MessageBox(L"Brak wyra�enia regularnego");
		return 0;
	}
	if(path.empty()) {
		MessageBox(L"Nie wybrano �adnego pliku");
		return 0;
	}

	int subItem = 0;
	int i = ctrlList.AddItem(ctrlList.GetItemCount(), subItem++, regex.c_str());
	ctrlList.SetItemText(i, subItem++, match.c_str());
	ctrlList.SetItemText(i, subItem++, desc.c_str());
	ctrlList.SetItemText(i, subItem++, icase.c_str());
	ctrlList.SetItemText(i, subItem++, path.c_str());

	return 0;
}

LRESULT MainDialog::onRem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int i = ctrlList.GetSelectedIndex();
	if(i >= 0) {
		ctrlList.DeleteItem(i);
	}
	return 0;
}

std::wstring MainDialog::getText(int id) {
	std::wstring buf;
	buf.resize(::GetWindowTextLength(GetDlgItem(id)) + 1);
	memset(&buf[0], 0, buf.size() * sizeof(wchar_t));
	buf.resize(::GetWindowText(GetDlgItem(id), &buf[0], buf.size()));
	return buf;
}

std::string MainDialog::fromWide(const std::wstring& str) {
	std::string ret;
	if(str.empty() == false) {
		int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, 0, 0, 0, 0);
		if(size > 0) {
			ret.resize(size);
			size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &ret[0], (int)ret.size(), 0, 0);
			if(size == 0) {
				ret.clear();
			}
		}
	}
	return ret;
}

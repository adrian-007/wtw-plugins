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
#include "..\sounds\EventDispatcher.hpp"
#include "..\utils\windows.hpp"

class Dummy { };
typedef EventDispatcher<Dummy> SoundEvents;

LRESULT MainDialog::onCreateDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	hFont = utils::windows::getSystemFont();

	ctrlList.Attach(GetDlgItem(IDC_LIST));
	ctrlList.ModifyStyle(0, LVS_SINGLESEL);
	ctrlList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	ctrlList.InsertColumn(0, L"Typ", LVCFMT_LEFT, 200, 0);
	ctrlList.InsertColumn(1, L"Plik", LVCFMT_LEFT, 600, 0);

	ctrlList.EnableGroupView(TRUE);
	utils::windows::SetExplorerTheme(ctrlList.m_hWnd);

	LVGROUP gr = { 0 };
	gr.cbSize = sizeof(LVGROUP);
	gr.mask = LVGF_GROUPID | LVGF_HEADER;

	gr.pszHeader = L"Wiadomo�ci";
	gr.cchHeader = wcslen(gr.pszHeader);
	gr.iGroupId = 0;
	ctrlList.AddGroup(&gr);

	gr.pszHeader = L"Zmiana statusu";
	gr.cchHeader = wcslen(gr.pszHeader);
	gr.iGroupId = 1;
	ctrlList.AddGroup(&gr);

	gr.pszHeader = L"Rozmowy audio/wideo";
	gr.cchHeader = wcslen(gr.pszHeader);
	gr.iGroupId = 2;
	ctrlList.AddGroup(&gr);

	gr.pszHeader = L"R�ne";
	gr.cchHeader = wcslen(gr.pszHeader);
	gr.iGroupId = 3;
	ctrlList.AddGroup(&gr);

	LVITEM lvItem = { 0 };
	lvItem.mask = LVIF_GROUPID;

	for(int i = 0; i < SoundEvents::EVENT_LAST; ++i) {
		ctrlList.AddItem(i, 0, SoundEvents::getEventName(i).c_str());

		lvItem.iGroupId = SoundEvents::getGroupIndex(i);

		lvItem.iItem = i;
		ctrlList.SetItem(&lvItem);
	}

	CComboBox combo;
	combo.Attach(GetDlgItem(IDC_SOUND_TYPE));
	combo.AddString(L"Plik z d�wi�kiem");
	combo.AddString(L"D�wi�k systemowy: %beep%");
	combo.AddString(L"D�wi�k systemowy: %warning%");
	combo.AddString(L"D�wi�k systemowy: %error%");
	combo.AddString(L"D�wi�k systemowy: %info%");
	combo.SetCurSel(0);
	combo.Detach();

	utils::windows::SetChildFont(m_hWnd, hFont);

	DlgResize_Init(true, true, WS_THICKFRAME);

	return 0;
}

LRESULT MainDialog::onDestroyDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	ctrlList.Detach();

	::DeleteObject(hFont);
	return 0;
}

LRESULT MainDialog::onEndDialog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}

LRESULT MainDialog::onFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int pos = ctrlList.GetSelectedIndex();
	if(pos < 0) {
		MessageBoxW(L"Musisz zaznaczy� na li�cie element do kt�rego chcesz przypisa� plik!", L"B��d", MB_ICONERROR);
		return 0;
	}
	
	std::wstring filePath;

	CComboBox combo;
	combo.Attach(GetDlgItem(IDC_SOUND_TYPE));

	switch(combo.GetCurSel())
	{
	case 1: // %beep%
		{
			filePath = L"%beep%";
			break;
		}
	case 2: // %warning%
		{
			filePath = L"%warning%";
			break;
		}
	case 3: // %error%
		{
			filePath = L"%error%";
			break;
		}
	case 4: // %info%
		{
			filePath = L"%info%";
			break;
		}
	default:
		{
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
					files.pszName = L"D�wi�ki";
					files.pszSpec = L"*.mp3;*.wav";

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
				CFileDialog fd(TRUE, 0, 0, 4 | 2, L"D�wi�ki\0*.mp3;*.wav\0");
				if(fd.DoModal() == IDOK) {
					filePath = fd.m_szFileName;
				}
			}
			break;
		}
	}

	combo.Detach();

	if(filePath.size())
	{
		ctrlList.SetItemText(pos, 1, filePath.c_str());
	}
	return 0;
}

LRESULT MainDialog::onGenerate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	using std::wstring;

	wstring dirName = getText(IDC_DESTINATION);
	wstring name = getText(IDC_SET_NAME);
	wstring desc = getText(IDC_SET_DESC);

	{
		//wstring::size_type i = 0;
		//while((i = dirName.find_first_of(L" -/\\;.,+=_()*&^%$#@!:\"'\n\t\r", i)) != wstring::npos) {
		//	dirName.erase(i, 1);
		//}
		for(size_t i = 0; i < dirName.length(); ++i) {
			if(iswalnum(dirName[i]) == 0) {
				dirName[i] = L'_';
			}
		}
	}

	if(dirName.empty()) {
		MessageBox(L"Musisz poda� nazw� folderu docelowego!", L"B��d", MB_ICONERROR);
		return 0;
	} else {
		SetDlgItemText(IDC_DESTINATION, dirName.c_str());
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

	if(::CreateDirectory(dirPath.c_str(), NULL) == FALSE) {
		MessageBox(L"Program nie m�g� utworzy� katalogu docelowego (prawopodobnie nie ma uprawnie�). Spr�buj przenie�� aplikacj� do innego katalogu i uruchom j� ponownie.", L"B��d", MB_ICONERROR);
		return 0;
	}

	xml::writer::element e("wtwSounds");
	e.addAttrib("author", fromWide(name));
	e.addAttrib("description", fromWide(desc));
	
	wchar_t buf[4096] = { 0 };
	wstring file;

	for(int i = 0; i < ctrlList.GetItemCount(); ++i) {
		xml::writer::element& c = e.getChild("item");

		c.addAttrib("type", fromWide(SoundEvents::getEventSettingName(i)));

		ctrlList.GetItemText(i, 1, buf, 4095);
		file = buf;
		
		if(file.length() > 0) {
			if(file[0] == L'%')
			{
				c.addAttrib("file", fromWide(file));
			}
			else
			{
				wstring::size_type j = file.rfind(L"\\");
				file = file.substr(j);

				c.addAttrib("file", fromWide(dirName + file));
				file = rootPath + dirName + file;
				::CopyFile(buf, file.c_str(), FALSE);
			}
		} else {
			c.addAttrib("file", "");
		}
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

	MessageBox(L"Poprawnie wygenerowano zestaw!", L"Sounds Set Creator", MB_ICONINFORMATION);
	return 0;
}

LRESULT MainDialog::onRem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int i = ctrlList.GetSelectedIndex();
	if(i >= 0) {
		ctrlList.SetItemText(i, 1, L"");
	} else {
		MessageBoxW(L"Musisz zaznaczy� na li�cie element w kt�rym chcesz usun�� plik!", L"B��d", MB_ICONERROR);
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

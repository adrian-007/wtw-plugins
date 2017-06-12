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
#include "resource.h"

#include "wtwAvatars.hpp"
#include "../utils/WinApiFile.hpp"

#include "../utils/wtw.hpp"

wtwAvatars* wtwAvatars::inst = 0;

#define MENU_ITEM_SET L"wtwAvatars/set"
#define MENU_ITEM_CLR L"wtwAvatars/clr"

wtwAvatars::wtwAvatars(HINSTANCE hInstance) 
{
	this->hInstance = hInstance;
	
	wtw_t::preloadIcon(wtw, hInstance, L"wtwAvatars/icon16", L"wtwAvatars16.png", IDB_PNG1);
	wtw_t::preloadIcon(wtw, hInstance, L"wtwAvatars/icon32", L"wtwAvatars32.png", IDB_PNG2);

	wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, filePath);

	filePath += L"wtwAvatars.bin";

	WinApiFile file(filePath, WinApiFile::READ_MODE);
	if(file.valid()) 
	{
		wchar_t uuid[256];
		wchar_t path[2048];

		while(file.good()) 
		{
			memset(uuid, 0, sizeof(uuid));
			memset(path, 0, sizeof(path));

			file.read(uuid, sizeof(uuid));
			file.read(path, sizeof(path));

			avatars[uuid] = path;
		}
	}

	wtwMenuItemDef mi;

	mi.menuID		= WTW_MENU_ID_CONTACT;
	mi.hModule		= ghInstance;
	mi.callback		= &wtwAvatars::MenuCallback;
	mi.cbData		= (void*)this;
	mi.iconId		= L"wtwAvatars/icon16";	

	mi.itemId		= MENU_ITEM_SET;
	mi.menuCaption	= L"Ustaw avatar";
	wtw->fnCall(WTW_MENU_ITEM_ADD, mi, 0);

	mi.itemId		= MENU_ITEM_CLR;
	mi.menuCaption	= L"Usu� avatar";
	wtw->fnCall(WTW_MENU_ITEM_ADD, mi, 0);

	menuRebuildHook = wtw->evHook(WTW_EVENT_MENU_REBUILD, &wtwAvatars::MenuRebuild, (void*)this);
	cntAddHook = wtw->evHook(WTW_EVENT_ON_CONTACT_ADD, wtwAvatars::func_onContactAdded, (void*)this);
}

wtwAvatars::~wtwAvatars()
{
	if(cntAddHook)
	{
		wtw->evUnhook(cntAddHook);
		cntAddHook = 0;
	}
	if(menuRebuildHook) 
	{
		wtw->evUnhook(menuRebuildHook);
		menuRebuildHook = 0;
	}

	wchar_t uuid[256];
	wchar_t path[2048];

	WinApiFile file(filePath, WinApiFile::WRITE_MODE, CREATE_ALWAYS);
	if(file.valid()) 
	{
		for(StringMap::const_iterator i = avatars.begin(); i != avatars.end(); ++i) 
		{
			memset(uuid, 0, sizeof(uuid));
			memset(path, 0, sizeof(path));

			wcscpy(uuid, i->first.c_str());
			wcscpy(path, i->second.c_str());

			file.write(uuid, sizeof(uuid));
			file.write(path, sizeof(path));
		}
	}
}

std::wstring wtwAvatars::getFile() const 
{
	std::wstring filePath;

	if(WTL::RunTimeHelper::IsVista()) 
	{
		IFileDialog* pfd;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if(SUCCEEDED(hr)) 
		{
			DWORD opt;
			hr = pfd->GetOptions(&opt);

			if(SUCCEEDED(hr)) 
			{
				pfd->SetOptions(opt | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
			}

			COMDLG_FILTERSPEC files;
			files.pszName = L"Obrazy";
			files.pszSpec = L"*.png;*.jpg;*.bmp;*.ico";

			pfd->SetFileTypes(1, &files);

			hr = pfd->Show(0);

			if(SUCCEEDED(hr))
			{
				IShellItem *psiResult;
				hr = pfd->GetResult(&psiResult);

				if(SUCCEEDED(hr)) 
				{
					LPOLESTR pwsz = NULL;
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

					if(SUCCEEDED(hr)) 
					{
						filePath = pwsz;
						CoTaskMemFree(pwsz);
					}

					psiResult->Release();
				}
			}
			pfd->Release();
		}
	} 
	else 
	{
		CFileDialog fd(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Obrazy\0*.png;*.jpg;*.bmp;*.ico\0\0");

		if(fd.DoModal() == IDOK) 
		{
			filePath = fd.m_szFileName;
		}
	}

	return filePath;
}

int wtwAvatars::getFileType(const std::wstring& path) 
{
	if(path.length() > 4) 
	{
		const wchar_t* ext = &path[path.length() - 4];

		//__LOG_F(wtw, 1, L"avatar", L"ext = %s, path = %s", ext, path.c_str());

		if(_wcsicmp(ext, L".png") == 0)
		{
			return WTW_AVATAR_TYPE_PNG;
		}

		if(_wcsicmp(ext, L".jpg") == 0)
		{
			return WTW_AVATAR_TYPE_JPG;
		}

		if(_wcsicmp(ext, L".bmp") == 0)
		{
			return WTW_AVATAR_TYPE_BMP;
		}

		if(_wcsicmp(ext, L".ico") == 0)
		{
			return WTW_AVATAR_TYPE_ICO;
		}
	}
	return -1;
}

void wtwAvatars::onContactAdded(wtwContactListItem* cnt) 
{
	if(cnt->itemType == WTW_CTL_TYPE_ELEMENT || cnt->itemType == WTW_CTL_TYPE_METAC)
	{
		if(wtw->fnCall(WTW_AVATAR_GET_SOURCE, cnt->contactData, 0) == AV_SOURCE_DEFAULT) 
		{
			StringMap::const_iterator i = avatars.find(cnt->contactData.id);

			if(i != avatars.end()) 
			{
				int type = getFileType(i->second);

				if(type == -1)
				{
					return;
				}

				wtwAvatarSet a;
				a.filePath = i->second.c_str();
				a.avatarType = type;
				a.source = AV_SOURCE_DEFAULT;
				a.contactHandle = cnt->itemHandle;

				wtw->fnCall(WTW_AVATAR_SET, a, 0);
			}
		} 
		else
		{ 
			if(avatars.find(cnt->contactData.id) != avatars.end()) 
			{
				avatars.erase(cnt->contactData.id);
			}
		}
	}
}

WTW_PTR wtwAvatars::MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;
	wtwMenuItemDef* menu = (wtwMenuItemDef*)wParam;

	if(!nfo || !menu)
	{
		return 0;
	}

	if(nfo->iContacts == 1) 
	{
		wtwContactDef* cnt = &nfo->pContacts[0];
		wtwContactListItem item;
		HANDLE hCnt = 0;
		wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)cnt, (WTW_PARAM)&hCnt);

		if(hCnt && wtw->fnCall(WTW_CTL_CONTACT_GET, (WTW_PARAM)hCnt, item) == S_OK && (item.itemType == WTW_CTL_TYPE_ELEMENT))
		{
			std::wstring buf;

			wtwAvatarSet a;
			a.contactHandle = hCnt;
			a.source = AV_SOURCE_DEFAULT;

			if(wcscmp(menu->itemId, MENU_ITEM_SET) == 0)
			{
				buf = ((wtwAvatars*)ptr)->getFile();
				a.filePath = buf.c_str();
				a.avatarType = WTW_AVATAR_TYPE_PNG;
				((wtwAvatars*)ptr)->avatars[cnt->id] = buf;
			} 
			else if(wcscmp(menu->itemId, MENU_ITEM_CLR) == 0)
			{
				((wtwAvatars*)ptr)->avatars.erase(cnt->id);
			}

			wtw->fnCall(WTW_AVATAR_SET, a, 0);
		}
	}
	return 0;
}

WTW_PTR wtwAvatars::MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) 
{
	wtwMenuCallbackEvent* event = (wtwMenuCallbackEvent*)wParam;
	wtwAvatars* av = (wtwAvatars*)ptr;

	if(event->pInfo->iContacts == 1)
	{
		wtwContactDef* cnt = event->pInfo->pContacts;
		wtwPresenceDef p;

		{
			initStruct(p);
			wchar_t buf[1024] = { 0 };
			wsprintf(buf, L"%s/%d/%s", cnt->netClass, cnt->netId, WTW_PF_STATUS_GET);
			wtw->fnCall(buf, reinterpret_cast<WTW_PARAM>(&p), 0);
		}

		wtwContactListItem item;
		HANDLE hCnt = 0;
		wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)cnt, (WTW_PARAM)&hCnt);

		if(p.curStatus != WTW_PRESENCE_OFFLINE && hCnt && wtw->fnCall(WTW_CTL_CONTACT_GET, (WTW_PARAM)hCnt, item) == S_OK && (item.itemType == WTW_CTL_TYPE_ELEMENT)) 
		{
			if(wtw->fnCall(WTW_AVATAR_GET_SOURCE, (WTW_PARAM)cnt, 0) == AV_SOURCE_DEFAULT) 
			{
				//__LOG_F(wtw, 1, L"avatars", L"uuid = %s", cnt->uuid);

				if(av->avatars.find(cnt->id) == av->avatars.end()) 
				{
					event->slInt.add(event->itemsToShow, MENU_ITEM_SET);
				}
				else
				{
					event->slInt.add(event->itemsToShow, MENU_ITEM_CLR);
				}
			}
		}
	}
	return 0;
}

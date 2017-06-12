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

#include <algorithm>
//#include <shobjidl.h>

#include "wtwFTPsender.hpp"
#include "wtwFTPsender_external.h"
#include "utlFTP_api.h"

wtwFTPsender* wtwFTPsender::inst = 0;

#define SCI_GETTEXTLENGTH 2183
#define SCI_INSERTTEXT 2003
#define MENU_ITEM_SEND L"wtwFTPsender/send"

wtwFTPsender::wtwFTPsender(HINSTANCE hInstance) : wndCreateHook(0) {
	extFuncHandle = (HANDLE)wtw->fnCreate(WTW_FTP_SENDER_SEND_FILE, wtwFTPsender::externalFuncSendFile, (void*)this);
	utlFtpFuncHandle = (HANDLE)wtw->fnCreate(utlFTP::WTW_FTP_SEND_FILE, wtwFTPsender::externalUtlFtpFuncSendFile, (void*)this);

	this->hInstance = hInstance;
	wndCreateHook = wtw->evHook(WTW_EVENT_ON_CHATWND_CREATE, wtwFTPsender::onWndCreate, (void*)this);

	updateServerInfo();

	wtwMenuItemDef mi;
	initStruct(mi);

	mi.menuID		= WTW_MENU_ID_CONTACT_SEND;
	mi.hModule		= hInstance;
	mi.callback		= &wtwFTPsender::MenuCallback;
	mi.itemId		= MENU_ITEM_SEND;
	mi.menuCaption	= L"Wy�lij plik na serwer FTP";
	mi.cbData		= (void*)this;
	mi.iconId		= PLUGIN_ICON;
	wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

	onMenuRebuild = wtw->evHook(WTW_EVENT_MENU_REBUILD, &wtwFTPsender::MenuRebuild, (void*)this);
}

wtwFTPsender::~wtwFTPsender() {
	if(wndCreateHook) {
		wtw->evUnhook(wndCreateHook);
		wndCreateHook = 0;
	}
	if(onMenuRebuild) {
		wtw->evUnhook(onMenuRebuild);
		onMenuRebuild = 0;
	}
	if(extFuncHandle) {
		wtw->fnDestroy(extFuncHandle);
		extFuncHandle = 0;
	}
	if(utlFtpFuncHandle) {
		wtw->fnDestroy(utlFtpFuncHandle);
		utlFtpFuncHandle = 0;
	}
}

void wtwFTPsender::addButton(wtwContactDef *cnt, wtwChatWindowInfo *nfo) {
	if(cfgGetInt(SETTING_FTP_SHOW_BUTTON, 1) == 0) {
		return;
	}

	wtwCommandEntry entry;
	initStruct(entry);
	entry.pContactData = cnt;
	entry.callback = onClickCB;
	entry.cbData = (void*)this;
	entry.hInstance = hInstance;
	entry.itemData = (void*)nfo->hWindow;
	entry.itemFlags = 0;
	entry.itemId = L"wtwFTPsender/button";
	entry.itemType = CCB_TYPE_STANDARD;
	entry.toolTip = L"Wy�lij plik przez FTP...";
	entry.graphId = PLUGIN_ICON;

	entry.pWnd = nfo->pWnd;

	wtw->fnCall(WTW_CCB_FUNCT_ADD, (WTW_PARAM)&entry, 0);

}

void wtwFTPsender::onClick(wtwCommandCallback* c) {
	if(c->action == CCB_ACTION_LCLICK) {
		getFile(&c->contactInfo[c->selectedContact]);
	}
}

void wtwFTPsender::getFile(wtwContactDef* c) {
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
		CFileDialog fd(TRUE, 0, 0, 4 | 2, L"Wszystkie pliki (*.*)\0*.*\0");
		if(fd.DoModal() == IDOK) {
			filePath = fd.m_szFileName;
		}
	}

	if(filePath.size()) {
		FTPSender::Contact cnt;
		cnt.id = c->id;
		cnt.netID = c->netClass;
		cnt.netSID = c->netId;

		sender.addToQueue(cnt, filePath);
	}
}

WTW_PTR wtwFTPsender::externalFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr) {
	wtwFTPsender* s = (wtwFTPsender*)ptr;

	if(!s || !lParam)
		return 1;

	wtwFtpFile* f = (wtwFtpFile*)lParam;
	if(f->contactId == 0 || wcslen(f->contactId) == 0)
		return 1;
	if(f->filePath == 0 || wcslen(f->filePath) < 3)
		return 1;
	if(f->netClass == 0 || wcslen(f->netClass) == 0)
		return 1;

	FTPSender::Contact c;
	c.id = f->contactId;
	c.netID = f->netClass;
	c.netSID = f->netId;

	s->sender.addToQueue(c, f->filePath);
	return 0;
}

WTW_PTR wtwFTPsender::externalUtlFtpFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr) {
	wtwFTPsender* s = (wtwFTPsender*)ptr;
//	__LOG_F(wtw, 1, L"ftp", L"params, l = %d, w = %d, ptr = %d", (int)lParam, (int)wParam, (int)ptr);

	if(!s || !lParam)
		return 1;

	utlFTP::wtwFtpFile* f = (utlFTP::wtwFtpFile*)lParam;
	if(f->contact.id == 0 || wcslen(f->contact.id) == 0) {
//		__LOG(wtw, L"ftp", L"id = null");
		return 1;
	}

	if(f->filePath == 0 || wcslen(f->filePath) < 4) {
//		__LOG(wtw, L"ftp", L"path = null");
		return 1;
	}

	//if(f->contact.netClass || wcslen(f->contact.netClass) == 0) {
	//	__LOG(wtw, L"ftp", L"netClass = null");
	//	return 1;
	//}

//	__LOG_F(wtw, 1, L"ftp", L"id = %s, netClass = %s, netId = %d, path = %s", f->contact.id, f->contact.netClass, f->contact.netId, f->filePath);

	FTPSender::Contact c;
	c.id = f->contact.id;
	c.netID = f->contact.netClass;
	c.netSID = f->contact.netId;

	s->sender.addToQueue(c, f->filePath);
	return 0;
}

WTW_PTR wtwFTPsender::MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;
	//wtwMenuItemDef* menu = (wtwMenuItemDef*)wParam;

	if(!nfo)
		return 0;

	if(nfo->iContacts == 1) {
		wtwContactDef* cnt = &nfo->pContacts[0];
		((wtwFTPsender*)ptr)->getFile(cnt);
	}
	return 0;
}

WTW_PTR wtwFTPsender::MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwMenuCallbackEvent* event = (wtwMenuCallbackEvent*)wParam;

	if(event->pInfo->iContacts == 1) {
		wtwContactDef* cnt = event->pInfo->pContacts;
		wtwPresenceDef p;
		initStruct(p);

		wchar_t buf[1024] = { 0 };
		wsprintf(buf, L"%s/%d/%s", cnt->netClass, cnt->netId, WTW_PF_STATUS_GET);

		wtw->fnCall(buf, reinterpret_cast<WTW_PARAM>(&p), 0);

		if(p.curStatus != WTW_PRESENCE_OFFLINE) {
			event->slInt.add(event->itemsToShow, MENU_ITEM_SEND);
		}
	}
	return 0;
}

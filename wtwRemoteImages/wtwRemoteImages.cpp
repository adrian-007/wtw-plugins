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
#include <ctime>

#include "wtwRemoteImages.hpp"
#include "wtwRemoteImages_external.h"

#include "../utils/text.hpp"
#include "../utils/WinApiFile.hpp"

wtwRemoteImages* wtwRemoteImages::inst = 0;

#define MENU_ITEM_SEND L"wtwRemoteImages/send"
#define REMOTE_IMAGES_API_KEY "<api-key>"

wtwRemoteImages::wtwRemoteImages(HINSTANCE hInstance)
{
	extFuncHandle = (HANDLE)wtw->fnCreate(WTW_REMOTE_IMAGES_SEND_IMAGE, wtwRemoteImages::externalFuncSendFile, (void*)this);
	extFuncQueryInfo = (HANDLE)wtw->fnCreate(WTW_REMOTE_IMAGES_QUERY_INFO, wtwRemoteImages::externalFuncQueryInfo, (void*)this);

	extFuncHandleFallback = (HANDLE)wtw->fnCreate(L"wtwImageShack/sendImage", wtwRemoteImages::externalFuncSendFile, (void*)this);

	this->hInstance = hInstance;

	wtwMenuItemDef mi;
	initStruct(mi);

	mi.menuID		= WTW_MENU_ID_CONTACT_SEND;
	mi.hModule		= hInstance;
	mi.callback		= &wtwRemoteImages::MenuCallback;
	mi.itemId		= MENU_ITEM_SEND;
	mi.menuCaption	= L"Wy�lij obrazek na " SERVICE_PROVIDER;
	mi.cbData		= (void*)this;
	mi.iconId		= L"wtwRemoteImages/icon16";
	wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

	onMenuRebuild = wtw->evHook(WTW_EVENT_MENU_REBUILD, &wtwRemoteImages::MenuRebuild, (void*)this);
}

wtwRemoteImages::~wtwRemoteImages()
{
	if(onMenuRebuild)
	{
		wtw->evUnhook(onMenuRebuild);
		onMenuRebuild = 0;
	}

	if(extFuncHandle)
	{
		wtw->fnDestroy(extFuncHandle);
		extFuncHandle = 0;
	}

	if(extFuncHandleFallback)
	{
		wtw->fnDestroy(extFuncHandleFallback);
		extFuncHandleFallback = 0;
	}

	wtw->fnCall(WTW_MENU_ITEM_CLEAR, (WTW_PARAM)hInstance, 0);
}

void wtwRemoteImages::sendImage(const wchar_t* contactId, const wchar_t* netClass, int netId, const wchar_t* filePath)
{
	std::string data;

	{
		WinApiFile file;
		if(file.open(filePath, WinApiFile::READ_MODE, WinApiFile::OPEN_IF_EXIST))
		{
			data.resize(file.getFileSize());
			file.read(&data[0], data.size());
		}
		else
		{
			data.clear();
		}

		file.close();
	}

	std::wstring filename = filePath;
	{
		std::wstring::size_type i = filename.rfind(L"\\");
		if(i != std::wstring::npos)
		{
			filename = filename.substr(i+1);
		}

		filename = utils::text::urlEscapeW(filename);
	}

	std::wstring header;
	std::string post;

	post += "------------ei4KM7Ef1ae0GI3GI3ei4ei4Ij5GI3\r\n";
	post += "Content-Disposition: form-data; name=\"key\"\r\n";
	post += "\r\n";
	post += REMOTE_IMAGES_API_KEY;
	post += "\r\n";
	post += "------------ei4KM7Ef1ae0GI3GI3ei4ei4Ij5GI3\r\n";
	post += "Content-Disposition: form-data; name=\"image\"; filename=\"" + utils::text::fromWide(filename) + "\"\r\n";
	post += "Content-Type: application/octet-stream\r\n";
	post += "\r\n";
	post += data;
	post += "\r\n";
	post += "------------ei4KM7Ef1ae0GI3GI3ei4ei4Ij5GI3--";

	header += L"Content-Type: multipart/form-data; boundary=----------ei4KM7Ef1ae0GI3GI3ei4ei4Ij5GI3\r\n";
	header += L"Content-Length: ";

	{
		wchar_t buf[64] = { 0 };
		wsprintf(buf, L"%d", post.length());
		header += buf;
	}

	header += L"\r\n";

	wchar_t fileId[2048] = { 0 };
	swprintf(fileId, sizeof(fileId), L"imgur/%s/%s/%d/%d", contactId, netClass, netId, time(0));

	itemData* item = new itemData;
	item->id = contactId;
	item->netClass = netClass;
	item->netId = netId;

	wtwHttpGetFile file;
	file.callback = wtwRemoteImages::onHttpService_callback;
	file.cbData = item;
	file.fileId = fileId;
	file.flags = WTW_HTTP_SERVICE_WRITE_TO_CB | WTW_HTTP_SERVICE_ASYNCHRONOUS;

	file.uri = L"http://api.imgur.com/2/upload.xml";
	file.headers = header.c_str();

	file.method = L"POST";
	file.methodData = post.c_str();
	file.methodDataSize = post.size();

	if(wtw->fnCall(WTW_HTTP_GET_FILE, file, 0) != S_OK)
	{
		__LOG(wtw, L"REMOTE_IMAGES", L"stg went wrong...");
	}
}

void wtwRemoteImages::onRequestComplete(const wchar_t* fileId, wtwHttpStatus* status, itemData* data)
{
	__LOG_F(wtw, 1, L"wtwRemoteImages", L"status [0x%X], HTTP code [%d], id [%s]", status->status, status->responseCode, fileId);

	if(status->status == S_OK && status->responseCode == 200)
	{
		std::wstring buf = utils::text::toWide(data->buffer);
		std::wstring ret = utils::text::stripFromTags<std::wstring>(L"<original>", L"</original>", buf);
		bool ok = false;

		if(ret.empty() == false)
		{
			ok = true;
			showToolTip(L"wtwRemoteImages", L"Link z obrazkiem wys�any do kontaktu, id: " + data->id);
		}
		else
		{
			ret = utils::text::stripFromTags<std::wstring>(L"<message>", L"</message>", buf);

			if(ret.empty())
			{
				ret = L"Nieznany b��d";
			}
			else
			{
				ret = L"B��d podczas wysy�ania: " + ret;
			}
		}

		sendMessage(data->id.c_str(), data->netClass.c_str(), data->netId, ret, ok && cfgGetInt(SETTING_REMOTE_IMAGES_SEND_AS_INFO, 0) == 0);
	}
	else
	{
		showToolTip(L"B��d", L"Nieznany b��d podczas wysy�ania pliku do " SERVICE_PROVIDER);
	}
}

void wtwRemoteImages::getFile(wtwContactDef* c)
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
			files.pszSpec = L"*.jpg;*.jpeg;*.png";

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
		CFileDialog fd(TRUE, 0, 0, 4 | 2, L"Obrazy\0*.jpg;*.jpeg;*.png\0");

		if(fd.DoModal() == IDOK)
		{
			filePath = fd.m_szFileName;
		}
	}

	if(filePath.size())
	{
		sendImage(c->id, c->netClass, c->netId, filePath.c_str());
	}
}

void wtwRemoteImages::showToolTip(const std::wstring& caption, const std::wstring& msg) {
	wtwTrayNotifyDef nt;
	initStruct(nt);
	nt.textMain = caption.c_str();
	nt.textLower = msg.c_str();
	nt.iconId = L"wtwRemoteImages/icon16";
	nt.graphType = WTW_TN_GRAPH_TYPE_SKINID;
	wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), 0);
}

void wtwRemoteImages::sendMessage(const wchar_t* id, const wchar_t* netClass, int netId, const std::wstring& msg, bool sendToContact)
{
	wtwMessageDef lMsg;
	initStruct(lMsg);

	lMsg.contactData.id = id;
	lMsg.contactData.netClass = netClass;
	lMsg.contactData.netId = netId;

	lMsg.msgMessage = msg.c_str();
	lMsg.msgTime = time(0);

	if(sendToContact)
	{
		wchar_t buf[1024] = { 0 };
		wsprintf(buf, L"%s/%d/%s", netClass, netId, WTW_PF_MESSAGE_SEND);

		lMsg.msgFlags = WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_OUTGOING;
		wtw->fnCall(buf, reinterpret_cast<WTW_PARAM>(&lMsg), 0);
	}
	else
	{
		lMsg.msgFlags = WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_INFO;
	}

	wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(&lMsg), 0);
}

WTW_PTR wtwRemoteImages::externalFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr)
{
	wtwRemoteImages* s = (wtwRemoteImages*)ptr;

	if(!s || !lParam)
	{
		return S_FALSE;
	}

	wtwRemoteImagesFile* f = (wtwRemoteImagesFile*)lParam;
	if(f->contactId == 0 || wcslen(f->contactId) == 0)
	{
		return S_FALSE;
	}

	if(f->filePath == 0 || wcslen(f->filePath) < 3)
	{
		return S_FALSE;
	}

	if(f->netClass == 0 || wcslen(f->netClass) == 0)
	{
		return S_FALSE;
	}

	s->sendImage(f->contactId, f->netClass, f->netId, f->filePath);

	return S_OK;
}

WTW_PTR wtwRemoteImages::externalFuncQueryInfo(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr)
{
	wtwRemoteImagesInfo* info = (wtwRemoteImagesInfo*)lParam;

	if(info == 0)
	{
		return S_FALSE;
	}

	if(info->serviceNameLen > 0)
	{
		if(info->serviceName == 0)
		{
			return S_FALSE;
		}

		wcsncpy(info->serviceName, SERVICE_PROVIDER, info->serviceNameLen);
	}

	if(info->icon16_len > 0)
	{
		if(info->icon16_id == 0)
		{
			return S_FALSE;
		}

		wcsncpy(info->icon16_id, L"wtwRemoteImages/icon16", info->icon16_len);
	}

	if(info->icon24_len > 0)
	{
		if(info->icon24_id == 0)
		{
			return S_FALSE;
		}

		wcsncpy(info->icon24_id, L"wtwRemoteImages/icon24", info->icon24_len);
	}

	if(info->icon32_len > 0)
	{
		if(info->icon32_id == 0)
		{
			return S_FALSE;
		}

		wcsncpy(info->icon32_id, L"wtwRemoteImages/icon32", info->icon32_len);
	}

	if(info->icon48_len > 0)
	{
		if(info->icon48_id == 0)
		{
			return S_FALSE;
		}

		wcsncpy(info->icon48_id, L"wtwRemoteImages/icon48", info->icon48_len);
	}

	return S_OK;
}

WTW_PTR wtwRemoteImages::MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;

	if(!nfo)
	{
		return 0;
	}

	if(nfo->iContacts == 1)
	{
		wtwContactDef* cnt = &nfo->pContacts[0];
		((wtwRemoteImages*)ptr)->getFile(cnt);
	}

	return 0;
}

WTW_PTR wtwRemoteImages::MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwMenuCallbackEvent* event = (wtwMenuCallbackEvent*)wParam;

	if(event->pInfo->iContacts == 1)
	{
		wtwContactDef* cnt = event->pInfo->pContacts;
		wtwPresenceDef p;
		initStruct(p);

		wchar_t buf[1024] = { 0 };
		wsprintf(buf, L"%s/%d/%s", cnt->netClass, cnt->netId, WTW_PF_STATUS_GET);

		wtw->fnCall(buf, reinterpret_cast<WTW_PARAM>(&p), 0);

		if(p.curStatus != WTW_PRESENCE_OFFLINE)
		{
			event->slInt.add(event->itemsToShow, MENU_ITEM_SEND);
		}
	}

	return 0;
}

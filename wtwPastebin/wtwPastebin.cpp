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

#include "wtwPastebin.hpp"
#include "wtwPastebin_external.h"
#include "TextDialog.h"

#include "../utils/wtw.hpp"
#include "../utils/utils.hpp"
#include "../utils/text.hpp"
#include "../utils/WinApiFile.hpp"

wtwPastebin* wtwPastebin::inst = 0;

#define MENU_ITEM_SEND L"wtwPastebin/send"
#define API_DEV_KEY "e974f9445ec8d7a68a33379fc3a675ca"

void wtwPastebin::create(wtwPastebin* m)
{
	wtwPastebin::inst = m;
}

void wtwPastebin::destroy()
{
	delete wtwPastebin::inst;
	wtwPastebin::inst = 0;
}

wtwPastebin* wtwPastebin::instance()
{
	return wtwPastebin::inst;
}

wtwPastebin::wtwPastebin(HINSTANCE hInstance)
{
	extFuncHandle = (HANDLE)wtw->fnCreate(WTW_FTP_SENDER_SEND_FILE, wtwPastebin::externalFuncSendFile, (void*)this);

	this->hInstance = hInstance;

	wtw_t::preloadIcon(wtw, hInstance, PLUGIN_ICON, L"wtwPastebin16.png", IDB_PNG1);

	wtwMenuItemDef mi;
	initStruct(mi);

	mi.menuID		= WTW_MENU_ID_CONTACT_SEND;
	mi.hModule		= hInstance;
	mi.callback		= &wtwPastebin::MenuCallback;
	mi.itemId		= MENU_ITEM_SEND;
	mi.menuCaption	= L"Wy�lij tekst na pastebin.com";
	mi.cbData		= (void*)this;
	mi.iconId		= PLUGIN_ICON;
	wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

	onMenuRebuild = wtw->evHook(WTW_EVENT_MENU_REBUILD, &wtwPastebin::MenuRebuild, (void*)this);
}

wtwPastebin::~wtwPastebin()
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

	wtw->fnCall(WTW_MENU_ITEM_CLEAR, (WTW_PARAM)hInstance, 0);
}

void wtwPastebin::getFile(wtwContactDef* c)
{
	TextDialog dlg;
	dlg.id = c->id;
	dlg.netClass = c->netClass;
	dlg.netId = c->netId;

	void* h = 0;
	wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)c, (WTW_PARAM)&h);

	if(h)
	{
		wtwContactListItem i;
		initStruct(i);
		wtw->fnCall(WTW_CTL_CONTACT_GET, (WTW_PARAM)h, (WTW_PARAM)&i);
		dlg.nick = i.itemText;
	} 
	else
	{
		dlg.nick = c->id;
	}

	dlg.DoModal(0);
}

void wtwPastebin::send(const wchar_t* id, const wchar_t* netClass, int netId, const pasteItem& pi)
{
	item* i = new item;
	i->id = id;
	i->netClass = netClass;
	i->netId = netId;

	std::wstring timestamp = L"wtwPastebin/" + utils::conv::toStringW(static_cast<unsigned int>(time(0)));

	wtwHttpGetFile file;
	file.callback = wtwPastebin::onHttpService_callback;
	file.cbData = i;
	file.fileId = timestamp.c_str();

	file.flags = WTW_HTTP_SERVICE_WRITE_TO_CB | WTW_HTTP_SERVICE_ASYNCHRONOUS;
	file.uri = L"http://pastebin.com/api/api_post.php";
	file.method = L"POST";

	std::wstring header;
	std::string post;

	{
		post += "api_dev_key=" API_DEV_KEY;
		post += "&api_option=paste&";

		if(pi.priv) 
		{
			post += "api_paste_private=1";
		} 
		else
		{
			post += "api_paste_private=0";
		}

		post += "&";
		post += "api_paste_expire_date=";
		post += utils::text::fromWide(pi.expire);
		post += "&";

		if(pi.format.empty() == false)
		{
			post += "api_paste_format=";
			post += utils::text::fromWide(pi.format);
			post += "&";
		}

		post += "api_paste_code=" + utils::text::urlEscape(pi.data);

		header += L"Content-Type: application/x-www-form-urlencoded\r\n";
		header += L"Content-Length: " + utils::conv::toStringW(post.size());
		header += L"\r\n";
	}

	file.headers = header.c_str();
	file.methodData = post.c_str();
	file.methodDataSize = post.size();

	wtw->fnCall(WTW_HTTP_GET_FILE, file, 0);
}

void wtwPastebin::item::sendUrl(const std::wstring& url)
{
	bool send = false;//cfgGetInt(SETTING_FTP_SEND_AS_INFO, 0) == 0;

	std::wstring msg;

	if(!send)
		msg += L"[wtwPastebin] \n";
	msg += url;

	sendMessage(msg, send);

	if(send)
	{
		wtwPastebin::ShowToolTip(L"wtwPastebin", L"Link wys�any do " + getContactName());
	}
	else
	{
		wtwPastebin::ShowToolTip(L"wtwPastebin", L"Link gotowy dla " + getContactName());
	}
}

void wtwPastebin::item::sendMessage(const std::wstring& msg, bool sendToContact)
{
	wtwMessageDef lMsg;
	initStruct(lMsg);

	lMsg.contactData.id = id.c_str();
	lMsg.contactData.netClass = netClass.c_str();
	lMsg.contactData.netId = netId;

	lMsg.msgMessage = msg.c_str();
	lMsg.msgTime = time(0);

	if(sendToContact) 
	{
		wchar_t buf[1024] = { 0 };
		wsprintf(buf, L"%s/%d/%s", netClass.c_str(), netId, WTW_PF_MESSAGE_SEND);

		lMsg.msgFlags = WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_OUTGOING | WTW_MESSAGE_FLAG_FORCEDISP;
		if(!wtw->fnCall(buf, reinterpret_cast<WTW_PARAM>(&lMsg), 0)) 
		{
			wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(&lMsg), 0);
		}
	}
	else
	{
		lMsg.msgFlags = WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_INFO;	
		wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(&lMsg), 0);
	}
}

std::wstring wtwPastebin::item::getContactName() const
{
	wtwContactDef c;
	initStruct(c);
	c.id = id.c_str();
	c.netClass = netClass.c_str();
	c.netId = netId;

	WTW_PTR h = 0;
	wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)&c, (WTW_PARAM)&h);

	if(h) 
	{
		wtwContactListItem item;
		initStruct(item);
		wtw->fnCall(WTW_CTL_CONTACT_GET, (WTW_PARAM)h, (WTW_PARAM)&item);
		return item.itemText;
	} 
	else 
	{
		return id;
	}
}

void wtwPastebin::ShowToolTip(const std::wstring& caption, const std::wstring& msg)
{
	wtwTrayNotifyDef nt;
	initStruct(nt);
	nt.textMain = caption.c_str();
	nt.textLower = msg.c_str();
	nt.iconId = PLUGIN_ICON;
	nt.graphType = WTW_TN_GRAPH_TYPE_SKINID;
	wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), 0);
}

WTW_PTR wtwPastebin::externalFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr)
{
	wtwPastebin* s = (wtwPastebin*)ptr;

	if(!s || !lParam)
	{
		return 1;
	}

	wtwPastebinData* f = (wtwPastebinData*)lParam;
	if(f->contactId == 0 || wcslen(f->contactId) == 0)
	{
		return 1;
	}

	if(f->data == 0 || wcslen(f->data) < 3) 
	{
		return 1;
	}

	if(f->netClass == 0 || wcslen(f->netClass) == 0)
	{
		return 1;
	}

	pasteItem pi;
	if(f->type == WTW_PASTEBIN_TYPE_PATH)
	{
		WinApiFile file;
		if(file.open(f->data, WinApiFile::READ_MODE))
		{
			pi.data.resize(file.getFileSize());
			file.read(&pi.data[0], pi.data.size());
		}
	}
	else
	{
		pi.data = utils::text::fromWide(f->data);
	}

	s->send(f->contactId, f->netClass, f->netId, pi);

	return 0;
}

WTW_PTR wtwPastebin::MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;
	wtwMenuItemDef* menu = (wtwMenuItemDef*)wParam;

	if(!nfo || !menu)
	{
		return 0;
	}

	if(nfo->iContacts == 1) 
	{
		wtwContactDef* cnt = &nfo->pContacts[0];
		((wtwPastebin*)ptr)->getFile(cnt);
	}
	return 0;
}

WTW_PTR wtwPastebin::MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
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

WTW_PTR wtwPastebin::onHttpService_callback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwHttpEvent* event = (wtwHttpEvent*)wParam;
	item* i = (item*)ptr;

	if(event == 0 || i == 0)
	{
		return 0;
	}

	switch(event->event)
	{
	case WTW_HTTP_EVENT_DATA:
		{
			wtwRawDataDef* rd = (wtwRawDataDef*)lParam;
			
			if(rd && rd->pDataLen > 0)
			{
				i->dataBuffer += utils::text::toWide(std::string((const char*)rd->pData, rd->pDataLen));
			}

			break;
		}
	case WTW_HTTP_EVENT_COMPLETE:
		{
			wtwHttpStatus* s = (wtwHttpStatus*)lParam;
			
			if(s)
			{
				std::wstring url;

				if(s->status == S_OK)
				{
					if(i->dataBuffer.compare(0, 7, L"http://") == 0) 
					{
						url = i->dataBuffer;
					}
				}

				if(url.empty()) 
				{
					std::wstring errMsg;
					errMsg += L"Wysy�anie pliku";
					errMsg += L" nie powiod�o si�!";

					i->sendMessage(errMsg, false);
					wtwPastebin::ShowToolTip(L"wtwPastebin", L"Wys�anie tekstu do " + i->getContactName() + L" nie powiod�o si�!");
					__LOG_F(wtw, 1, L"wtwPastebin", L"Failed to send text: %s", i->dataBuffer.c_str());
				} 
				else 
				{
					i->sendUrl(url);
				}
			}
			
			delete i;
			
			break;
		}
	default:
		{
		}
	}

	return 0;
}

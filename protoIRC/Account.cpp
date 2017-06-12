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
#include "PrivateChatTab.hpp"
#include "Channel.h"
#include "Account.h"
#include "Utils.hpp"
#include "version.hpp"

#include "ContactPage.h"
#include "SettingsPage.h"

#include <boost/format.hpp>

using boost::str;
using boost::wformat;

Account::Account(const std::wstring& server, const std::wstring& uuid, const std::wstring& name, const std::wstring& user /*= L""*/, const std::wstring& nick /*= L""*/, const std::wstring& pass /*= L""*/)
{
	this->server = server;
	this->uuid = uuid;
	this->name = name;
	//this->status = 0;
	this->port = 6667;
	this->nick = nick;
	this->passwd = pass;
	this->user = user;
	this->windowHandle = 0;

	if(this->nick.empty()) 
	{
		const wchar_t* pName = 0;
		core->fnCall(WTW_GET_PROFILE_NAME_PTR_EX, (WTW_PARAM)&pName, 0);
		this->nick = pName;
	}

	if(this->uuid.empty())
	{
		this->uuid = generateUUID();
	}

	// rejestracja protokolu
	{
		wtwProtocolDef pDef;
		initStruct(pDef);

		pDef.protoCaps1 = WTW_PROTO_CAPS1_CHAT | WTW_PROTO_CAPS1_PRESENCE;
		pDef.protoCaps2	= WTW_PROTO_CAPS2_UTF;
		pDef.protoDescr	= L"Internet Relay Channel";
		pDef.netClass	= PROTO_CLASS;
		pDef.protoName	= PROTO_NAME;
		pDef.netGUID	= uuid.c_str();

		pDef.pInterface.pfDeleteContact	= &Account::funcRemoveChannel;
		pDef.pInterface.pfMoveContact	= &Account::funcMoveChannel;
		pDef.pInterface.cbData			= (void*)this;

		pDef.flags		= WTW_PROTO_FLAG_NO_PUBDIR;
		pDef.protoState = &status;

		core->fnCall(WTW_PROTO_FUNC_ADD, reinterpret_cast<WTW_PARAM>(&pDef), 0);
		netId = pDef.netId;

		protoFunc = wtwInstProtoFunc(core, PROTO_CLASS, netId, WTW_PF_INIT_CHAT, &funcChannelInit, (void*)this);
		messageFunc = wtwInstProtoFunc(core, PROTO_CLASS, netId, WTW_PF_MESSAGE_SEND, &funcMessageSend, (void*)this);
		presenceSetFunc = wtwInstProtoFunc(core, PROTO_CLASS, netId, WTW_PF_STATUS_SET, &funcPresenceSet, (void*)this);
	}

	// rejestracja itemow w menu statusow
	{
		addPresenceMenuItem(L"available", L"Po��czony");
		addPresenceMenuItem(L"away", L"Zaraz wracam");
		addPresenceMenuItem(L"unavailable", L"Roz��czony");
	}

	// strona opcji kontaktu
	{
		wtwPropertiesPage pPage;
		initStruct(pPage);

		wchar_t pageID[1024];
		swprintf(pageID, sizeof(pageID), L"IRC/%d/Properties", netId);

		pPage.callback			= &ContactPage::callback;
		pPage.caption			= L"W�a�ciwo�ci";
		pPage.flags				= WTW_PP_FLAG_ALLOW_ADD | WTW_PP_FLAG_ALLOW_EDT | WTW_PP_FLAG_AS_GROUP;
		pPage.netClass			= PROTO_CLASS;
		pPage.netId				= netId;
		pPage.cbData			= (void*)this;
		pPage.pageId			= pageID;
		pPage.position			= -1000;
		pPage.iconId			= L"IRC/Icon/available";

		core->fnCall(WTW_PROPERTIES_WND_ADD_STATIC_PROTO_PAGE, reinterpret_cast<WTW_PARAM>(&pPage), 0);
	}

	// strona w opcjach
	{
		wtwOptionPageDef def;
		initStruct(def);
		wchar_t pageID[1024] = { 0 };
		wchar_t pageName[1024] = { 0 };
		wnsprintf(pageID, sizeof(pageID), L"protoIRC/%d/Options", netId);

		if(name.empty())
		{
			wcscpy(pageName, PROTO_CLASS);
		}
		else
		{
			wnsprintf(pageName, sizeof(pageName), L"%s - %s", PROTO_CLASS, this->name.c_str());
		}

		def.callback	= SettingsPage::callback;
		def.cbData		= (void*)this;
		def.id			= pageID;
		def.caption		= pageName;
		def.parentId	= WTW_OPTIONS_GROUP_NETWORK;
		def.iconId		= L"IRC/Icon/available";

		core->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
	}

	// itemy w menu kontaktu
	{
		wtwMenuItemDef mi;
		initStruct(mi);
		std::wstring itemId;

		mi.menuID		= WTW_MENU_ID_CONTACT;
		mi.hModule		= ghInstance;
		mi.callback		= funcMenuCallback;
		mi.cbData		= (void*)this;

		itemId = str(wformat(L"IRC/%1%/join") % netId);
		mi.itemId		= itemId.c_str();
		mi.menuCaption  = L"Do��cz do kana�u";
		core->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

		itemId = str(wformat(L"IRC/%1%/part") % netId);
		mi.itemId		= itemId.c_str();
		mi.menuCaption  = L"Opu�� kana�";
		core->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
	}

	updateInfo();
	sendPresence(WTW_PRESENCE_OFFLINE);

	onMenuRebuildHook = core->evHook(WTW_EVENT_MENU_REBUILD, &Account::funcMenuRebuild, (void*)this);

	core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Created object: %p, netId = %d", this, netId);
}

void Account::close()
{
	irc_onDisconnected();

	core->evUnhook(onMenuRebuildHook);
	onMenuRebuildHook = 0;

	//status = 0;

	core->fnCall(WTW_PROTO_FUNC_DEL, (WTW_PARAM)PROTO_CLASS, netId);
	core->fnDestroy(protoFunc);
	core->fnDestroy(messageFunc);
	core->fnDestroy(presenceSetFunc);

	protoFunc = presenceSetFunc = messageFunc = 0;

	wchar_t pageID[1024] = { 0 };
	wnsprintf(pageID, sizeof(pageID), L"protoIRC/%d/Options", netId);
	core->fnCall(WTW_OPTION_PAGE_REMOVE, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(pageID));

	core->fnCall(WTW_CTL_DELETE_ALL, reinterpret_cast<WTW_PARAM>(PROTO_CLASS), netId);

	for(Channels::iterator i = channels.begin(); i != channels.end(); ++i) 
	{
		core->fnCall(WTW_CTL_CONTACT_DELETE, (WTW_PARAM)(*i)->cntHandle, 0);
		delete *i;
	}

	channels.clear();

	if(windowHandle != 0)
	{
		core->fnCall(WTW_CONFERENCE_WINDOW_DESTROY, windowHandle, 0);
		windowHandle = 0;
	}
}

std::wstring Account::generateUUID()
{
	std::wstring buf;
	UUID id;
	if(UuidCreate(&id) == RPC_S_OK) 
	{
		RPC_WSTR p = 0;
		if(UuidToStringW(&id, &p) == RPC_S_OK)
		{
			buf = (LPWSTR)p;
			RpcStringFree(&p);
		}
	}
	return buf;
}

void Account::updateInfo() 
{
	wtwSetProtoInfo(core, PROTO_CLASS, netId, server.c_str(), name.c_str());
}

void Account::updateChannels()
{
	for(Channels::iterator i = channels.begin(); i != channels.end(); ++i) 
	{
		(*i)->nick = (*i)->tempNick = this->nick;
	}
}

void Account::setPresence(const wchar_t* presence, const std::wstring& description /*= L""*/)
{
	if(wcscmp(presence, L"available") == 0)
	{
		bool conn = isConnected();

		if(conn == false)
		{
			updateChannels();
			ServerSocket::connect(server, port, utils::text::fromWide(nick), utils::text::fromWide(this->user), "", utils::text::fromWide(passwd));
		} 
		else
		{
			setAway();

			if(windowHandle != 0)
			{
				core->fnCall(WTW_CONFERENCE_WINDOW_SET_ICON, static_cast<WTW_PARAM>(windowHandle), reinterpret_cast<WTW_PARAM>(L"IRC/Icon/available"));
			}
		}
	}
	else if(wcscmp(presence, L"away") == 0)
	{
		bool conn = isConnected();

		if(conn == false)
		{
			setPresence(L"available", description);
		} 
		else 
		{
			setAway(description.empty() ? L"I'm not here right now." : description);

			if(windowHandle != 0)
			{
				core->fnCall(WTW_CONFERENCE_WINDOW_SET_ICON, static_cast<WTW_PARAM>(windowHandle), reinterpret_cast<WTW_PARAM>(L"IRC/Icon/away"));
			}
		}
	}
	else 
	{
		ServerSocket::disconnect(quitMessage);
	}
}

void Account::OnPresenceChange(const wchar_t* presence)
{
	setPresence(presence);
}

void Account::sendPresence(DWORD p)
{
	wtwProtocolEvent ev;
	initStruct(ev);

	ev.netClass = PROTO_CLASS;
	ev.netId = netId; 
	ev.event = WTW_PEV_PRESENCE_SEND; 
	ev.type = WTW_PEV_TYPE_BEFORE; 

	wtwPresenceDef prs;
	initStruct(prs);
	prs.curStatus = p;
	prs.curTimeStamp = time(0);

	if(core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&prs))) 
	{
		return; 
	}

	ev.type = WTW_PEV_TYPE_AFTER; 
	core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&prs));
}

Channel* Account::findChannel(const std::wstring& channel) 
{
	for(Channels::iterator i = channels.begin(); i != channels.end(); ++i)
	{
		if((*i)->equals(channel))
		{
			return *i;
		}
	}
	return 0;
}

PrivateChatTab* Account::findPrivateChat(const std::wstring& nick)
{
	for(PrivateChats::iterator i = privChats.begin(); i != privChats.end(); ++i)
	{
		if((*i)->getNick() == nick)
		{
			return *i;
		}
	}

	return 0;
}

AbstractTab* Account::findTab(const std::wstring& id)
{
	AbstractTab* tab = 0;

	if((tab = findChannel(id)) == 0)
	{
		tab = findPrivateChat(id);
	}

	return tab;
}

void Account::removeChannel(const std::wstring& channel)
{
	Channel* c;

	for(Channels::iterator i = channels.begin(); i != channels.end(); ++i)
	{
		c = *i;
		if(c->channel == channel)
		{
			channels.erase(i);
			delete c;
			break;
		}
	}
}

WTW_PARAM Account::getWindowHandle()
{
	if(windowHandle == 0)
	{
		wtwConferenceWindow wnd;
		initStruct(wnd);

		wnd.windowId = server.c_str();
		wnd.hInst = ghInstance;
		wnd.cbData = reinterpret_cast<void*>(this);
		wnd.windowCallback = Account::funcWindow;

		if(core->fnCall(WTW_CONFERENCE_WINDOW_CREATE_EX, wnd, reinterpret_cast<WTW_PARAM>(&windowHandle)) == S_OK)
		{
			core->fnCall(WTW_CONFERENCE_WINDOW_SET_TITLE, static_cast<WTW_PARAM>(windowHandle), reinterpret_cast<WTW_PARAM>(server.c_str()));
			core->fnCall(WTW_CONFERENCE_WINDOW_SET_ICON, static_cast<WTW_PARAM>(windowHandle), reinterpret_cast<WTW_PARAM>(L"IRC/Icon/available"));

			wtwConferenceTab tab;
			initStruct(tab);

			tab.tabId = server.c_str();
			tab.tabName = L"Serwer";
			tab.tabIcon = WTW_GRAPH_ID_CONSOLE;
			tab.tabClass = CFTC_LOG;

			core->fnCall(WTW_CONFERENCE_TAB_ADD, windowHandle, tab);
		}
	}

	return static_cast<WTW_PARAM>(windowHandle);
}

WTW_PTR Account::funcWindow(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	Account* acc = reinterpret_cast<Account*>(ptr);
	wtwConferenceWindowEvent* cb = reinterpret_cast<wtwConferenceWindowEvent*>(wParam);

	if(acc == 0 || cb == 0)
	{
		return -1;
	}

	//__LOG_F(core, 1, L"protoIRC", L"wParam = 0x%08X, lParam = 0x%08X, event = 0x%08X, tabId = %s", wParam, lParam, cb->event, cb->tabId);
	WTW_PTR ret = 0;
	AbstractTab* tab = 0;

	if(cb->tabId != 0)
	{
		tab = acc->findChannel(cb->tabId);
		
		if(tab == 0)
		{
			tab = acc->findPrivateChat(cb->tabId);
		}
	}

	switch(cb->event)
	{
	case CONFERENCE_EVENT_WND_CLOSED:
	case CONFERENCE_EVENT_WND_SHOW:
		{
			if(lParam != 0)
			{
				acc->pendingSenders.clear();
			}
			break;
		}
	case CONFERENCE_EVENT_WND_DESTROYED:
		{
			acc->windowHandle = 0;
			acc->pendingSenders.clear();
			break;
		}
	case CONFERENCE_EVENT_TAB_ON_SHOW:
		{
			if(lParam != 0)
			{
				acc->currentTabId = cb->tabId == 0 ? L"" : cb->tabId;

				std::wstring header = acc->server;

				if(tab != 0)
				{
					tab->getHeader(header);
				}

				core->fnCall(WTW_CONFERENCE_WINDOW_SET_TITLE, static_cast<WTW_PARAM>(cb->windowPtr), reinterpret_cast<WTW_PARAM>(header.c_str()));
			}
			break;
		}
	}

	if(tab != 0)
	{
		ret = tab->onWindowEvent(cb, lParam);
	}

/*	switch(cb->event) 
	{
	case CONFERENCE_EVENT_SEND_MESSAGE: 
		{
			wtwMessageDef* msg = (wtwMessageDef*)lParam;
			c->OnOutgoingMessage(msg);
			break;
		}
	case CONFERENCE_EVENT_WND_CLOSED:
		{
			//c->parent->partChannel(c->channel);
			//c->handle = 0;
			break;
		}
	case CONFERENCE_EVENT_CMD_LCLICK:
		{
			wtwConferenceCmdInfo* nfo = (wtwConferenceCmdInfo*)lParam;
			if(nfo)
			{
				if(wcscmp(nfo->itemId, L"protoIRC/partOrJoin") == 0)
				{
					wtwConferenceCmdEntry e;
					e.itemFlags = CCB_FLAG_CHANGECAPTION | CCB_FLAG_CHANGETIP;
					e.itemId = L"protoIRC/partOrJoin";
					e.pWnd = (WTW_PTR)c->handle;
					e.tabId = L"Channel/Chat";

					if(c->isSet(FLAG_JOINED))
					{
						c->part();
						e.caption = L"Join";
						e.toolTip = L"Do��cz do kana�u";
					}
					else 
					{
						c->join();
						e.caption = L"Part";
						e.toolTip = L"Opu�� kana�";
					}
					core->fnCall(WTW_CONFERENCE_CMD_MOD, e, 0);
				}		
			}
			break;
		}
	case CONFERENCE_EVENT_CT_RCLICK:
		{
			wtwConferenceContact* cnt = (wtwConferenceContact*)lParam;
			if(cnt)
			{
				bool me = wcscmp(cnt->name, c->tempNick.c_str()) == 0;
				int flags = (int)cnt->userData;

				CMenu menu;
				CMenu mode;
				menu.CreatePopupMenu();

				if(me == false)
				{
					menu.AppendMenu(MF_STRING, Channel::MENU_SHOW_USER_INFO, L"Poka� informacje o u�ytkowniku");
					menu.AppendMenu(MF_STRING, Channel::MENU_SHOW_CLIENT_INFO, L"Poka� informacje o kliencie");
					menu.AppendMenu(MF_STRING, Channel::MENU_PING, L"PING");
				}

				menu.AppendMenu(MF_STRING, Channel::MENU_SLAP, L"Slap");

				if(c->isSet(FLAG_OP)) 
				{
					mode.CreatePopupMenu();
					if(flags & FLAG_OP)
					{
						mode.AppendMenu(MF_STRING, Channel::MENU_REM_OP, L"-o");
					}
					else
					{
						mode.AppendMenu(MF_STRING, Channel::MENU_ADD_OP, L"+o");
					}

					if(flags & FLAG_VOICE)
					{
						mode.AppendMenu(MF_STRING, Channel::MENU_REM_VOICE, L"-v");
					}
					else
					{
						mode.AppendMenu(MF_STRING, Channel::MENU_ADD_VOICE, L"+v");
					}

					menu.AppendMenu(MF_SEPARATOR);
					menu.AppendMenu(MF_POPUP | MF_STRING, (UINT)mode.m_hMenu, L"MODE");

					if(me == false)
					{
						menu.AppendMenu(MF_STRING, Channel::MENU_KICK, L"Kopnij");
					}
				}

				CPoint pt(cb->pt);
				HWND hWnd = ::GetActiveWindow();
				UINT cmdId = menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, cb->pt.x, cb->pt.y, hWnd);

				switch(cmdId) 
				{
				case Channel::MENU_SHOW_USER_INFO: 
					{
						c->parent->sendCTCPRequest(cnt->contactInfo.id, L"FINGER");
						break;
					}
				case Channel::MENU_SHOW_CLIENT_INFO: 
					{
						c->parent->sendCTCPRequest(cnt->contactInfo.id, L"VERSION");
						break;
					}
				case Channel::MENU_PING: 
					{
						c->parent->sendCTCPRequest(cnt->contactInfo.id, L"PING");
						break;
					}
				case Channel::MENU_KICK: 
					{
						c->parent->kickUser(c->channel, cnt->contactInfo.id);
						break;
					}
				case Channel::MENU_SLAP:
					{
						std::wstring slap = str(wformat(L"/me slaps %1% around a bit with a large trout.") % cnt->name);
						c->handleUserCommand(slap);
						break;
					}
				case Channel::MENU_REM_OP:
					{
						c->parent->setMode(c->channel, cnt->contactInfo.id, L"-o");
						break;
					}
				case Channel::MENU_ADD_OP:
					{
						c->parent->setMode(c->channel, cnt->contactInfo.id, L"+o");
						break;
					}
				case Channel::MENU_REM_VOICE:
					{
						c->parent->setMode(c->channel, cnt->contactInfo.id, L"-v");
						break;
					}
				case Channel::MENU_ADD_VOICE:
					{
						c->parent->setMode(c->channel, cnt->contactInfo.id, L"+v");
						break;
					}
				}
			}
			break;
		}
	}*/

	return ret;
}

void Account::OnChannelInit(wtwContactDef* cnt) 
{
	core->logFormatted(1, L"status", L"netId = %d, status = %d", netId, status);

	if(!isConnected())
	{
		Utils::showNotify(L"Brak po��czenia", L"Aby do��czy� do kana�u musisz po��czy� si� z serwerem!");		
		return;
	}

	Channel* c = findChannel(cnt->id);
	if(c)
	{
		c->openChannel(true);
	}
}

void Account::OnMessageSend(wtwMessageDef* msg)
{
	Channel* c = findChannel(msg->contactData.id);

	if(c && c->isSet(Channel::FLAG_JOINED))
	{
		c->OnOutgoingMessage(msg);
	}
}

Channel* Account::setChannel(const std::wstring& uuid, const std::wstring& channel, const std::wstring& group, const std::wstring& name, const std::wstring& pass, bool autojoin /*= false*/, bool temporary /*= false*/) 
{
	Channel* c = findChannel(channel);

	if(!c)
	{
		std::wstring _uuid;

		if(uuid.empty())
		{
			_uuid = generateUUID();
		}
		else
		{
			_uuid = uuid;
		}

		c = new Channel(this, channel, group, name, _uuid, this->nick, pass);
		channels.push_back(c);
	}
	else
	{
		c->group = group;
		c->name = name;
	}

	if(autojoin)
	{
		c->setFlag(Channel::FLAG_AUTOJOIN);
	} 
	else
	{
		c->unsetFlag(Channel::FLAG_AUTOJOIN);
	}

	if(temporary)
	{
		c->setFlag(Channel::FLAG_TEMPORARY);
	}

	wtwContactListItem item;
	initStruct(item);

	if(c->group.empty()) 
	{
		c->group = WTW_DEFAULT_CT_GROUP;
	}

	item.itemText				= c->name.c_str();
	item.contactData.id			= c->channel.c_str();
	item.contactData.netClass	= PROTO_CLASS;
	item.contactData.netId		= netId;
	item.itemType				= WTW_CTL_TYPE_ELEMENT;
	item.itemGroup				= c->group.c_str();
	item.flags					= WTW_CTL_FLAG_REPLACE_DUPLICATE;

	core->fnCall(WTW_CTL_CONTACT_ADD, reinterpret_cast<WTW_PARAM>(&item), 0);
	c->cntHandle = item.itemHandle;

	wtwIconSet wgd;
	initStruct(wgd);

	wgd.iconSetID	= L"IRC/Icon";
	wgd.flags		= WTW_ICS_FLAG_PRELOADED;
	wgd.iType		= WTW_ICSET_TYPE_PRE_TEXT;

	core->fnCall(WTW_CTL_CONTACT_ICON_ADD, reinterpret_cast<WTW_PARAM>(item.itemHandle), reinterpret_cast<WTW_PARAM>(&wgd));

	return c;
}

void Account::addPresenceMenuItem(const wchar_t* presence, const wchar_t* caption)
{
	wtwMenuItemDef mid;
	wchar_t iconId[1024];
	wchar_t itemId[1024];
	wchar_t menuID[256];

	initStruct(mid);
	swprintf(itemId, sizeof(itemId), L"IRC/%d/Presence/%s", netId, presence);
	swprintf(menuID, sizeof(menuID), L"IRC/%d/Menu", netId);
	swprintf(iconId, sizeof(iconId), L"IRC/Icon/%s", presence);

	mid.itemId = itemId;
	mid.iconId = iconId;
	mid.menuID = menuID;
	mid.menuCaption = caption;

	mid.callback = &Account::funcPresenceMenuCallback;
	mid.hModule = ghInstance;
	mid.cbData = static_cast<void*>(this);
	core->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mid), 0);
}

void Account::setMainPresence(const wchar_t* presence, bool add /*= false*/)
{
	wtwMenuItemDef mi;
	wchar_t itemId[1024];
	wchar_t iconId[256];

	initStruct(mi); 
	swprintf(itemId, sizeof(itemId), L"IRC/%d/Menu", netId);
	swprintf(iconId, sizeof(iconId), L"IRC/Icon/%s", presence);

	mi.flags	= add ? 0 : WTW_MENU_MODIFY_ICON; 
	mi.menuID	= WTW_MENU_ID_MAIN_STS; 
	mi.itemId	= itemId;
	mi.iconId	= iconId; 
	mi.hModule	= ghInstance;

	core->fnCall(add ? WTW_MENU_ITEM_ADD : WTW_MENU_ITEM_MODIFY, reinterpret_cast<WTW_PARAM>(&mi), 0); 
}

std::wstring Account::getWhatifyString() const
{
	std::wstring ret;
	size_t items = pendingSenders.size();

	if(items > 0)
	{
		for(size_t i = 0; i < items - 1; ++i)
		{
			ret += pendingSenders[i] + L", ";
		}
		ret += pendingSenders[items-1];
	}

	return str(wformat(L"%1% (%2%)") % ret % server);
}

void Account::irc_onConnected() 
{
	WTW_PARAM handle = getWindowHandle();

	if(handle == 0)
	{
		return;
	}

	sendPresence(WTW_PRESENCE_ONLINE);

	core->fnCall(WTW_CONFERENCE_WINDOW_SHOW_WINDOW, handle, CWSW_SHOW_NORMAL);
}

void Account::irc_onReady()
{
	for(Channels::const_iterator i = channels.begin(); i != channels.end(); ++i) 
	{
		if((*i)->isSet(Channel::FLAG_AUTOJOIN)) 
		{
			//(*i)->openChannel();
			(*i)->join();
		}
	}
}

void Account::irc_onDisconnected() 
{
	sendPresence(WTW_PRESENCE_OFFLINE);
	core->fnCall(WTW_CTL_SET_ALL_OFFLINE, (WTW_PARAM)PROTO_CLASS, netId);

	for(Channels::iterator i = channels.begin(); i != channels.end(); ++i)
	{
		(*i)->part(L"", false);
	}

	for(PrivateChats::iterator i = privChats.begin(); i != privChats.end(); ++i)
	{
		delete *i;
	}

	privChats.clear();
}

void Account::irc_onServerLog(const std::wstring& msg)
{
	std::wstring message;
	message.reserve(msg.size());

	for(size_t i = 0; i < msg.size(); ++i)
	{
		if(iswprint(msg[i]) == 0 && msg[i] != L'\r' && msg[i] != L'\n')
		{
			message += boost::str(boost::wformat(L"\\x%02X") % static_cast<unsigned short>(msg[i]));
		}
		else
		{
			message += msg[i];
		}
	}

	wtwConferenceMessage m; 
	initStruct(m); 

	m.message.contactData.netClass	= PROTO_CLASS; 
	m.message.contactData.netId		= netId; 
	m.message.contactData.id		= server.c_str();

	m.flags							|= CONFERENCE_MESSAGE_FLAG_NO_WHATIFY;
	m.message.msgTime				= time(0); 
	m.message.msgMessage			= message.c_str();
	m.tabId							= server.c_str();

	core->fnCall(WTW_CONFERENCE_WINDOW_ADD_MESSAGE, getWindowHandle(), m);
}

void Account::irc_onIncomingMessage(const std::wstring& channel, const std::wstring& from, const std::wstring& msg) 
{
	if(callMessageHooks(channel, from, msg, true) == false)
	{
		return;
	}

	if(channel == utils::text::toWide(currentNick))
	{
		// private message
		PrivateChatTab* priv = initPrivateChat(from);

		if(priv != 0)
		{
			if(std::find(pendingSenders.begin(), pendingSenders.end(), from) == pendingSenders.end())
			{
				pendingSenders.push_back(from);
			}

			priv->OnIncomingMessage(from, msg);
		}
	}
	else
	{
		Channel* c = findChannel(channel);

		if(c) 
		{
			if(std::find(pendingSenders.begin(), pendingSenders.end(), channel) == pendingSenders.end())
			{
				pendingSenders.push_back(channel);
			}

			c->OnIncomingMessage(from, msg);
		}
	}
}

void Account::irc_onUserJoin(const std::wstring& channel, const std::wstring& user, const std::wstring& host) 
{
	Channel* c = findChannel(channel);

	if(c == 0)
	{
		c = setChannel(L"", channel, L"", channel, L"", false, true);
	}

	{
		c->addUser(user, host);
		c->setPresence(WTW_PRESENCE_ONLINE);

		OnlineUser ou(user, host);
		if(_wcsicmp(ou.getNick().c_str(), utils::text::toWide(currentNick).c_str()) == 0)
		{
			c->openChannel();
		}
	}
}

void Account::irc_onUserPart(const std::wstring& channel, const std::wstring& user, const std::wstring& host, const std::wstring& reason /*= L""*/) 
{
	Channel* c = findChannel(channel);

	if(c)
	{
		if(user == utils::text::toWide(currentNick))
		{
			c->setPresence(WTW_PRESENCE_OFFLINE);
		}
		else
		{
			c->remUser(user, true, reason);
		}
	}
}

void Account::irc_onUserQuit(const std::wstring& user, const std::wstring& reason /*= L""*/) 
{
	for(Channels::iterator i = channels.begin(); i != channels.end(); ++i)
	{
		(*i)->remUser(user, true, reason);
	}
}

void Account::irc_onNickList(const std::wstring& channel, const StringVectorW& users)
{
	Channel* c = findChannel(channel);

	if(c) 
	{
		for(StringVectorW::const_iterator i = users.begin(); i != users.end(); ++i)
		{
			c->addUser(*i, L"", false);
		}
	}
}

void Account::irc_onTopic(const std::wstring& channel, const std::wstring& topic, const std::wstring& setBy)
{
	Channel* c = findChannel(channel);

	if(c != 0)
	{
		c->setTopic(topic, setBy);

		if(currentTabId == channel)
		{
			std::wstring header;
			c->getHeader(header);

			core->fnCall(WTW_CONFERENCE_WINDOW_SET_TITLE, static_cast<WTW_PARAM>(windowHandle), reinterpret_cast<WTW_PARAM>(header.c_str()));
		}
	}
}

void Account::irc_onCTCP(const std::wstring& target, const std::wstring& sender, const std::wstring& cmd, const std::wstring& params)
{
	if(cmd == L"ACTION" && callMessageHooks(target, sender, params, true) == true)
	{
		AbstractTab* tab = findTab(target);
		
		if(tab != 0 || (tab = initPrivateChat(target)) != 0)
		{
			tab->OnIncomingMessage(sender, params, true);
		}
	}
	else if(cmd == L"VERSION")
	{
		sendNotice(sender, boost::str(boost::wformat(L"protoIRC@WTW2 v%1%.%2%.%3%.%4% by adrian_007") % V_GET_MAJOR(PLUGIN_VERSION) % V_GET_MINOR(PLUGIN_VERSION) % V_GET_REVISION(PLUGIN_VERSION) % V_GET_BUILD(PLUGIN_VERSION)), L"VERSION");
	}

	core->logFormatted(1, PROTO_CLASS, L"%s / %s: [%s] '%s'", target.c_str(), sender.c_str(), cmd.c_str(), params.c_str());
}

void Account::irc_onNick(const std::wstring& oldNick, const std::wstring& newNick)
{
	for(Channels::iterator i = channels.begin(); i != channels.end(); ++i)
	{
		if((*i)->hasUser(oldNick))
		{
			(*i)->nickChange(oldNick, newNick);
		}
	}
}

void Account::irc_onAway(const std::wstring& sender, bool away)
{
	if(sender == utils::text::toWide(currentNick))
	{
		sendPresence(away ? WTW_PRESENCE_AWAY : WTW_PRESENCE_ONLINE);

		for(Channels::iterator i = channels.begin(); i != channels.end(); ++i)
		{
			if((*i)->hasTab())
			{
				(*i)->addMessage(Channel::MESSAGE_INFO, L"", (away ? L"Tryb AWAY aktywny" : L"Tryb AWAY nieaktywny"), false, false, false);
			}
		}
	}
}

void Account::OnKick(const std::wstring& channel, const std::wstring& who, const std::wstring& victim, const std::wstring& reason)
{
	Channel* c = findChannel(channel);

	if(c) 
	{
		c->OnKick(who, victim, reason);
	}
}

void Account::OnMode(const std::wstring& channel, const std::wstring& who, const std::wstring& user, const std::wstring& mode) 
{
	Channel* c = findChannel(channel);

	if(c) 
	{
		c->OnMode(who, user, mode);
	}
}

void Account::OnUmode(const std::wstring& /*mode*/) 
{

}

void Account::OnNotice(const std::wstring& channel, const std::wstring& from, const std::wstring& message)
{
	Channel* c = findChannel(channel);

	if(c) 
	{
		c->OnNotice(from, message);
	}
	else
	{
//		ScopedLock mtx(cs);

		for(Channels::iterator i = channels.begin(); i != channels.end(); ++i)
		{
			(*i)->OnNotice(from, message, true);
		}
	}
}

void Account::OnError(const std::wstring& channel, const std::wstring& message) 
{
	Channel* c = findChannel(channel);

	if(c)
	{
//		c->putErrorMessage(message);
	}
}

PrivateChatTab* Account::initPrivateChat(const std::wstring& nick)
{
	PrivateChatTab* priv = findPrivateChat(nick);

	if(priv == 0)
	{
		priv = new PrivateChatTab(nick, this);
		privChats.push_back(priv);
	}
	else
	{
		priv->addTab();
	}

	wtwConferenceTab tab;
	initStruct(tab);
	tab.tabId = nick.c_str();
	core->fnCall(WTW_CONFERENCE_TAB_SWITCH_TO, this->windowHandle, reinterpret_cast<WTW_PARAM>(&tab));

	return priv;
}

WTW_PTR Account::funcChannelInit(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwContactDef* cnt = (wtwContactDef*)wParam;
	Account* acc = (Account*)ptr;

	core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"OnChannelInit: %p, netId = %d", ptr, acc->netId);

	if(acc->netId == cnt->netId && wcscmp(cnt->netClass, PROTO_CLASS) == 0)
	{
		acc->OnChannelInit(cnt);
	}
	return 1;
}

WTW_PTR Account::funcMessageSend(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwMessageDef* m = (wtwMessageDef*)wParam;
	Account* acc = (Account*)ptr;

	if(acc->netId == m->contactData.netId && wcscmp(m->contactData.netClass, PROTO_CLASS) == 0)
	{
		m->msgFlags |= WTW_MESSAGE_FLAG_CONFERENCE | WTW_MESSAGE_FLAG_OUTGOING;
		m->msgFlags &= ~WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_INCOMING;
		acc->OnMessageSend(m);
	}
	return 1;
}

WTW_PTR Account::funcPresenceSet(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) 
{
	wtwPresenceDef* prs = (wtwPresenceDef*)wParam;
	Account* acc = (Account*)ptr;

	const wchar_t* status = L"unavailable";

	switch(prs->curStatus)
	{
	case WTW_PRESENCE_ONLINE:
	case WTW_PRESENCE_CHAT:
		{
			status = L"available";
			break;
		}
	case WTW_PRESENCE_AWAY:
	case WTW_PRESENCE_XA:
	case WTW_PRESENCE_DND:
	case WTW_PRESENCE_INV:
		{
			status = L"away";
			break;
		}
	}

	acc->OnPresenceChange(status);
	return 0;
}

WTW_PTR Account::funcPresenceMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) 
{
	wtwMenuItemDef* mid = (wtwMenuItemDef*)wParam;
	wtwMenuPopupInfo* pi = (wtwMenuPopupInfo*)lParam;
	Account* acc = (Account*)ptr;

	wchar_t buf[1024];
	int netId;

	if(swscanf(mid->itemId, L"IRC/%d/Presence/%s", &netId, buf) == 2 && netId == acc->netId)
	{
		acc->OnPresenceChange(buf);
	}
	return 0;
}

WTW_PTR Account::funcMoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	return 0;
}

WTW_PTR Account::funcRemoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwContactDef* cnt = reinterpret_cast<wtwContactDef*>(wParam);
	Account* acc = reinterpret_cast<Account*>(ptr);

	if(cnt && acc && cnt->netId == acc->netId)
	{
		WTW_PARAM hCnt = 0;
		
		core->fnCall(WTW_CTL_CONTACT_FIND_EX, reinterpret_cast<WTW_PARAM>(cnt), (WTW_PARAM)&hCnt);

		if(hCnt)
		{
			acc->removeChannel(cnt->id);
			core->fnCall(WTW_CTL_CONTACT_DELETE, hCnt, 0);
		}
	}
	return 0;
}

WTW_PTR Account::funcMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) 
{
	wtwMenuCallbackEvent* event = reinterpret_cast<wtwMenuCallbackEvent*>(wParam);
	Account* acc = reinterpret_cast<Account*>(ptr);

	if(!event->pInfo->iContacts)
	{
		return 0;
	}

	if(wcscmp(event->pInfo->pContacts[0].netClass, PROTO_CLASS))
	{
		return 0;
	}

	if(event->pInfo->pContacts[0].netId != acc->netId)
	{
		return 0;
	}

	if(acc->isConnected())
	{
		std::wstring prefix = str(wformat(L"IRC/%1%/") % acc->netId);
		std::wstring itemId;

		Channel* c = acc->findChannel(event->pInfo->pContacts[0].id);

		if(c)
		{
			if(c->isSet(Channel::FLAG_JOINED))
			{
				itemId = prefix + L"part";
				event->slInt.add(event->itemsToShow, itemId.c_str());
			}
			else
			{
				itemId = prefix + L"join";
				event->slInt.add(event->itemsToShow, itemId.c_str());
				event->slInt.add(event->itemsToShow, WTW_MIID_CT_EDIT);
				event->slInt.add(event->itemsToShow, WTW_MIID_CT_DELETE);		
			}
		}
	}
	else
	{
		event->slInt.add(event->itemsToShow, WTW_MIID_CT_EDIT);
		event->slInt.add(event->itemsToShow, WTW_MIID_CT_DELETE);		
	}

	return 0;
}

WTW_PTR Account::funcMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) 
{
	wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;
	wtwMenuItemDef* menu = (wtwMenuItemDef*)wParam;
	Account* acc = (Account*)ptr;

	if(!nfo || !menu || !acc)
	{
		return 0;
	}

	if(nfo->iContacts == 1) 
	{
		wtwContactDef* cnt = &nfo->pContacts[0];
		Channel* c = acc->findChannel(cnt->id);

		if(c) 
		{
			if(wcsstr(menu->itemId, L"/join") != 0)
			{
				c->openChannel(true);
			}

			if(wcsstr(menu->itemId, L"/part") != 0)
			{
				c->part();
			}
		}
	}
	return 0;
}

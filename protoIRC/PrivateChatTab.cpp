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
#include "Account.h"
#include "../utils/text.hpp"

PrivateChatTab::PrivateChatTab(const std::wstring& _nick, Account* parent) : OnlineUser(_nick)
{
	this->parent = parent;
	addTab();
}

PrivateChatTab::~PrivateChatTab(void)
{
}

void PrivateChatTab::addTab(int showMethod /*= -1*/)
{
	getWindowHandle();
}

bool PrivateChatTab::getHeader(std::wstring& header) const
{
	header = boost::str(boost::wformat(L"%1% - %2%") % parent->server % this->getNick());
	return true;
}

void PrivateChatTab::addMessage(int messageType, const std::wstring& from, const std::wstring& message, bool incoming, bool notify, bool flash)
{
	wtwConferenceMessage m; 
	initStruct(m); 

	std::wstring whatify = parent->getWhatifyString();
	m.whatifyHint = whatify.c_str();

	m.message.contactData.netClass	= PROTO_CLASS; 
	m.message.contactData.netId		= parent->netId; 
	m.message.contactData.id		= from.c_str();

	m.message.msgTime		= time(0); 
	m.message.msgMessage	= message.c_str();

	m.flags							|= CONFERENCE_MESSAGE_FLAG_NO_WHATIFY;

	switch(messageType)
	{
	case AbstractTab::MESSAGE_INFO:
		{
			m.message.msgFlags		= WTW_MESSAGE_FLAG_INFO;
			break;
		}
	case AbstractTab::MESSAGE_WARNING:
		{
			m.message.msgFlags		= WTW_MESSAGE_FLAG_WARNING;
			break;
		}
	case AbstractTab::MESSAGE_ERROR:
		{
			m.message.msgFlags		= WTW_MESSAGE_FLAG_ERROR;
			break;
		}
	case AbstractTab::MESSAGE_NORMAL:
		{
			m.message.msgFlags		= WTW_MESSAGE_FLAG_CONFERENCE | WTW_MESSAGE_FLAG_NOAUTORECV | (incoming ? WTW_MESSAGE_FLAG_INCOMING : WTW_MESSAGE_FLAG_OUTGOING);
			m.flags					&= ~CONFERENCE_MESSAGE_FLAG_NO_WHATIFY;
			break;
		}
	default:
		{
			return;
		}
	}

	if(notify == false)
	{
		m.flags |= CONFERENCE_MESSAGE_FLAG_NO_WHATIFY;
	}

	m.nick = from.c_str();
	m.tabId = nick.c_str();

	if(flash)
	{
		m.flags	|= CONFERENCE_MESSAGE_FLAG_FLASH; 
	}

	WTW_PARAM handle = getWindowHandle();

	if(handle)
	{
		core->fnCall(WTW_CONFERENCE_WINDOW_ADD_MESSAGE, handle, m);
	}
}

void PrivateChatTab::OnOutgoingMessage(wtwMessageDef* msg)
{
	WTW_PARAM handle = getWindowHandle();

	if(handle) 
	{
		parent->sendMessage(nick, msg->msgMessage);
		addMessage(MESSAGE_NORMAL, utils::text::toWide(parent->currentNick), msg->msgMessage, false, false, false);

		/*const std::wstring& message = msg->msgMessage;

		bool drop = false;
		if(message[0] == L'/')
		{
		//	handleUserCommand(message);
		}
		//else if(isSet(FLAG_JOINED))
		{
			parent->sendMessage(channel, message);
			addMessage(MESSAGE_NORMAL, utils::text::toWide(parent->currentNick), message, false, false, false);
		}
		else
		{
			addMessage(MESSAGE_ERROR, L"", L"Nie jeste� obecny na kanale!", false, false, false);
		}*/
	}
}

void PrivateChatTab::OnIncomingMessage(const std::wstring& from, const std::wstring& message, bool ctcpAction /*= false*/)
{
	addMessage(MESSAGE_NORMAL, from, message, true, true, true);
}

WTW_PARAM PrivateChatTab::getWindowHandle()
{
	WTW_PARAM handle = parent->getWindowHandle();

	if(handle != 0)
	{
		wtwConferenceTab tab;
		initStruct(tab);

		tab.tabId = nick.c_str();
		tab.tabName = nick.c_str();
		tab.tabIcon = WTW_GRAPH_ID_CONF_USER;
		tab.tabClass = CFTC_SINGLE_USER;
		tab.flags = CFTF_CLOSEABLE | CFTF_AUTO_MARK;

		if(core->fnCall(WTW_CONFERENCE_TAB_ADD, handle, tab) == S_OK)
		{
			//core->fnCall(WTW_CONFERENCE_WINDOW_SHOW_WINDOW, handle, CWSW_SHOW_NORMAL);

			/*wtwConferenceCmdEntry e;
			e.caption = L"Part";
			e.toolTip = L"Opu�� kana�";
			e.itemId = L"protoIRC/partOrJoin";
			e.pWnd = (WTW_PTR)handle;
			e.tabId = channel.c_str();

			core->fnCall(WTW_CONFERENCE_CMD_ADD, e, 0);*/
		}
	}

	return handle;
}

WTW_PTR PrivateChatTab::onWindowEvent(wtwConferenceWindowEvent* cb, WTW_PARAM lParam)
{
	switch(cb->event) 
	{
	case CONFERENCE_EVENT_SEND_MESSAGE: 
		{
			wtwMessageDef* msg = (wtwMessageDef*)lParam;
			OnOutgoingMessage(msg);
			break;
		}
	/*
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
					e.pWnd = cb->windowPtr;
					e.tabId = channel.c_str();

					if(isSet(FLAG_JOINED))
					{
						part();
						e.caption = L"Join";
						e.toolTip = L"Do��cz do kana�u";
					}
					else 
					{
						join();
						e.caption = L"Part";
						e.toolTip = L"Opu�� kana�";
					}
					core->fnCall(WTW_CONFERENCE_CMD_MOD, e, 0);
				}		
			}
			break;
		}
	*/
	}
	return 0;
}

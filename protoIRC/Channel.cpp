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
#include "Account.h"
#include "Channel.h"
#include "Utils.hpp"

#include "../utils/text.hpp"

using boost::str;
using boost::wformat;

Channel::Channel(Account* parent, const std::wstring& channel, const std::wstring& group, const std::wstring& name, const std::wstring& uuid, const std::wstring& nick, const std::wstring& passwd) : AbstractTab(), Flags()
{
	this->parent = parent;
	this->channel = channel;
	this->group = group;
	this->name = name;
	this->uuid = uuid;
	this->nick = nick;
	this->passwd = passwd;
	this->tempNick = nick;
	this->archId = -1;
}

Channel::~Channel()
{
}

void Channel::addTab(int showMethod /*= -1*/)
{
	WTW_PARAM handle = parent->getWindowHandle();

	if(handle != 0)
	{
		wtwConferenceTab tab;
		initStruct(tab);

		tab.tabId = channel.c_str();
		tab.tabName = channel.c_str();
		tab.tabIcon = WTW_GRAPH_ID_CHAT;//L"IRC/Icon/available";
		tab.tabClass = CFTC_MULTI_USER;
		tab.flags = CFTF_CLOSEABLE | CFTF_AUTO_MARK;

		if(core->fnCall(WTW_CONFERENCE_TAB_ADD, handle, tab) == S_OK)
		{
			wtwConferenceCmdEntry e;
			e.caption = L"Part";
			e.toolTip = L"Opu�� kana�";
			e.itemId = L"protoIRC/partOrJoin";
			e.pWnd = (WTW_PTR)handle;
			e.tabId = channel.c_str();

			core->fnCall(WTW_CONFERENCE_CMD_ADD, e, 0);

			for(OnlineUserList::iterator i = users.begin(); i != users.end(); ++i)
			{
				addUser(*(*i), (*i)->getHost(), false);
			}
		}

		if(showMethod >= 0)
		{
			core->fnCall(WTW_CONFERENCE_WINDOW_SHOW_WINDOW, handle, showMethod);
		}
	}
}

void Channel::remTab()
{
	WTW_PARAM handle = parent->getWindowHandle();

	if(handle != 0)
	{
		wtwConferenceTab tab;
		initStruct(tab);

		tab.tabId = channel.c_str();
		
		core->fnCall(WTW_CONFERENCE_TAB_DELETE, handle, tab);
	}
}

bool Channel::hasTab() const
{
	WTW_PARAM handle = parent->getWindowHandle();

	if(handle != 0)
	{
		wtwConferenceTab tab;
		initStruct(tab);

		tab.tabId = channel.c_str();
		tab.tabName = channel.c_str();
		tab.flags = CFTF_VALID_NAME;

		return core->fnCall(WTW_CONFERENCE_TAB_EXISTS, handle, tab) == S_OK;
	}

	return false;
}

void Channel::addUser(const std::wstring& user, const std::wstring& host, bool notify /*= true*/)
{
	OnlineUser* ou = getUser(user);

	if(ou == 0)
	{
		ou = new OnlineUser(user, host);
		users.push_back(ou);
	}

	if(hasTab())
	{
		WTW_PARAM handle = getWindowHandle();

		wtwConferenceContact cnt;
		initStruct(cnt);

		cnt.flags = WTW_CONFERENCE_CONTACT_FLAG_UPDATE_NAME | WTW_CONFERENCE_CONTACT_FLAG_UPDATE_DATA | WTW_CONFERENCE_CONTACT_FLAG_UPDATE_ICON | WTW_CONFERENCE_CONTACT_FLAG_UPDATE_SCLS;

		if(ou->isSet(OnlineUser::FLAG_VOICE))
		{
			cnt.sortClass -= 2;
			cnt.iconId = WTW_GRAPH_ID_CONF_MOD;
		}

		if(ou->isSet(OnlineUser::FLAG_OP)) 
		{
			cnt.sortClass -= 3;
			cnt.iconId = WTW_GRAPH_ID_CONF_ADMIN;
		}

		// check if user is our account
		if(*ou == parent->currentNick) 
		{
			cnt.sortClass -= 1;
			cnt.iconId = WTW_GRAPH_ID_CONF_OWNER;

			if(ou->isSet(OnlineUser::FLAG_OP)) 
			{
				setFlag(FLAG_OP);
			} 
			else
			{
				unsetFlag(FLAG_OP);
			}

			if(ou->isSet(OnlineUser::FLAG_VOICE))
			{
				setFlag(FLAG_VOICE);
			}
			else
			{
				unsetFlag(FLAG_VOICE);
			}
		} 

		if(cnt.iconId == 0) 
		{
			cnt.iconId = WTW_GRAPH_ID_CONF_USER;
		}

		std::wstring hint = str(wformat(L"<table><tr><td>Nick:</td><td>%1%</td></tr><tr><td>Host:</td><td>%2%</td></tr></table>") % ou->getNick() % ou->getHost());

		cnt.name = *ou;
		cnt.contactInfo.id = *ou;
		cnt.contactHint = hint.c_str();
		cnt.userData = static_cast<void*>(ou);
		cnt.tabId = channel.c_str();

		core->fnCall(WTW_CONFERENCE_WINDOW_CONTACT_ADD, handle, cnt);

		if(notify) 
		{
			std::wstring text = str(wformat(L"U�ytkownik %1% (%2%) do��czy� do kana�u") % ou->getNick() % ou->getHost());
			addMessage(MESSAGE_INFO, *ou, text, false, false, false);
		}
	}
}

void Channel::remUser(const std::wstring& user, bool notify /*= true*/, const std::wstring& reason /*= L""*/)
{
	OnlineUser* ou = 0;

	for(OnlineUserList::iterator i = users.begin(); i != users.end();)
	{
		if((*i)->equals(user))
		{
			ou = *i;
			i = users.erase(i);
		}
		else
		{
			++i;
		}
	}

	if(ou == 0)
	{
		return;
	}

	//users.erase(std::remove(users.begin(), users.end(), ou), users.end());

	if(hasTab())
	{
		WTW_PARAM handle = getWindowHandle();

		wtwConferenceContact cnt;
		initStruct(cnt);

		cnt.contactInfo.id = *ou;
		cnt.tabId = channel.c_str();

		if(core->fnCall(WTW_CONFERENCE_WINDOW_CONTACT_DELETE, handle, cnt) == S_OK && notify) 
		{
			std::wstring text;
			if(reason.empty()) 
			{
				text = str(wformat(L"U�ytkownik %1% opu�ci� kana�") % ou->getNick());
			} 
			else
			{
				text = str(wformat(L"U�ytkownik %1% opu�ci� kana� (%2%)") % ou->getNick() % reason);
			}

			addMessage(MESSAGE_INFO, *ou, text, false, false, false);
		}
	}

	delete ou;
}

bool Channel::hasUser(const std::wstring& user)
{
	return getUser(user) != 0;
}

bool Channel::getHeader(std::wstring& header) const
{
	if(topic.empty() == false)
	{
		header = boost::str(boost::wformat(L"%1% - %2%") % channel % topic);
	}
	else
	{
		header = channel;
	}

	return true;
}

OnlineUser* Channel::getUser(const std::wstring& nick)
{
	for(OnlineUserList::iterator i = users.begin(); i != users.end(); ++i)
	{
		if(*(*i) == nick)
		{
			return *i;
		}
	}
	return 0;
}

void Channel::addMessage(int messageType, const std::wstring& from, const std::wstring& message, bool incoming, bool notify, bool flash)
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
	//m.message.msgSubject	= channel.c_str();

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
	m.tabId = channel.c_str();

	if(flash)
	{
		m.flags	|= CONFERENCE_MESSAGE_FLAG_FLASH; 
	}

	WTW_PARAM handle = getWindowHandle();

	if(handle)
	{
		core->fnCall(WTW_CONFERENCE_WINDOW_ADD_MESSAGE, handle, m);
	}

	{
		wtwArchiveEntryMessage arch;
		initStruct(arch);
		arch.chatID = archId;

		arch.message.contactData.netClass	= PROTO_CLASS; 
		arch.message.contactData.netId		= parent->netId; 
		arch.message.contactData.id			= from.c_str();

		arch.message.msgTime				= time(0); 
		arch.message.msgMessage				= message.c_str();
		//arch.message.msgSubject				= channel.c_str();
		arch.message.msgFlags				= m.message.msgFlags | WTW_MESSAGE_FLAG_CONFERENCE;

		core->fnCall(WTW_ARCH_WRITE_MESSAGE, reinterpret_cast<WTW_PARAM>(&arch), 0);

		archId = arch.chatID;
	}
}

bool Channel::equals(const std::wstring& id) const
{
	size_t chLen = channel.size();
	size_t idLen = id.size();

	if(chLen == idLen)
	{
		return _wcsnicmp(channel.c_str(), id.c_str(), chLen) == 0;
	}

	return false;
}

WTW_PARAM Channel::getWindowHandle()
{
	WTW_PARAM handle = parent->getWindowHandle();

	if(handle != 0)
	{
		wtwConferenceTab tab;
		initStruct(tab);

		tab.tabId = channel.c_str();
		tab.tabName = channel.c_str();
		tab.tabIcon = L"IRC/Icon/available";
		tab.tabClass = CFTC_MULTI_USER;

		if(core->fnCall(WTW_CONFERENCE_TAB_ADD, handle, tab) == S_OK)
		{
			core->fnCall(WTW_CONFERENCE_WINDOW_SHOW_WINDOW, handle, CWSW_SHOW_NORMAL);

			wtwConferenceCmdEntry e;
			e.caption = L"Part";
			e.toolTip = L"Opu�� kana�";
			e.itemId = L"protoIRC/partOrJoin";
			e.pWnd = (WTW_PTR)handle;
			e.tabId = channel.c_str();

			core->fnCall(WTW_CONFERENCE_CMD_ADD, e, 0);

			for(OnlineUserList::iterator i = users.begin(); i != users.end(); ++i)
			{
				addUser(*(*i), (*i)->getHost(), false);
			}
		}
	}

	return handle;
}

void Channel::openChannel(bool forceShow /*= false*/)
{
	addTab(forceShow ? CWSW_SHOW_NORMAL : -1);

	if(Flags::isSet(FLAG_JOINED) == false)
	{
		join();
	}
}

void Channel::join()
{
	addTab();
	parent->joinChannel(this->channel, this->passwd);
	setFlag(FLAG_JOINED);
}

void Channel::part(const std::wstring& reason /*= L""*/, bool sendCommand /*= true*/)
{
	clearUserlist();

	if(isSet(FLAG_JOINED) && sendCommand)
	{
		parent->partChannel(channel, reason);
	}

	topic.clear();
	setPresence(WTW_PRESENCE_OFFLINE);
	unsetFlag(FLAG_JOINED);
}

void Channel::setPresence(int presence)
{
	wtwContactDef c;
	wtwPresenceDef p;
	initStruct(p);
	initStruct(c);
	p.pContactData = &c;

	c.id		= channel.c_str();
	c.netClass	= PROTO_CLASS;
	c.netId		= parent->netId;

	p.curStatus		= presence;
	p.curDescription = topic.c_str();
	p.curTimeStamp = time(0);

	core->fnCall(WTW_CTL_SET_CONTACT_STATE, reinterpret_cast<WTW_PARAM>(&p), 0);
}

void Channel::nickChange(const std::wstring& oldNick, const std::wstring& newNick)
{
	remUser(oldNick, false);
	addUser(newNick, L"", false);

	addMessage(MESSAGE_INFO, newNick, str(wformat(L"%1% zmieni� nick na %2%") % oldNick % newNick), false, false, false);
}

void Channel::setTopic(const std::wstring& newTopic, const std::wstring& setBy)
{
	topic = newTopic;

	if(hasTab()) 
	{
		std::wstring msg;
		if(setBy.empty() == true)
		{
			msg = str(wformat(L"Temat: %1%") % topic);
		}
		else
		{
			if(topic.empty() == true)
			{
				msg = str(wformat(L"Temat zosta� usuni�ty przez %1%") % setBy);
			}
			else
			{
				msg = str(wformat(L"Temat zmieniony przez %1% na: %2%") % setBy % topic);
			}
		}

		addMessage(MESSAGE_INFO, L"", msg, true, false, false);
	}

	setPresence(WTW_PRESENCE_ONLINE);
}

void Channel::OnIncomingMessage(const std::wstring& from, const std::wstring& message, bool ctcpAction /*= false*/)
{
	if(ctcpAction == true)
	{
		addMessage(MESSAGE_INFO, from, str(wformat(L"** %1% %2%") % from % message), true, true, true);
	}
	else
	{
		addMessage(MESSAGE_NORMAL, from, message, true, true, true);
	}
}

void Channel::OnOutgoingMessage(wtwMessageDef* msg)
{
	WTW_PARAM handle = getWindowHandle();

	if(handle) 
	{
		const std::wstring& message = msg->msgMessage;

		bool drop = false;
		if(message[0] == L'/')
		{
			handleUserCommand(message);
		}
		else if(isSet(FLAG_JOINED))
		{
			parent->sendMessage(channel, message);
			addMessage(MESSAGE_NORMAL, utils::text::toWide(parent->currentNick), message, false, false, false);
		}
		else
		{
			addMessage(MESSAGE_ERROR, L"", L"Nie jeste� obecny na kanale!", false, false, false);
		}
	}
}

void Channel::OnActionCommand(const std::wstring& from, const std::wstring& msg)
{
//	putInfoMessage(str(wformat(L"** %1% %2%") % from % msg), true);
}

void Channel::OnCTCPReply(const std::wstring& from, const std::wstring& reply) {
/*	WTW_PARAM handle = getWindowHandle();

	wtwConferenceContact cnt;
	cnt.contactInfo.id = from.c_str();
	cnt.tabId = channel.c_str();

	if(core->fnCall(WTW_CONFERENCE_WINDOW_IS_CT_PRESENT, (WTW_PARAM)handle, cnt) == S_OK) 
	{
		std::wstring m = str(wformat(L"REPLY from %1%: %2%") % from % reply);
		putInfoMessage(m);
	}*/
}

void Channel::OnKick(const std::wstring& who, const std::wstring& victim, const std::wstring& reason) {
/*	std::wstring r = reason;

	if(r.empty() || who == reason)
	{
		r = L"<< brak >>";
	}

	std::wstring msg = str(wformat(L"%1% zosta� wykopany z kana�u! Oprawca: %2%, pow�d: %3%") % victim % who % r);

	putInfoMessage(msg);

	if(victim == tempNick)
	{
		unsetFlag(FLAG_JOINED);
		clearUserlist();
		topic.clear();
		setPresence(WTW_PRESENCE_OFFLINE);
	}
	else
	{
		remUser(victim, false);
	}*/
}

void Channel::OnMode(const std::wstring& who, const std::wstring& user, const std::wstring& mode) {
/*	WTW_PARAM handle = getWindowHandle();

	int flags = 0;

	wtwConferenceContact cnt;
	cnt.contactInfo.id = who.c_str();
	cnt.tabId = channel.c_str();

	bool present = core->fnCall(WTW_CONFERENCE_WINDOW_GET_CONTACT, (WTW_PARAM)handle, cnt) == S_OK;

	flags |= (int)cnt.userData;

	if(mode[0] == L'+')
	{
		switch(mode[1])
		{
		case L'o':
			{
				flags |= FLAG_OP; 
				break;
			}
		case L'v':
			{
				flags |= FLAG_VOICE;
				break;
			}
		case L'b':
			{
				flags |= FLAG_BANNED;
				break;
			}
		}
	} 
	else
	{
		switch(mode[1]) 
		{
		case L'o':
			{
				flags &= ~FLAG_OP;
				break;
			}
		case L'v':
			{
				flags &= ~FLAG_VOICE;
				break;
			}
		case L'b':
			{
				flags &= ~FLAG_BANNED;
				break;
			}
		}
	}

	if(present) 
	{
		remUser(user, false);

		if((flags & FLAG_BANNED) == 0)
		{
			addUser(user, false, flags);
		}

		putInfoMessage(str(wformat(L"MODE %1% zmienione na %2% przez %3%") % user % mode % who));
	}
	else
	{
		putInfoMessage(str(wformat(L"MODE kana�u (%1%) zmienione na %2% przez %3%") % mode % user % who));
	}*/
}

void Channel::OnNotice(const std::wstring& from, const std::wstring& message, bool checkPresence /*= false*/) {
/*	if(isSet(FLAG_JOINED) == false)
	{
		return;
	}

	WTW_PARAM handle = getWindowHandle();

	if(checkPresence)
	{
		wtwConferenceContact cnt;
		cnt.contactInfo.id = from.c_str();
		cnt.tabId = channel.c_str();
		checkPresence = core->fnCall(WTW_CONFERENCE_WINDOW_IS_CT_PRESENT, (WTW_PARAM)handle, cnt) == S_OK;
	} 
	else
	{
		checkPresence = true;
	}

	if(checkPresence)
	{
		std::wstring m = str(wformat(L"%1% - %2%") % from % message);
		putInfoMessage(m);
	}*/
}

void Channel::handleUserCommand(const std::wstring& message)
{
	std::wstring::size_type i = message.find(L" ");
	std::wstring cmd;
	std::wstring params;

	if(i == std::wstring::npos)
	{
		cmd = message.substr(1);
	}
	else
	{
		cmd = message.substr(1, i-1);
		params = message.substr(i+1);
	}

	if(cmd == L"raw")
	{
		if(params.empty())
		{
			addMessage(MESSAGE_ERROR, L"", L"Komenda /raw wymaga parametr�w", false, false, false);
		}
		else
		{
			parent->getSendBuffer() << params;
		}
	}
	else if(cmd == L"me")
	{
		if(params.empty())
		{
			addMessage(MESSAGE_ERROR, L"", L"Komenda /me wymaga parametru!", false, false, false);
		} 
		else
		{
			parent->sendMessage(channel, params, true);
			addMessage(MESSAGE_INFO, L"", str(wformat(L"** %1% %2%") % nick % params), false, false, false);
		}
	}
	else if(cmd == L"nick")
	{
		if(params.empty())
		{
			addMessage(MESSAGE_ERROR, L"", L"Komenda /nick wymaga parametru!", false, false, false);
		}
		else
		{
			parent->changeNick(params);
		}
	}
	else if(cmd == L"part")
	{
		if(isSet(FLAG_JOINED))
		{
			part(params);
		}
	}
	else if(cmd == L"join")
	{
		if(params.empty())
		{
			if(isSet(FLAG_JOINED) == false)
			{
				join();
			}
		}
		else
		{
			StringVectorW parts = Utils::split(params);
			
			if(parts.size() > 0)
			{
				parent->joinChannel(parts[0], parts.size() > 1 ? parts[1] : L"");
			}
		}
	}
	else if(cmd == L"topic")
	{
		parent->setTopic(channel, params);
	}
	else if(cmd == L"away")
	{
		parent->setPresence(L"away", params);
	}
	else if(cmd == L"back")
	{
		parent->setPresence(L"available");
	}
	else if(cmd == L"remove")
	{
		setFlag(FLAG_TEMPORARY);
	}
	else if(cmd == L"save")
	{
		unsetFlag(FLAG_TEMPORARY);
	}

	/*
	else if(cmd == L"quit")
	{
		//closeChannel(false);
		//parent->disconnect(params);
		//parent->setPresence(L"unavailable");
	}
	else if(cmd == L"help" || cmd == L"pomoc")
	{
		std::wstring m = L"Lista dost�pnych komend:";
		m += L"\r\n/help - wy�wietla ten tekst...";
		m += L"\r\n/me [tekst] - m�wimy w trzeciej osobie (cokolwiek to znaczy...)";
		m += L"\r\n/part [pow�d] - wychodzimy z kana�u (pow�d opcjonalny)";
		m += L"\r\n/join - wchodzimy na kana�";
		//m += L"\r\n/quit [pow�d] - roz��czamy si� z serwerem (pow�d opcjonalny)";
		m += L"\r\n/nick [nowy_nick] - zmiana obecnego nick'u";
		m += L"\r\n/topic [temat] - ustawia temat dla kana�u";
		putInfoMessage(m);
	}
	else
	{
		putErrorMessage(cmd + L" - nieznana/nieobs�ugiwana komenda!");
	}*/
}

void Channel::clearUserlist()
{
	while(users.size())
	{
		remUser(*users.back(), false);
		//users.pop_back();
	}

/*	WTW_PARAM handle = getWindowHandle();

	wtwConferenceContacts cc;
	initStruct(cc);
	cc.contactCount = -1;

	cc.contactCount = (int)core->fnCall(WTW_CONFERENCE_WINDOW_GET_CONTACTS, (WTW_PARAM)handle, (WTW_PARAM)&cc);
	if(cc.contactCount > 0) 
	{
		cc.pContacts = new wtwConferenceContact[cc.contactCount];
		initStruct(cc.pContacts[0]);
		core->fnCall(WTW_CONFERENCE_WINDOW_GET_CONTACTS, (WTW_PARAM)handle, (WTW_PARAM)&cc);

		for(int i = 0; i < cc.contactCount; ++i)
		{
			core->fnCall(WTW_CONFERENCE_WINDOW_CONTACT_DELETE, (WTW_PARAM)handle, (WTW_PARAM)&cc.pContacts[i]);
		}

		delete[] cc.pContacts;
	}*/
}

WTW_PTR Channel::onWindowEvent(wtwConferenceWindowEvent* cb, WTW_PARAM lParam)
{
	switch(cb->event) 
	{
	case CONFERENCE_EVENT_SEND_MESSAGE: 
		{
			wtwMessageDef* msg = (wtwMessageDef*)lParam;
			OnOutgoingMessage(msg);
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
	case CONFERENCE_EVENT_CT_LDBLCLK:
		{
			wtwConferenceContact* cnt = (wtwConferenceContact*)lParam;
		
			if(cnt)
			{
				parent->initPrivateChat(cnt->name);
			}
			break;
		}
	case CONFERENCE_EVENT_CT_RCLICK:
		{
			wtwConferenceContact* cnt = (wtwConferenceContact*)lParam;
			if(cnt)
			{
				bool me = wcscmp(cnt->name, tempNick.c_str()) == 0;
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

				if(isSet(FLAG_OP)) 
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
						parent->ctcpRequest(cnt->contactInfo.id, L"FINGER");
						break;
					}
				case Channel::MENU_SHOW_CLIENT_INFO: 
					{
						parent->ctcpRequest(cnt->contactInfo.id, L"VERSION");
						break;
					}
				case Channel::MENU_PING: 
					{
						parent->ctcpRequest(cnt->contactInfo.id, L"PING");
						break;
					}
				case Channel::MENU_KICK: 
					{
						parent->kickUser(channel, cnt->contactInfo.id);
						break;
					}
				case Channel::MENU_SLAP:
					{
						std::wstring slap = str(wformat(L"/me slaps %1% around a bit with a large trout.") % cnt->name);
						handleUserCommand(slap);
						break;
					}
				case Channel::MENU_REM_OP:
					{
						parent->setMode(channel, cnt->contactInfo.id, L"-o");
						break;
					}
				case Channel::MENU_ADD_OP:
					{
						parent->setMode(channel, cnt->contactInfo.id, L"+o");
						break;
					}
				case Channel::MENU_REM_VOICE:
					{
						parent->setMode(channel, cnt->contactInfo.id, L"-v");
						break;
					}
				case Channel::MENU_ADD_VOICE:
					{
						parent->setMode(channel, cnt->contactInfo.id, L"+v");
						break;
					}
				}
			}
			break;
		}
	}
	return 0;
}

WTW_PTR Channel::funcChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
/*	Channel* c = (Channel*)ptr;
	wtwConferenceWindowEvent* cb = (wtwConferenceWindowEvent*)wParam;
	
	if(c && cb) 
	{
		switch(cb->event) 
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
						e.tabId = channel.c_str();

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
		}
	}*/
	return 0;
}

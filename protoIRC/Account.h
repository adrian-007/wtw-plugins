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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <vector>
//#include "IRCConnection.h"
#include "ServerSocket.hpp"
#include "AbstractTab.hpp"

class Channel;
class PrivateChatTab;
class IRCManager;

class Account : public ServerSocket 
{
public:
	typedef std::vector<Channel*> Channels;
	typedef std::vector<PrivateChatTab*> PrivateChats;

	Account(const std::wstring& server, const std::wstring& uuid, const std::wstring& name, const std::wstring& user = L"", const std::wstring& nick = L"", const std::wstring& pass = L"");
	void close();

	static std::wstring generateUUID();

	Channel* setChannel(const std::wstring& uuid, const std::wstring& channel, const std::wstring& group, const std::wstring& name, const std::wstring& pass, bool autojoin = false, bool temporary = false);
	void updateInfo();
	void updateChannels();

	std::wstring name;
	std::wstring server;
	std::wstring uuid;
	std::wstring user;
	std::wstring nick;
	std::wstring passwd;
	std::wstring quitMessage;

	unsigned short port;
	Channels channels;
	PrivateChats privChats;

	void setPresence(const wchar_t* presence, const std::wstring& description = L"");
	Channel* findChannel(const std::wstring& channel);
	PrivateChatTab* findPrivateChat(const std::wstring& nick);
	PrivateChatTab* initPrivateChat(const std::wstring& nick);
	AbstractTab* findTab(const std::wstring& id);

	WTW_PARAM getWindowHandle();

	std::wstring getWhatifyString() const;
private:
	friend class Channel;
	friend class IRCManager;

	void removeChannel(const std::wstring& channel);

	// server events
	void irc_onConnected();
	void irc_onDisconnected();
	void irc_onReady();
	void irc_onServerLog(const std::wstring& msg);
	void irc_onIncomingMessage(const std::wstring& channel, const std::wstring& from, const std::wstring& msg);
	void irc_onUserJoin(const std::wstring& channel, const std::wstring& user, const std::wstring& host);
	void irc_onUserPart(const std::wstring& channel, const std::wstring& user, const std::wstring& host, const std::wstring& reason = L"");
	void irc_onUserQuit(const std::wstring& user, const std::wstring& reason = L"");
	void irc_onNickList(const std::wstring& channel, const StringVectorW& users);
	void irc_onTopic(const std::wstring& channel, const std::wstring& topic, const std::wstring& setBy);
	void irc_onCTCP(const std::wstring& target, const std::wstring& sender, const std::wstring& cmd, const std::wstring& params);
	void irc_onNick(const std::wstring& oldNick, const std::wstring& newNick);
	void irc_onAway(const std::wstring& sender, bool away);

	void OnKick(const std::wstring& channel, const std::wstring& who, const std::wstring& victim, const std::wstring& reason);
	void OnMode(const std::wstring& channel, const std::wstring& who, const std::wstring& user, const std::wstring& mode);
	void OnUmode(const std::wstring& mode);
	void OnNotice(const std::wstring& channel, const std::wstring& from, const std::wstring& message);
	void OnError(const std::wstring& channel, const std::wstring& message);

	bool NickCompare(const std::wstring& nick)
	{
		return this->nick == nick;
	}

	void OnChannelInit(wtwContactDef* cnt);
	void OnMessageSend(wtwMessageDef* msg);

	void addPresenceMenuItem(const wchar_t* presence, const wchar_t* caption);
	void setMainPresence(const wchar_t* presence, bool add = false);
	void sendPresence(DWORD p);
	void OnPresenceChange(const wchar_t* presence);
	//bool callMessageHooks(const std::wstring& channel, const std::wstring& message, bool incoming);

	static WTW_PTR funcChannelInit(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMessageSend(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcPresenceSet(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcPresenceMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcRemoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcWindow(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);

	//int netId;
	//DWORD status;

	HANDLE onMenuRebuildHook;
	HANDLE protoFunc;
	HANDLE messageFunc;
	HANDLE presenceSetFunc;

	WTW_PTR windowHandle;

	std::vector<std::wstring> pendingSenders;
	std::wstring currentTabId;
};

#endif

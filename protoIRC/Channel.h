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

#pragma once

#include "Flags.hpp"
#include "AbstractTab.hpp"
#include "OnlineUser.hpp"

class Account;

class Channel : public AbstractTab, public Flags
{
public:
	typedef AbstractTab TabClass;
	typedef boost::intrusive_ptr<Channel> ChannelPtr;
	typedef std::vector<OnlineUser*> OnlineUserList;

	Channel(Account* parent, const std::wstring& channel, const std::wstring& group, const std::wstring& name, const std::wstring& uuid, const std::wstring& nick, const std::wstring& passwd);
	virtual ~Channel();

	enum
	{
		FLAG_JOINED					= 0x0001,
		FLAG_AWAY					= 0x0002,
		FLAG_OP						= 0x0004,
		FLAG_VOICE					= 0x0008,
		FLAG_BANNED					= 0x0010,
		FLAG_AUTOJOIN				= 0x0100,
		FLAG_TEMPORARY				= 0x0200
	};

	void addTab(int showMethod = -1);
	void remTab();
	bool hasTab() const;

	void addUser(const std::wstring& user, const std::wstring& host, bool notify = true);
	void remUser(const std::wstring& user, bool notify = true, const std::wstring& reason = L"");
	bool hasUser(const std::wstring& user);

	bool getHeader(std::wstring& header) const;

	void addMessage(int messageType, const std::wstring& from, const std::wstring& message, bool incoming, bool notify, bool flash);
	bool equals(const std::wstring& id) const;

	OnlineUser* getUser(const std::wstring& nick);
	//

	void join();
	void part(const std::wstring& reason = L"", bool sendCommand = true);

	void openChannel(bool forceShow = false);
	void clearUserlist();

	void setPresence(int presence);

	void nickChange(const std::wstring& oldNick, const std::wstring& newNick);

	void OnIncomingMessage(const std::wstring& from, const std::wstring& message, bool ctcpAction = false);
	void OnOutgoingMessage(wtwMessageDef* msg);
	void OnActionCommand(const std::wstring& from, const std::wstring& msg);
	void OnCTCPReply(const std::wstring& from, const std::wstring& reply);
	void OnKick(const std::wstring& who, const std::wstring& victim, const std::wstring& reason);
	void OnMode(const std::wstring& who, const std::wstring& user, const std::wstring& mode);
	void OnNotice(const std::wstring& from, const std::wstring& message, bool checkPresence = false);

	void setTopic(const std::wstring& newTopic, const std::wstring& setBy);

	void handleUserCommand(const std::wstring& message);
	WTW_PTR onWindowEvent(wtwConferenceWindowEvent* event, WTW_PARAM lParam);

	static WTW_PTR funcChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);

	std::wstring channel;
	std::wstring group;
	std::wstring name;
	std::wstring uuid;

	std::wstring nick;
	std::wstring tempNick;
	std::wstring passwd;

	std::wstring topic;

	HANDLE cntHandle;
	Account* parent;

	OnlineUserList users;

	int archId;

	WTW_PARAM getWindowHandle();

private:
	enum
	{
		MENU_SHOW_USER_INFO = 1,
		MENU_SHOW_CLIENT_INFO,
		MENU_PING,
		MENU_SLAP,
		MENU_ADD_OP,
		MENU_REM_OP,
		MENU_ADD_VOICE,
		MENU_REM_VOICE,
		MENU_KICK
	};
};

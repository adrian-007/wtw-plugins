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

#include "OnlineUser.hpp"
#include "AbstractTab.hpp"

class Account;
class PrivateChatTab : public OnlineUser, public AbstractTab
{
public:
	PrivateChatTab(const std::wstring& nick, Account* parent);
	~PrivateChatTab();

	void addTab(int showMethod = -1);
	void remTab() { };
	bool hasTab() const { return false; };

	void addUser(const std::wstring& user, const std::wstring& host, bool notify = true) { }
	void remUser(const std::wstring& user, bool notify = true, const std::wstring& reason = L"") { }
	bool hasUser(const std::wstring& user) { return nick == user; }

	bool getHeader(std::wstring& header) const;

	void addMessage(int messageType, const std::wstring& from, const std::wstring& message, bool incoming, bool notify, bool flash);
	bool equals(const std::wstring& id) const { return OnlineUser::equals(id); }

	void OnIncomingMessage(const std::wstring& from, const std::wstring& message, bool ctcpAction = false);
	void OnOutgoingMessage(wtwMessageDef* msg);
	WTW_PTR onWindowEvent(wtwConferenceWindowEvent* event, WTW_PARAM lParam);

private:
	WTW_PARAM getWindowHandle();

	Account* parent;
};

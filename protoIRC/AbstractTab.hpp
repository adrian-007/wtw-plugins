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

class AbstractTab
{
public:
	virtual ~AbstractTab()
	{
	}

	enum
	{
		MESSAGE_NORMAL	= 0x01,
		MESSAGE_INFO	= 0x02,
		MESSAGE_WARNING = 0x04,
		MESSAGE_ERROR	= 0x08
	};

	virtual void addTab(int showMethod = -1) = 0;
	virtual void remTab() = 0;
	virtual bool hasTab() const = 0;

	virtual void addUser(const std::wstring& user, const std::wstring& host, bool notify = true) = 0;
	virtual void remUser(const std::wstring& user, bool notify = true, const std::wstring& reason = L"") = 0;
	virtual bool hasUser(const std::wstring& user) = 0;
	virtual bool equals(const std::wstring& id) const = 0;

	virtual bool getHeader(std::wstring& header) const = 0;

	virtual void addMessage(int messageType, const std::wstring& from, const std::wstring& message, bool incoming, bool notify, bool flash) = 0;

	virtual void OnIncomingMessage(const std::wstring& from, const std::wstring& message, bool ctcpAction = false) = 0;
	virtual WTW_PTR onWindowEvent(wtwConferenceWindowEvent* event, WTW_PARAM lParam) = 0;

	inline bool operator == (const std::wstring& id) const
	{
		return equals(id);
	}

	inline bool operator != (const std::wstring& id) const
	{
		return equals(id) == false;
	}
};

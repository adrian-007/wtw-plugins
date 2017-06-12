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
#include "flags.hpp"

class OnlineUser : public Flags
{
public:
	enum
	{
		FLAG_OP = 0x01,
		FLAG_VOICE = 0x02,
		FLAG_AWAY = 0x04
	};

	OnlineUser(const std::wstring& _nick, const std::wstring& _host = L"");
	~OnlineUser();

	void setNick(const std::wstring& _nick);
	void setHost(const std::wstring& _host);
	void setMode(const std::wstring& mode);

	inline operator std::wstring() const
	{
		return nick;
	}

	inline operator const wchar_t* () const
	{
		return nick.c_str();
	}

	inline bool operator == (const std::wstring& nick) const
	{
		return equals(nick);
	}

	inline bool operator != (const std::wstring& nick) const
	{
		return equals(nick) == false;
	}

	inline bool operator == (const std::string& nick) const
	{
		return equals(nick);
	}

	inline bool operator != (const std::string& nick) const
	{
		return equals(nick) == false;
	}

	bool equals(const std::wstring& nick) const;
	bool equals(const std::string& nick) const;

	std::wstring getNick() const;
	std::wstring getHost() const;

protected:
	std::wstring nick;
	std::wstring host;
};

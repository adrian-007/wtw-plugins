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
#include "OnlineUser.hpp"
#include "../utils/text.hpp"

OnlineUser::OnlineUser(const std::wstring& _nick, const std::wstring& _host /*= L""*/)
{
	setNick(_nick);
	setHost(_host);
}

OnlineUser::~OnlineUser(void)
{
}

void OnlineUser::setNick(const std::wstring& _nick)
{
	nick = _nick;
	clearFlags();

	if(nick[0] == L'@')
	{
		nick.erase(0, 1);
		setFlag(FLAG_OP);
	}
	else if(nick[0] == L'+')
	{
		nick.erase(0, 1);
		setFlag(FLAG_VOICE);
	}
}

void OnlineUser::setHost(const std::wstring& _host)
{
	host = _host;
}

void OnlineUser::setMode(const std::wstring& mode)
{
}

bool OnlineUser::equals(const std::wstring& nick) const
{
	const wchar_t* victim = nick.c_str();
	const wchar_t* target = this->nick.c_str();

	if(victim[0] == L'@' || victim[0] == L'+')
	{
		++victim;
	}

	size_t victimLen = wcslen(victim);
	size_t targetLen = wcslen(target);

	if(victimLen == targetLen)
	{
		return _wcsnicmp(victim, target, victimLen) == 0;
	}

	return false;
}

bool OnlineUser::equals(const std::string& nick) const
{
	return equals(utils::text::toWide(nick));
}

std::wstring OnlineUser::getNick() const
{
	return nick;
}

std::wstring OnlineUser::getHost() const
{
	return host;
}

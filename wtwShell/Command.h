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

class wtwShell;

class Command  {
public:
	enum {
		TYPE_COMMAND = 0,
		TYPE_APPLICATION,
		TYPE_DIRECTORY,
		TYPE_LINK
	};

	Command(const std::wstring& uuid, const std::wstring& name, const std::wstring& group, const std::wstring& command, int type = TYPE_COMMAND, int presence = WTW_PRESENCE_OFFLINE, const std::wstring description = L"");

	static std::wstring generateUUID();

	void update(const std::wstring& name, const std::wstring& group, const std::wstring& command, int type = TYPE_COMMAND, int presence = WTW_PRESENCE_OFFLINE, const std::wstring description = L"");
	void exec();

	std::wstring uuid;
	std::wstring name;
	std::wstring command;
	std::wstring group;
	int type;
	int presence;
	std::wstring description;
};

#endif

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
#include "Command.h"
#include "Utils.hpp"
#include "wtwShell.hpp"

#include "ContactPage.h"

#include <shellapi.h>

Command::Command(const std::wstring& uuid, const std::wstring& name, const std::wstring& group, const std::wstring& command, int type /*= TYPE_COMMAND*/, int presence /*= WTW_PRESENCE_OFFLINE*/, const std::wstring description /*= L""*/) {
	this->uuid = uuid;
	this->name = name;
	this->command = command;
	this->group = group;
	this->type = type;
	this->presence = presence;
	this->description = description;

	if(this->uuid.empty()) {
		this->uuid = generateUUID();
	}
}

std::wstring Command::generateUUID() {
	std::wstring buf;
	UUID id;
	if(UuidCreate(&id) == RPC_S_OK) {
		RPC_WSTR p = 0;
		if(UuidToStringW(&id, &p) == RPC_S_OK) {
			buf = (LPWSTR)p;
			RpcStringFree(&p);
		}
	}
	return buf;
}

void Command::exec() {
	ShellExecuteW(0, 0, command.c_str(), 0, 0, SW_SHOWNORMAL);
}

void Command::update(const std::wstring& name, const std::wstring& group, const std::wstring& command, int type /*= TYPE_COMMAND*/, int presence /*= WTW_PRESENCE_OFFLINE*/, const std::wstring description /*= L""*/) {
	this->group = group;
	this->name = name;
	this->command = command;
	this->type = type;
	this->presence = presence;
	this->description = description;

	if(group.empty()) {
		this->group = WTW_DEFAULT_CT_GROUP;
	}

	if(uuid.empty()) {
		uuid = generateUUID();
	}

	wtwContactListItem item;
	initStruct(item);
	
	item.itemText				= name.c_str();
	item.contactData.id			= uuid.c_str();
	item.contactData.netClass	= PROTO_NAME;
	item.contactData.netId		= wtwShell::instance().getNetId();
	item.itemType				= WTW_CTL_TYPE_ELEMENT;
	item.itemGroup				= group.c_str();
	item.flags					= WTW_CTL_FLAG_REPLACE_DUPLICATE;

	wtw->fnCall(WTW_CTL_CONTACT_ADD, reinterpret_cast<WTW_PARAM>(&item), 0);

	wtwIconSet set;
	initStruct(set);
	set.iconSetID = L"wtwShell/set";
	set.flags = WTW_ICS_FLAG_PRELOADED | WTW_ICONSET_FLAG_MODIFY_CUR_IMAGE;
	set.iType = WTW_ICSET_TYPE_PRE_TEXT;
	set.curImage = type;

	wtw->fnCall(WTW_CTL_CONTACT_ICON_ADD, reinterpret_cast<WTW_PARAM>(item.itemHandle), reinterpret_cast<WTW_PARAM>(&set));
	wtw->fnCall(WTW_CTL_CONTACT_ICON_MODIFY, reinterpret_cast<WTW_PARAM>(item.itemHandle), reinterpret_cast<WTW_PARAM>(&set));

	wtwPresenceDef p;
	initStruct(p);

	item.contactData.flags = WTW_PRESENCE_FLAG_NO_NOTIFY;
	p.pContactData = &item.contactData;
	p.curStatus = presence;
	p.curDescription = description.c_str();

	wtw->fnCall(WTW_CTL_SET_CONTACT_STATE, reinterpret_cast<WTW_PARAM>(&p), 0);
}

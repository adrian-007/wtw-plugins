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

#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <list>

class Manager
{
public:
	Manager(HINSTANCE hInstance);
	~Manager();
private:
	typedef std::list<std::wstring> Contacts;

	static WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR PresenceListener(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr);

	void load();
	void save();

	void addItem(const std::wstring& id);
	void remItem(const std::wstring& id);
	bool findItem(const std::wstring& id) const;

	void onMenuCallback(wtwMenuItemDef* menuItem, wtwMenuPopupInfo* menuInfo);
	void onMenuRebuild(wtwMenuCallbackEvent* event);
	void onPresence(wtwPresenceDef* presence);

	Contacts items;
	HANDLE menuRebuildHook;
	HANDLE presenceHook;
	void* config;
	HINSTANCE hInstance;
	std::wstring emptyString;
};

#endif

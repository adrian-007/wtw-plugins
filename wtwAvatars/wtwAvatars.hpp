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

#ifndef WTW_CLIPBOARD
#define WTW_CLIPBOARD

#include <unordered_map>

#define PLUGIN_ICON L"wtwAvatar/icon"

class wtwAvatars 
{
	static wtwAvatars* inst;
public:
	wtwAvatars(HINSTANCE hInstance);
	~wtwAvatars();

	static void create(wtwAvatars* m) 
	{
		wtwAvatars::inst = m;
	}

	static void destroy() 
	{
		delete wtwAvatars::inst;
		wtwAvatars::inst = 0;
	}

	static wtwAvatars* instance()
	{
		return wtwAvatars::inst;
	}

	static bool isInstance() 
	{
		return wtwAvatars::inst != 0;
	}

private:
	typedef std::tr1::unordered_map<std::wstring, std::wstring> StringMap;
	StringMap avatars;

	void onContactAdded(wtwContactListItem* cnt);

	std::wstring getFile() const;
	int getFileType(const std::wstring& path);

	static WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
	static WTW_PTR MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void*);

	static WTW_PTR func_onContactAdded(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) 
	{
		((wtwAvatars*)ptr)->onContactAdded((wtwContactListItem*)wParam);
		return 0;
	}

	std::wstring filePath;

	HINSTANCE hInstance;
	HANDLE menuRebuildHook;
	HANDLE cntAddHook;

};

#endif

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
#include "resource.h"

#include "wtwProtoConsole.hpp"
#include "../utils/wtw.hpp"

#define MENU_ITEM_SHOW L"wtwProtoConsole/show"

wtwProtoConsole* wtwProtoConsole::inst = 0;

void wtwProtoConsole::create(wtwProtoConsole* m) 
{
	wtwProtoConsole::inst = m;
}

void wtwProtoConsole::destroy() 
{
	delete wtwProtoConsole::inst;
	wtwProtoConsole::inst = 0;
}

wtwProtoConsole* wtwProtoConsole::instance()
{
	return wtwProtoConsole::inst;
}

bool wtwProtoConsole::isInstance() 
{
	return wtwProtoConsole::inst != 0;
}

wtwProtoConsole::wtwProtoConsole() 
{
	//wtw_t::preloadIcon(wtw, hInstance, L"wtwProtoConsole/icon16", L"wtwProtoConsole16.png", IDB_PNG1);
	//wtw_t::preloadIcon(wtw, hInstance, L"wtwProtoConsole/icon32", L"wtwProtoConsole32.png", IDB_PNG2);

	//wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, filePath);

	wtwMenuItemDef mi;

	mi.menuID		= WTW_MENU_ID_MAIN_OPT;
	mi.hModule		= ghInstance;
	mi.callback		= &wtwProtoConsole::MenuCallback;
	mi.cbData		= (void*)this;

	mi.itemId		= MENU_ITEM_SHOW;
	mi.menuCaption	= L"Protocol Console";
	wtw->fnCall(WTW_MENU_ITEM_ADD, mi, 0);
}

wtwProtoConsole::~wtwProtoConsole()
{
	wtw->fnCall(WTW_MENU_ITEM_CLEAR, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

	if(dlg.IsWindow())
	{
		dlg.DestroyWindow();
	}
}

void wtwProtoConsole::onShowDialog() 
{
	if(dlg.IsWindow() == FALSE)
	{
		dlg.Create(nullptr);
	}

	dlg.ShowWindow(SW_SHOWNORMAL);
}

WTW_PTR wtwProtoConsole::MenuCallback(WTW_PARAM, WTW_PARAM, void* ptr)
{
	wtwProtoConsole* p = reinterpret_cast<wtwProtoConsole*>(ptr);

	if(p != 0)
	{
		p->onShowDialog();
	}

	return 0;
}

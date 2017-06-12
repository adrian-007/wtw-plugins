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

#ifndef WTW_PROTO_CONSOLE
#define WTW_PROTO_CONSOLE

#include "ProtoDialog.hpp"

class wtwProtoConsole 
{
public:
	wtwProtoConsole();
	~wtwProtoConsole();

	static void create(wtwProtoConsole* m);
	static void destroy();
	static wtwProtoConsole* instance();
	static bool isInstance();

private:
	static wtwProtoConsole* inst;

	void onShowDialog();
	static WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*);

	ProtoDialog dlg;
};

#endif // WTW_PROTO_CONSOLE

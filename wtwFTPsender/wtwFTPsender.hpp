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

#include "FTPThread.hpp"

class wtwFTPsender {
	static wtwFTPsender* inst;
public:
	wtwFTPsender(HINSTANCE hInstance);
	~wtwFTPsender();

	static void create(wtwFTPsender* m) {
		wtwFTPsender::inst = m;
	}

	static void destroy() {
		delete wtwFTPsender::inst;
		wtwFTPsender::inst = 0;
	}

	static wtwFTPsender* instance() {
		return wtwFTPsender::inst;
	}

	static bool isInstance() {
		return wtwFTPsender::inst != 0;
	}

	inline void updateServerInfo() {
		sender.setServerInfo();
	}

private:
	void addButton(wtwContactDef* cnt, wtwChatWindowInfo* nfo);
	void onClick(wtwCommandCallback* c);

	static WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
	static WTW_PTR MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void*);

	static WTW_PTR onWndCreate(WTW_PARAM cnt, WTW_PARAM nfo, void* ptr) {
		((wtwFTPsender*)ptr)->addButton((wtwContactDef*)cnt, (wtwChatWindowInfo*)nfo);
		return 0;
	}
	static WTW_PTR onClickCB(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr) {
		((wtwFTPsender*)ptr)->onClick((wtwCommandCallback*)lParam);
		return 0;
	}
	static WTW_PTR externalFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr);
	static WTW_PTR externalUtlFtpFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr);

	void getFile(wtwContactDef* c);

	FTPSender sender;

	HINSTANCE hInstance;
	HANDLE wndCreateHook;
	HANDLE onMenuRebuild;
	HANDLE extFuncHandle;
	HANDLE utlFtpFuncHandle;
};

#endif

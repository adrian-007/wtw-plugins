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

#ifndef PROTO_IRC_HPP
#define PROTO_IRC_HPP

#include <list>
#include "ExpatImpl.h"

class Command;

class wtwShell {
	static wtwShell* inst;
public:
	wtwShell(HINSTANCE hInst);
	~wtwShell();

	static void create(wtwShell* m) {
		wtwShell::inst = m;
	}

	static void destroy() {
		delete wtwShell::inst;
		wtwShell::inst = 0;
	}

	static wtwShell& instance() {
		return *wtwShell::inst;
	}

	static bool isInstance() {
		return wtwShell::inst != 0;
	}

	void load();
	void save();
	void install();

	int getNetId() const { return netSID; }

	void addCommand(Command* c) { commands.push_back(c); }
	Command* getCommand(const std::wstring& uuid);
	void removeCommand(Command* c);

private:
	typedef std::list<Command*> CommandList;
	CommandList commands;

	HINSTANCE hInstance;
	std::wstring rootDir;

	static WTW_PTR funcChannelInit(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMessageSend(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcRemoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);

	std::wstring uuid;
	int netSID;
	DWORD status;
	HANDLE protoFunc;
	HANDLE messageFunc;
	HANDLE onMenuRebuildHook;

	class XmlReader : public CExpatImpl<wtwShell::XmlReader> {
	public:
		XmlReader(wtwShell* e) : mng(e), valid(true), currentCommand(0) { }
		void OnPostCreate() {
			EnableElementHandler();
			EnableEndElementHandler();
		}

		void OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs);
		void OnEndElement(const XML_Char *pszName);
		bool valid;
	private:
		wtwShell* mng;
		Command* currentCommand;
	};
};

#endif
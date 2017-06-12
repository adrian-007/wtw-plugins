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

#include <vector>
#include "ExpatImpl.h"

class Account;
class Channel;

class IRCManager {
	static IRCManager* inst;
public:
	IRCManager(HINSTANCE hInst);
	~IRCManager();

	static void create(IRCManager* m) {
		IRCManager::inst = m;
	}

	static void destroy() {
		delete IRCManager::inst;
		IRCManager::inst = 0;
	}

	static IRCManager& instance() {
		return *IRCManager::inst;
	}

	static bool isInstance() {
		return IRCManager::inst != 0;
	}

	void load();
	void save();
private:
	static const unsigned short CONFIG_VERSION;

	typedef std::vector<Account*> AccountList;
	AccountList accounts;

	void accountManagerCallback(WTW_PARAM wParam, WTW_PARAM lParam);

	static WTW_PTR funcAccountManagerCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
		((IRCManager*)ptr)->accountManagerCallback(wParam, lParam);
		return 0;
	}

	HINSTANCE hInstance;
	std::wstring rootDir;

	HANDLE accMngHook;

	class XmlReader : public CExpatImpl<IRCManager::XmlReader> {
	public:
		XmlReader(IRCManager* e) : mng(e), valid(true), currentAccount(0) { }
		void OnPostCreate() {
			EnableElementHandler();
			EnableEndElementHandler();
		}

		void OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs);
		void OnEndElement(const XML_Char *pszName);
		bool valid;
	private:
		IRCManager* mng;
		Account* currentAccount;
	};
};

#endif

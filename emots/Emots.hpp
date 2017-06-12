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

#ifndef EMOTS
#define EMOTS

#include "ExpatImpl.h"
#include <list>
#include <unordered_map>

class EmotsListBox;

class EmotsManager {
	static EmotsManager* inst;

	class XmlReader : public CExpatImpl<EmotsManager::XmlReader> {
	public:
		XmlReader(EmotsManager* e) : mng(e), valid(true) { }
		void OnPostCreate() {
			EnableElementHandler();
			//EnableEndElementHandler();
		}

		void OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs);
		void OnEndElement(const XML_Char *pszName) { }
		bool valid;
	private:
		EmotsManager* mng;
	};
public:
	struct Item {
		std::wstring regex;
		std::wstring file;
		std::wstring description;
		std::wstring match;
		bool ignoreCase;
	};

	typedef std::list<Item*> ItemList;

	EmotsManager(HINSTANCE hInst);
	~EmotsManager();

	static void create(EmotsManager* m) {
		EmotsManager::inst = m;
	}

	static void destroy() {
		delete EmotsManager::inst;
		EmotsManager::inst = 0;
	}

	static EmotsManager& instance() {
		return *EmotsManager::inst;
	}

	static bool isInstance() {
		return EmotsManager::inst != 0;
	}

	int reload(const std::wstring& fName) throw();
	void reset();

	const std::wstring& getAuthor() const { return author; }
	const std::wstring& getDescription() const { return description; }
	const std::wstring& getRootDir() const { return rootDir; }

	size_t getCount() const { return items.size(); }
	const ItemList& getItems() { return items; }

	static void replaceAll(const std::wstring& what, const std::wstring& with, std::wstring& str);
private:
	typedef std::tr1::unordered_map<std::wstring, std::wstring> WStringMap;

	static const wchar_t format[];
	static const wchar_t formatDesc[];

	inline void setInfo(const std::wstring& a, const std::wstring& d) {
		author = a;
		description = d;
	}

	void clear();
	void htmlEscape(std::wstring& msg);
	void insertEmots(std::wstring& msg, const WStringMap& links);
	WStringMap hideLinks(std::wstring& msg) const;
	
	static WTW_PTR ChatWndCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*);

	static WTW_PTR cb_ChatWndCreate(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
		((EmotsManager*)ptr)->onChatWndCreate((wtwContactDef*)wParam, (wtwChatWindowInfo*)lParam);
		return 0;
	}

	static WTW_PTR cb_ChatBtnClick(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
		((EmotsManager*)ptr)->onChatBtnClick((wtwCommandCallback*)wParam);
		return 0;
	}

	void onChatWndCreate(wtwContactDef *cnt, wtwChatWindowInfo* nfo);
	void onChatWndDestroy(wtwChatWindowInfo* nfo);
	void onChatBtnClick(wtwCommandCallback* cb);

	std::list<Item*> items;

	std::wstring rootDir;
	std::wstring rootDirEscape;
	std::wstring curSetPath;

	std::wstring author;
	std::wstring description;

	HANDLE hook;
	HANDLE wndCreateHook;
	HANDLE wndDestroyHook;

	HINSTANCE hInstance;

	EmotsListBox* preview;
};

#endif
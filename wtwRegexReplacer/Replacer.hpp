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

#ifndef REPLACER_HPP
#define REPLACER_HPP

#include <list>

class Replacer {
public:
	class Item {
	public:
		Item() : ignoreCase(false), displayOnly(true), direction(DIR_BOTH) { }
		Item(const Item& rhs) {
			regex = rhs.regex;
			format = rhs.format;
			ignoreCase = rhs.ignoreCase;
			direction = rhs.direction;
			displayOnly = rhs.displayOnly;
		}

		Item& operator = (const Item& rhs) {
			regex = rhs.regex;
			format = rhs.format;
			ignoreCase = rhs.ignoreCase;
			direction = rhs.direction;
			displayOnly = rhs.displayOnly;
			return *this;
		}

		enum {
			DIR_BOTH = 0,
			DIR_IN,
			DIR_OUT
		};

		std::wstring regex;
		std::wstring format;

		bool ignoreCase;
		unsigned short direction;
		bool displayOnly;
	};

	typedef std::list<Item*> ItemList;

	Replacer();
	~Replacer();

	static void create(Replacer* r) {
		if(inst)
			delete inst;
		inst = r;
	}
	static void destroy() {
		if(inst) {
			delete inst;
			inst = 0;
		}
	}
	static Replacer* getInstance() {
		return inst;
	}

	static void replaceAll(const std::wstring& what, const std::wstring& with, std::wstring& str);

	void load();
	void save();
	void reload();
	void clear();

	ItemList getItems() { return items; }
	void setItems(ItemList& i) { 
		items = i; 
	}

private:
	static Replacer* inst;

	static WTW_PTR ChatWndCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* data);
	static WTW_PTR onMsgProcess(WTW_PARAM wParam, WTW_PARAM lParam, void* data);
	static WTW_PTR dispatchProtocol(WTW_PARAM lParam, WTW_PARAM wParam, void* data);

	void onMessage(bool display, wtwMessageDef* msg, std::wstring& out);

	ItemList items;

	std::wstring profilePath;
	std::wstring appPath;
	std::wstring profileName;
};

#endif

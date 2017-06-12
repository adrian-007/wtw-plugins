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
#include "Utils.hpp"

std::wstring Utils::toWide(const std::string& str) {
	std::wstring ret;
	if(str.empty() == false) {
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), 0, 0);
		if(size > 0) {
			ret.resize(size);
			size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &ret[0], (int)ret.size());
			if(size == 0) {
				ret.clear();
			}
		}
	}
	return ret;
}

std::string Utils::fromWide(const std::wstring& str) {
	std::string ret;
	if(str.empty() == false) {
		int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, 0, 0, 0, 0);
		if(size > 0) {
			ret.resize(size);
			size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &ret[0], (int)ret.size(), 0, 0);
			if(size == 0) {
				ret.clear();
			}
		}
	}
	return ret;
}

std::vector<std::wstring> Utils::split(const std::wstring& str, const std::wstring& sep /*= L" "*/) {
	std::wstring::size_type i = 0, j = 0;
	std::vector<std::wstring> l;

	while((i = str.find(sep, j)) != std::wstring::npos) {
		l.push_back(str.substr(j, i-j));
		j = i + sep.length();
	}
	if(j < str.length()) 
		l.push_back(str.substr(j, str.length()-j));
	return l;
}

void Utils::showNotify(const std::wstring& caption, const std::wstring& message) {
	wtwTrayNotifyDef nt;
	initStruct(nt);
	nt.textMain = caption.c_str();
	nt.textLower = message.c_str();
	nt.iconId = L"IRC/Icon/available";
	nt.graphType = WTW_TN_GRAPH_TYPE_SKINID;
	nt.flags = WTW_TN_FLAG_TXT_MULTILINE | WTW_TN_FLAG_HDR_MULTILINE;
	wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), 0);
}
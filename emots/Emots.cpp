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
#include "Emots.hpp"
#include "EmotsListBox.h"
#include "SettingsPage.h"
#include "resource.h"

#pragma warning(disable:4244)
#include <boost/regex.hpp>

#include "../utils/WinApiFile.hpp"
#include "../utils/wtw.hpp"

#define LOG(X) __LOG(wtw, L"EMOT", X)

const wchar_t EmotsManager::format[] = L"<img alt=\"\\1\" title=\"\\1\" src=\"%s%s\" border=\"0\" align=\"absmiddle\"/>";
const wchar_t EmotsManager::formatDesc[] = L"<img alt=\"%s\" title=\"%s\" src=\"%s%s\" border=\"0\" align=\"absmiddle\"/>";
const boost::regex_constants::syntax_option_type ignoreCaseFlag = boost::regex_constants::ECMAScript | boost::regex_constants::icase;

EmotsManager* EmotsManager::inst = 0;

EmotsManager::EmotsManager(HINSTANCE hInst) : hook(0), hInstance(hInst), preview(0), wndCreateHook(0), wndDestroyHook(0) {
	/*wtwBuffer buf;
	initStruct(buf);
	buf.pBuffer = new wchar_t[MAX_PATH + 1];
	buf.bufferSize = MAX_PATH + 1;

	wtw->fnCall(WTW_INFO_GET_PROFILE_DIRECTORY, (WTW_PARAM)&buf, 0);
	rootDir = buf.pBuffer;
	delete[] buf.pBuffer;*/

	wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, rootDir);
	rootDir += L"Emoticons\\";

	::CreateDirectory(rootDir.c_str(), 0);

	rootDirEscape = rootDir;
	replaceAll(L"\\", L"\\\\", rootDirEscape);
	replaceAll(L"$", L"\\$", rootDirEscape);

	cfgGetStr(curSetPath, L"EMOTXmlFile");
	
	wtw_t::preloadIcon(wtw, hInst, L"emots/icon16", L"iconEmots.png", IDB_PNG1);
	wtw_t::preloadIcon(wtw, hInst, L"emots/icon32", L"iconEmots32.png", IDB_PNG2);

	/*
	// ladowanie ikony
	{
		wtwGraphics icon;
		initStruct(icon);
		icon.graphId = L"emots/mainIcon";
		icon.imageType = 0;
		icon.hInst = hInstance;
		
		// probujemy zaladowac pliczek
		icon.flags = WTW_GRAPH_FLAG_RELATIVE_DEF_PATH;
		icon.filePath = L"iconEmots.png";
		
		HRESULT ret = (HRESULT)wtw->fnCall(WTW_GRAPH_LOAD, (WTW_PARAM)&icon, 0);

		if(ret == 0) {
			// dupa, ladujemy z zasobow
			icon.resourceID = MAKEINTRESOURCE(IDB_PNG1);
			icon.filePath = 0;
			icon.flags = 0;
			wtw->fnCall(WTW_GRAPH_LOAD, (WTW_PARAM)&icon, 0);
		}
	}*/

	// dodawanie strony do opcji
	{
		wtwOptionPageDef def;
		initStruct(def);

		def.callback	= SettingsPage::callback;
		def.id			= L"Emots/Options";
		def.caption		= L"Emotikony";
		def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;
		def.iconId		= L"emots/icon16";
		wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(hInstance), reinterpret_cast<WTW_PARAM>(&def));
	}

	if(cfgGetInt(L"usePreview", 0)) {
		wndCreateHook = wtw->evHook(WTW_EVENT_ON_CHATWND_CREATE, EmotsManager::cb_ChatWndCreate, (void*)this);
		//wndDestroyHook = wtw->evHook(WTW_EVENT_ON_CHATWND_DESTROY, EmotsManager::cb_ChatWndDestroy, (void*)this);
	}
	reload(curSetPath);
}

EmotsManager::~EmotsManager() {
	if(wndCreateHook) {
		wtw->evUnhook(wndCreateHook);
		wndCreateHook = 0;
	}
	if(wndDestroyHook) {
		wtw->evUnhook(wndDestroyHook);
		wndDestroyHook = 0;
	}

	clear();
	wtw->fnCall(WTW_CCB_FUNCT_CLEAR, reinterpret_cast<WTW_PARAM>(hInstance), 0);
	wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(hInstance), 0);
}

void EmotsManager::XmlReader::OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs) {
	if(!papszAttrs || !valid) return;
	if(wcsncmp(pszName, L"WTWEmoticons", 12) == 0) {
		valid = false;
		return;
	}

	if(wcsncmp(pszName, L"wtwEmots", 8) == 0) {
		mng->setInfo(papszAttrs[1], papszAttrs[3]);
	} else if(wcscmp(pszName, L"item") == 0) {
		Item* item = new Item;

		const wchar_t* regex = 0;
		const wchar_t* file = 0;
		const wchar_t* desc = 0;
		const wchar_t* icase = 0;
		const wchar_t* match = 0;

		for(int i = 0; papszAttrs[i] != 0; i += 2) {
			if(wcscmp(papszAttrs[i], L"regex") == 0) {
				regex = papszAttrs[i+1];
			} else if(wcscmp(papszAttrs[i], L"file") == 0) {
				file = papszAttrs[i+1];
			} else if(wcscmp(papszAttrs[i], L"desc") == 0) {
				desc = papszAttrs[i+1];
			} else if(wcscmp(papszAttrs[i], L"icase") == 0) {
				icase = papszAttrs[i+1];
			} else if(wcscmp(papszAttrs[i], L"match") == 0) {
				match = papszAttrs[i+1];
			}
		}

		bool remove = true;

		if(regex && file && desc) {
			item->regex += L'(';
			item->regex += std::wstring(regex);
			item->regex += L')';

			EmotsManager::replaceAll(L"#", L"(#9#)", item->regex);
			EmotsManager::replaceAll(L"&", L"(#1#)", item->regex);
			EmotsManager::replaceAll(L"<", L"(#0#)", item->regex);
			EmotsManager::replaceAll(L">", L"(#2#)", item->regex);
			EmotsManager::replaceAll(L"&lt;", L"(#0#)", item->regex);
			EmotsManager::replaceAll(L"&amp;", L"(#1#)", item->regex);
			EmotsManager::replaceAll(L"&gt;", L"(#2#)", item->regex);
			//lookbehind syntax
			EmotsManager::replaceAll(L"(?(#0#)=", L"(?<=", item->regex);
			EmotsManager::replaceAll(L"(?(#0#)!", L"(?<!", item->regex);

			item->file = std::wstring(file);

			EmotsManager::replaceAll(L"\\", L"\\\\", item->file);
			EmotsManager::replaceAll(L"$", L"\\$", item->file);

			item->description = std::wstring(desc);

			if(icase) {
				item->ignoreCase = wcscmp(icase, L"1") == 0;
			} else {
				item->ignoreCase = true;
			}

			if(match) {
				item->match = std::wstring(match);
			}

			EmotsManager::replaceAll(L"&", L"&amp;", item->description);
			EmotsManager::replaceAll(L"<", L"&lt;", item->description);
			EmotsManager::replaceAll(L">", L"&gt;", item->description);

			try {
				const boost::wregex rx(item->regex);
				mng->items.push_back(item);
				remove = false;
			} catch(...) {

			}
		}

		if(remove)
			delete item;
	}
}

void EmotsManager::clear() {
	if(hook) {
		wtw->evUnhook(hook);
		hook = 0;
	}

	for(std::list<Item*>::iterator i = items.begin(); i != items.end(); ++i)
		delete *i;
	items.clear();

	author = description = L"";
	if(preview) {
		preview->DestroyWindow();
		delete preview;
		preview = 0;
	}
}

void EmotsManager::reset() {
	cfgSetStr(L"EMOTXmlFile", L"");
	clear();
}

int EmotsManager::reload(const std::wstring& fName) throw() {
	cfgSetStr(L"EMOTXmlFile", fName.c_str());
	clear();
	if(fName.empty())
		return 1;

	std::wstring filePath = rootDir + fName;

	std::wstring buf;
	WinApiFile file(filePath, WinApiFile::READ_MODE);
	buf.resize(file.getFileSize());
	file >> buf;
	file.close();

	XmlReader xml(this);
	xml.Create();
	if(!xml.Parse(buf.c_str(), buf.length())) {
		LOG(xml.GetErrorString());
		clear();
		return 3;
	} else if(xml.valid == false) {
		cfgSetStr(L"EMOTXmlFile", L"");
		clear();
		return 4;
	}

	if(!hook)
		hook = wtw->evHook(WTW_EVENT_CHATWND_BEFORE_MSG_DISP2, EmotsManager::ChatWndCallback, (void*)this);

	if(!preview) {
		if(wndCreateHook != 0) {
			preview = new EmotsListBox;
			CRect rc;
			rc.left = rc.top = 0;
			rc.right = 250;
			rc.bottom = 200;
			preview->Create(0, rc, 0, WS_POPUP, WS_EX_TOOLWINDOW);
			preview->ModifyStyle(WS_CAPTION | WS_SYSMENU, 0);
		}
	} else {
		preview->clear();
	}

	if(preview) {
		StringMap* imgs = new StringMap;
		for(std::list<Item*>::const_iterator i = items.begin(); i != items.end(); ++i) {
			if((*i)->match.empty() == false) {
				imgs->insert(std::make_pair((*i)->match, rootDir + (*i)->file));
			}
		}
		preview->setPreviewImages(imgs);
	}
	return 0;
}

void EmotsManager::htmlEscape(std::wstring& msg) {
	std::wstring::size_type i = 0;
	const wchar_t fmt[] = L"<&>";

	while((i = msg.find_first_of(fmt, i)) != std::wstring::npos) {
		switch(msg[i]) {
			case L'<': msg.replace(i, 1, L"#0#"); i+=3; break;
			case L'&': msg.replace(i, 1, L"#1#"); i+=3; break;
			case L'>': msg.replace(i, 1, L"#2#"); i+=3; break;
			//case L'\r': msg.erase(i, 1); break;
			//case L'\n': msg.replace(i, 1, L"#3#"); i+=3; break;
			//case L'\t': msg.replace(i, 1, L"#4#"); i+=3; break;
			default: i++;
		}
	}
}

void EmotsManager::replaceAll(const std::wstring& what, const std::wstring& with, std::wstring& str) {
	std::wstring::size_type i = 0;

	while((i = str.find(what, i)) != std::wstring::npos) {
		str.replace(i, what.length(), with);
		i += with.length();
	}
}

EmotsManager::WStringMap EmotsManager::hideLinks(std::wstring& msg) const {
	EmotsManager::WStringMap links;

	std::wstring::size_type i = 0, j = 0;
	std::wstring buf;
	buf.resize(64);

	int cnt = 0;
	int len;

	while((i = msg.find(L"http://", i)) != std::wstring::npos) {
		j = msg.find_first_of(L" \t\n", i);
		if(j == std::wstring::npos) {
			j = msg.length();
		}
		std::wstring link = msg.substr(i, j-i);
		len = swprintf(&buf[0], buf.size(), L"#L=%d#", cnt++);

		const std::wstring& code = buf.substr(0, len);

		links.insert(std::make_pair(code, link));
		msg.replace(i, link.length(), code);
		i += len;
	}

	i = j = 0;
	while((i = msg.find(L"https://", i)) != std::wstring::npos) {
		j = msg.find_first_of(L" \t\n", i);
		if(j == std::wstring::npos) {
			j = msg.length();
		}
		std::wstring link = msg.substr(i, j-i);
		len = swprintf(&buf[0], buf.size(), L"#L=%d#", cnt++);

		const std::wstring& code = buf.substr(0, len);

		links.insert(std::make_pair(code, link));
		msg.replace(i, link.length(), code);
		i += len;
	}
	i = j = 0;
	while((i = msg.find(L"ftp://", i)) != std::wstring::npos) {
		j = msg.find_first_of(L" \t\n", i);
		if(j == std::wstring::npos) {
			j = msg.length();
		}
		std::wstring link = msg.substr(i, j-i);
		len = swprintf(&buf[0], buf.size(), L"#L=%d#", cnt++);

		const std::wstring& code = buf.substr(0, len);

		links.insert(std::make_pair(code, link));
		msg.replace(i, link.length(), code);
		i += len;
	}
	return links;
}

void EmotsManager::insertEmots(std::wstring& msg, const WStringMap& links) {
	std::wstring buffer;
	buffer.resize(4096);
	size_t len;

	//replaceAll(L"\n", L"#3#", msg);
	//replaceAll(L"\t", L"#4#", msg);
	
	for(std::list<Item*>::iterator i = items.begin(); i != items.end(); ++i) {
		if((*i)->file.empty())
			continue;
		try {
			const boost::wregex reg((*i)->regex, (*i)->ignoreCase ? ignoreCaseFlag : boost::regex_constants::ECMAScript);
			if((*i)->description.empty()) {
				len = swprintf(&buffer[0], buffer.size(), format, rootDirEscape.c_str(), (*i)->file.c_str());
			} else {
				len = swprintf(&buffer[0], buffer.size(), formatDesc, (*i)->description.c_str(), (*i)->description.c_str(), rootDirEscape.c_str(), (*i)->file.c_str());
			}
			msg = boost::regex_replace(msg, reg, buffer.substr(0, len));
			//__LOG(wtw, L"emots", msg.c_str());
		} catch(...) {
			//...
		}
	}

	replaceAll(L"#0#", L"&lt;", msg);
	replaceAll(L"#1#", L"&amp;", msg);
	replaceAll(L"#2#", L"&gt;", msg);
	//replaceAll(L"#3#", L"<br/>", msg);
	//replaceAll(L"#4#", L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", msg);

	for(EmotsManager::WStringMap::const_iterator i = links.begin(); i != links.end(); ++i)
		replaceAll(i->first, i->second, msg);
}

WTW_PTR EmotsManager::ChatWndCallback(WTW_PARAM wParam, WTW_PARAM, void*) {
	wtwBmd2Struct* pBmd = (wtwBmd2Struct*)wParam;

	if(pBmd == 0 || (pBmd->bmdResult & BMD_FORCE_NO_DISP) != 0)
		return 0;

	wtwMessageDef *pMsg = pBmd->pMsg;

	if(!pMsg || pMsg->msgMessage == 0 || pMsg->contactData.netClass == 0 || wcslen(pMsg->msgMessage) <= 0)
		return 0;

	if((pMsg->msgFlags & WTW_MESSAGE_FLAG_PICTURE) ||
		(pMsg->msgFlags & WTW_MESSAGE_FLAG_CUSTOM_VARS) || (pMsg->msgFlags & WTW_MESSAGE_FLAG_TRANSFER_LINK))
		return 0;

	if(((pMsg->msgFlags & WTW_MESSAGE_FLAG_ERROR) ||
		(pMsg->msgFlags & WTW_MESSAGE_FLAG_INFO) ||
		(pMsg->msgFlags & WTW_MESSAGE_FLAG_WARNING)) && cfgGetInt(L"ignoreSpecial", 0))
		return 0;

	std::wstring message(pMsg->msgMessage);
	EmotsManager::replaceAll(L"#", L"#9#", message);

	const WStringMap& links = EmotsManager::instance().hideLinks(message);

	bool wantEscape = (pMsg->msgFlags & WTW_MESSAGE_FLAG_NOHTMLESC) == 0;

	//__LOG_F(wtw, 1, L"emots", L"msg = %s wantEscape = %d", message.c_str(), wantEscape ? 1 : 0);

	if(wantEscape) {
		EmotsManager::instance().htmlEscape(message);
		pMsg->msgFlags |= WTW_MESSAGE_FLAG_NOHTMLESC;
	} else {
		EmotsManager::replaceAll(L"&lt;", L"#0#", message);
		EmotsManager::replaceAll(L"&amp;", L"#1#", message);
		EmotsManager::replaceAll(L"&gt;", L"#2#", message);
	}

	EmotsManager::instance().insertEmots(message, links);
	EmotsManager::replaceAll(L"#9#", L"#", message);
	
	if(wantEscape) {
		const std::wstring br = (wcscmp(pMsg->contactData.netClass, L"GG") == 0 ? L"<br>" : L"<br/>");
		EmotsManager::replaceAll(L"\r\n", br, message);
		EmotsManager::replaceAll(L"\n", br, message);
		EmotsManager::replaceAll(L"\t", L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", message);
	}

	//__LOG_F(wtw, 1, L"emots", L"after = %s", message.c_str());

	pBmd->fnReplaceMsg(message.c_str(), pBmd);
	return 0;
}

void EmotsManager::onChatWndCreate(wtwContactDef *cnt, wtwChatWindowInfo* nfo) {
	wtwCommandEntry entry;
	initStruct(entry);
	entry.pContactData = cnt;
	entry.callback = cb_ChatBtnClick;
	entry.cbData = (void*)this;
	entry.hInstance = hInstance;
	entry.itemData = (void*)nfo->hWindow;
	entry.itemFlags = 0;
	entry.itemId = L"emots/button";
	entry.itemType = CCB_TYPE_STANDARD;
	entry.toolTip = L"Lista emotikon";
	entry.graphId = L"emots/icon16";
	entry.pWnd = nfo->pWnd;

	wtw->fnCall(WTW_CCB_FUNCT_ADD, (WTW_PARAM)&entry, 0);
}

void EmotsManager::onChatWndDestroy(wtwChatWindowInfo* nfo) {

}

void EmotsManager::onChatBtnClick(wtwCommandCallback* c) {
	if(c->action == CCB_ACTION_LCLICK && items.size() > 0) {
		HWND edit = FindWindowExW((HWND)c->itemData, 0, L"A7A0B242-211B-424c-96E0-73D31C1A2436", 0);
		if(edit && preview) {
			preview->show(c->pt, edit);
		}
	}
}

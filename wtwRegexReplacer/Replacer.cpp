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

#pragma warning(disable:4244)
#include <boost/regex.hpp>
#pragma warning(default:4244)

#include "Replacer.hpp"
#include "../utils/WinApiFile.hpp"

Replacer* Replacer::inst = 0;

Replacer::Replacer() {
	using namespace wtw_t;

	{
		paths::getPath(api::get(), WTW_DIRECTORY_PROFILE, profilePath);
		paths::getPath(api::get(), WTW_DIRECTORY_EXE, appPath);
	}

	if(api::get()->fnExists(IGNORE_ADD_OPTION))
	{
		utlIgnoreOptionEntry ie;
		ie.lockId = L"wtwRegexReplacer/ignoreMessage";
		ie.lockName = L"Przetwarzanie wiadomo�ci za pomoc� wyra�e� regularnych";
		ie.lockOptionName = L"Nie przetwarzaj wiadomo�ci przy pomocy wyra�e� regularnych";
		
		api::get()->fnCall(IGNORE_ADD_OPTION, ie, 0);
	}

	load();

	api::get()->hookAttach(api::get()->evHook(WTW_EVENT_CHATWND_BEFORE_MSG_DISP2, &Replacer::ChatWndCallback, (void*)this));
	api::get()->hookAttach(api::get()->evHook(WTW_EVENT_CHATWND_BEFORE_MSG_PROC, &Replacer::onMsgProcess, (void*)this));
	api::get()->hookAttach(api::get()->evHook(WTW_ON_PROTOCOL_EVENT, &Replacer::dispatchProtocol, (void*)this));
}

Replacer::~Replacer() {
	save();
	wtw_t::api::get()->hookClearAll();
}

void Replacer::load() {
	WinApiFile f(profilePath + L"wtwRegexReplacerRules.bin", WinApiFile::READ_MODE);

	unsigned int len;
	std::wstring buf;
	while(f.good()) {
		Item* i = new Item;

		f >> len;
		i->regex.resize(len);
		f >> i->regex;

		f >> len;
		i->format.resize(len);
		f >> i->format;

		f >> i->ignoreCase;
		f >> i->displayOnly;
		f >> i->direction;

		if(!i->regex.empty()) {
			try {
				const boost::wregex reg(i->regex);
				items.push_back(i);
				continue;
			} catch(...) {

			}
		}
		delete i;
	}

	f.close();
}

void Replacer::save() {
	WinApiFile f(profilePath + L"wtwRegexReplacerRules.bin", WinApiFile::WRITE_MODE, WinApiFile::ALWAYS_CREATE);

	for(ItemList::const_iterator it = items.begin(); it != items.end(); ++it) {
		Item* i = *it;
		if(i->regex.empty())
			continue;

		f << (unsigned int)i->regex.size();
		f << i->regex;

		f << (unsigned int)i->format.size();
		f << i->format;

		f << i->ignoreCase;
		f << i->displayOnly;
		f << i->direction;
	}
	f.close();

	clear();
}

void Replacer::reload() {
	save();
	load();
}

void Replacer::clear() {
	for(ItemList::iterator i = items.begin(); i != items.end(); ++i)
		delete *i;
	items.clear();
}

void Replacer::replaceAll(const std::wstring& what, const std::wstring& with, std::wstring& str) {
	std::wstring::size_type i = 0;

	while((i = str.find(what, i)) != std::wstring::npos) {
		str.replace(i, what.length(), with);
		i += with.length();
	}
}

void Replacer::onMessage(bool display, wtwMessageDef* msg, std::wstring& out)
{
	out = msg->msgMessage;
	bool incoming = (msg->msgFlags & WTW_MESSAGE_FLAG_INCOMING) != 0;

	if(wtw_t::api::get()->fnExists(IGNORE_IS_IGNORED))
	{
		utlIgnoreEntry ie;
		ie.pContacts = &msg->contactData;
		ie.iContacts = 1;

		DWORD tmp;

		ie.flags |= IGNORE_FLAG_CHECK_USER | IGNORE_FLAG_CHECK_MESSAGES;
		ie.userLockName = L"wtwRegexReplacer/ignoreMessage";
		ie.rInfo = &tmp;

		if(wtw_t::api::get()->fnCall(IGNORE_IS_IGNORED, ie, 0) == S_OK)
		{
			if(ie.rInfo[0] & (IGNORE_FLAG_CHECK_USER | IGNORE_FLAG_CHECK_MESSAGES))
			{
				return;
			}
		}
	}

	if((msg->msgFlags & WTW_MESSAGE_FLAG_NOHTMLESC) == 0)
	{
		std::wstring::size_type i = 0;
		const wchar_t fmt[] = L"<&>\r\n\t";
		
		std::wstring br;
		/*
		  bug w wtw, wtyczka od xmpp nie potrafi obslugiwac formatowania HTML
		  przez co przejscie do nastepnej linii nie jest poprawnie interpretowane

		  dodatkowo gg jest na tyle idiotyczne, ze
		  - nie interpretuje <br/> a jedynie <br> (sic!)
		  - interpretuje \r\n (!!!!!!!!) jako <br>

		  kurwa co za zycie...
		 */

		if(wcscmp(msg->contactData.netClass, L"GG") == 0)
		{
			br = L"<br>";
		}
		else if(incoming == false && display == false && (wcscmp(msg->contactData.netClass, L"XMPP") == 0 || wcscmp(msg->contactData.netClass, L"TLEN") == 0))
		{
			//br = L"<br/>\r\n";
		}
		else
		{
			br = L"<br/>";
		}

		if(br.length() > 0)
		{
			while((i = out.find_first_of(fmt, i)) != std::wstring::npos)
			{
				switch(out[i])
				{
					case L'<': out.replace(i, 1, L"&lt;"); i+=4; break;
					case L'&': out.replace(i, 1, L"&amp;"); i+=5; break;
					case L'>': out.replace(i, 1, L"&gt;"); i+=4; break;
					case L'\r': out.erase(i, 1); break;
					case L'\n': out.replace(i, 1, br); i+=br.length(); break;
					case L'\t': out.replace(i, 1, L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"); i+=42; break;
					default: i++;
				}
			}

			msg->msgFlags |= WTW_MESSAGE_FLAG_NOHTMLESC;
		}
	}

	std::wstring fmt, rx, cntNick;
	Item* i;

	void* h = 0;
	
	wtw_t::api::get()->call(WTW_CTL_CONTACT_FIND_EX, &msg->contactData, &h);
	
	if(h)
	{
		wtwContactListItem item;
		initStruct(item);
		wtw_t::api::get()->call(WTW_CTL_CONTACT_GET, h, &item);
		cntNick = item.itemText;
	}

	for(ItemList::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		i = *it;
		if(display != i->displayOnly)
			continue;

		if(i->direction == Item::DIR_BOTH || (i->direction == Item::DIR_IN && incoming) || (i->direction == Item::DIR_OUT && incoming == false))
		{
			rx = i->regex;
			//replaceAll(L"%profileName%", profileName, rx);
			replaceAll(L"%contactId%", msg->contactData.id, rx);
			replaceAll(L"%contactNick%", cntNick, rx);

			try
			{
				const boost::wregex reg(rx, i->ignoreCase ? boost::regex_constants::ECMAScript | boost::regex_constants::icase : boost::regex_constants::ECMAScript);
				
				fmt = i->format;
				//replaceAll(L"%profileName%", profileName, fmt);
				replaceAll(L"%contactId%", msg->contactData.id, fmt);
				replaceAll(L"%contactNick%", cntNick, fmt);
				replaceAll(L"%profilePath%", profilePath, fmt);
				replaceAll(L"%appPath%", appPath, fmt);

				out = boost::regex_replace(out, reg, fmt);
			}
			catch(...)
			{

			}
		}
	}
}

WTW_PTR Replacer::ChatWndCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* data)
{
	Replacer* parent = reinterpret_cast<Replacer*>(data);

	if(parent == 0)
	{
		return 0;
	}

	wtwBmd2Struct *pBmd = (wtwBmd2Struct*)wParam;

	if(!pBmd || (pBmd->bmdResult & BMD_FORCE_NO_DISP))
	{
		return 0;
	}

	wtwMessageDef *pMsg = pBmd->pMsg;

	if(!pMsg || pMsg->msgMessage == 0 || pMsg->contactData.netClass == 0 || wcslen(pMsg->msgMessage) <= 0)
	{
		return 0;
	}

	if((pMsg->msgFlags & WTW_MESSAGE_FLAG_PICTURE) || (pMsg->msgFlags & WTW_MESSAGE_FLAG_CUSTOM_VARS))
	{
		return 0;
	}

	WTW_PTR ret = BMD_OK;
	std::wstring message;
	parent->onMessage(true, pMsg, message);

	if(message.empty() == false)
	{
		pBmd->fnReplaceMsg(message.c_str(), pBmd);
	}
	else
	{
		ret = BMD_FORCE_NO_DISP;
	}

	pBmd->bmdResult |= (int)ret;

	return 0;
}

// pobrane z edita, przed wyslaniem
WTW_PTR Replacer::onMsgProcess(WTW_PARAM wParam, WTW_PARAM lParam, void* data)
{
	Replacer* parent = reinterpret_cast<Replacer*>(data);

	if(!parent)
	{
		return 0;
	}

	wtwBmpStruct* pBmd = (wtwBmpStruct*)wParam;

	if(pBmd == 0 || pBmd->fnReplaceMsg == 0)
	{
		return 0;
	}

	wtwMessageDef* pMsg = &pBmd->message;

	if(pMsg == 0 || pMsg->msgMessage == 0 || pMsg->contactData.netClass == 0 || wcslen(pMsg->msgMessage) <= 0)
	{
		return 0;
	}

	if((pMsg->msgFlags & WTW_MESSAGE_FLAG_PICTURE) || (pMsg->msgFlags & WTW_MESSAGE_FLAG_CUSTOM_VARS))
	{
		return 0;
	}

	WTW_PTR ret;
	std::wstring message;

	parent->onMessage(false, pMsg, message);

	if(message.empty() == false)
	{
		pBmd->fnReplaceMsg(message.c_str(), pBmd);
		ret = BMD_OK;
	}
	else
	{
		ret = BMP_NO_PROCESS;
	}

	return ret;
}

// na wiadomosci przychodzace, zeby mozna bylo tresc zmienic
// magic number, powiedzmy, ze po tym bedziemy rozpoznawac wiadomosc...
#define MAGIC_NUMBER reinterpret_cast<void*>(0x007)

WTW_PTR Replacer::dispatchProtocol(WTW_PARAM lParam, WTW_PARAM wParam, void* data) {
	Replacer* parent = reinterpret_cast<Replacer*>(data);
	if(!parent)
		return 0;

	wtwProtocolEvent* pEvn = (wtwProtocolEvent*)lParam;

	if(!pEvn || pEvn->type != WTW_PEV_TYPE_BEFORE)
		return 0;

	if(pEvn->event == WTW_PEV_MESSAGE_RECV) {
		wtwMessageDef* pMsg = (wtwMessageDef*)wParam;
		if(!pMsg || pMsg->msgMessage == 0 || pMsg->contactData.netClass == 0 || (pMsg->msgFlags & WTW_MESSAGE_FLAG_INCOMING) == 0)
			return 0;

		if((pMsg->msgFlags & WTW_MESSAGE_FLAG_PICTURE) || (pMsg->msgFlags & WTW_MESSAGE_FLAG_CUSTOM_VARS))
			return 0;

		// tymczasowy fix na wiadomosci konferencyjne
		if(pMsg->msgFlags & WTW_MESSAGE_FLAG_CONFERENCE)
			return 0;

		if(pMsg->ownerData == 0 || pMsg->ownerData != MAGIC_NUMBER) {
			std::wstring out;
			parent->onMessage(false, pMsg, out);

			if(out.empty() == false) {
				wtwProtocolEvent ev;
				initStruct(ev);

				ev.netClass = pEvn->netClass;
				ev.netId = pEvn->netId; 
				ev.type = WTW_PEV_TYPE_BEFORE; 
				ev.event = WTW_PEV_MESSAGE_RECV; 

				wtwMessageDef msg;
				msg = *pMsg;
				msg.structSize = sizeof(msg);
				msg.msgMessage = out.c_str();
				msg.ownerData = MAGIC_NUMBER;

				if(wtw_t::api::get()->call(WTW_PF_CALL_HOOKS, &ev, &msg) == 0) { 
					ev.type = WTW_PEV_TYPE_AFTER; 
					wtw_t::api::get()->call(WTW_PF_CALL_HOOKS, &ev, &msg);
				}
			}
			return 1;
		}
	}
	return 0;
}

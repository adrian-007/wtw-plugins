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
#include "wtwShell.hpp"
#include "Command.h"
#include "ContactPage.h"

#include "../utils/WinApiFile.hpp"
#include "../utils/XMLWriter.hpp"
#include "Utils.hpp"

#include <wtwInputWindows.h>
#include <algorithm>

#include "../utils/wtw.hpp"

wtwShell* wtwShell::inst = 0;

wtwShell::wtwShell(HINSTANCE hInst) : hInstance(hInst) {
	wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, rootDir);

	uuid = Command::generateUUID();

	wtw_t::preloadIcon(wtw, hInst, L"wtwShell/command", L"iconShell.png", IDB_PNG2);
	wtw_t::preloadIcon(wtw, hInst, L"wtwShell/application", L"iconApplication.png", IDB_PNG2);
	wtw_t::preloadIcon(wtw, hInst, L"wtwShell/directory", L"iconDirectory.png", IDB_PNG2);
	wtw_t::preloadIcon(wtw, hInst, L"wtwShell/link", L"iconLink.png", IDB_PNG2);

	// ladowanie ikony
	{
		wtwIconSet set;
		initStruct(set);
		set.iconSetID = L"wtwShell/set";
		set.iType = WTW_ICSET_TYPE_PRE_TEXT;

		wtw->fnCall(WTW_ICSET_CREATE,(WTW_PARAM)&set, 0);

		wtw->fnCall(WTW_ICSET_ADD_IMG, (WTW_PARAM)L"wtwShell/set", (WTW_PARAM)L"wtwShell/command");
		wtw->fnCall(WTW_ICSET_ADD_IMG, (WTW_PARAM)L"wtwShell/set", (WTW_PARAM)L"wtwShell/application");
		wtw->fnCall(WTW_ICSET_ADD_IMG, (WTW_PARAM)L"wtwShell/set", (WTW_PARAM)L"wtwShell/directory");
		wtw->fnCall(WTW_ICSET_ADD_IMG, (WTW_PARAM)L"wtwShell/set", (WTW_PARAM)L"wtwShell/link");
	}
}

wtwShell::~wtwShell() {
	wtw->fnCall(WTW_PROTO_FUNC_DEL, (WTW_PARAM)PROTO_NAME, netSID);
	wtw->fnDestroy(protoFunc);
	wtw->fnDestroy(messageFunc);
	wtw->evUnhook(onMenuRebuildHook);

	protoFunc = 0;

	wtw->fnCall(WTW_CTL_DELETE_ALL, reinterpret_cast<WTW_PARAM>(PROTO_NAME), netSID);

	for(CommandList::iterator i = commands.begin(); i != commands.end(); ++i) {
		delete *i;
	}
	commands.clear();
}

void wtwShell::load() {
	WinApiFile f;
	f.open(rootDir + L"wtwShell.xml", WinApiFile::READ_MODE, WinApiFile::ALWAYS_OPEN);
	std::wstring buf;
	buf.resize(f.getFileSize());
	f >> buf;

	XmlReader r(this);
	r.Create();
	if(!r.Parse(buf.c_str(), buf.size())) {
		__LOG_F(wtw, 1, PROTO_NAME, L"Error while loading xml: %s", r.GetErrorString());
	}
}

void wtwShell::save() {
	WinApiFile f;
	f.open(rootDir + L"wtwShell.xml", WinApiFile::WRITE_MODE, WinApiFile::ALWAYS_CREATE);
	unsigned char smarker[3];
	smarker[0] = 0xEF;
	smarker[1] = 0xBB;
	smarker[2] = 0xBF;
	f.write(smarker, sizeof(unsigned char) * sizeof(smarker));

	xml::writer::element root("wtwShell");
	root.addAttrib("uuid", Utils::fromWide(uuid));

	for(CommandList::const_iterator i = commands.begin(); i != commands.end(); ++i) {
		Command* a = *i;
		xml::writer::element& e = root.getChild("shellCommand");

		e.addAttrib("uuid", Utils::fromWide(a->uuid));
		e.addAttrib("name", Utils::fromWide(a->name));
		e.addAttrib("group", Utils::fromWide(a->group));
		e.addAttrib("cmd", Utils::fromWide(a->command));
		e.addAttrib("type", a->type);
		e.addAttrib("presence", a->presence);
		e.addAttrib("description", Utils::fromWide(a->description));
	}

	std::string buf = root.getUtf8Header();
	root.print(buf);
	f << buf;
	f.close();
}

void wtwShell::install() {
	// rejestracja protokolu
	{
		wtwProtocolDef pDef;
		initStruct(pDef);

		pDef.protoCaps1 = WTW_PROTO_CAPS1_CHAT;
		pDef.protoCaps2	= WTW_PROTO_CAPS2_UTF;
		pDef.protoDescr	= L"Windows Shell";
		pDef.netClass	= PROTO_NAME;
		pDef.protoName	= L"- Windows Shell -";
		pDef.netGUID	= this->uuid.c_str();

		pDef.pInterface.pfDeleteContact	= &wtwShell::funcRemoveChannel;
		pDef.pInterface.pfMoveContact	= &wtwShell::funcMoveChannel;
		pDef.flags		= WTW_PROTO_FLAG_PSEUDO | WTW_PROTO_FLAG_NO_MENU | WTW_PROTO_FLAG_NO_PUBDIR;

		wtw->fnCall(WTW_PROTO_FUNC_ADD, reinterpret_cast<WTW_PARAM>(&pDef), 0);
		netSID = pDef.netId;

		protoFunc = wtwInstProtoFunc(wtw, PROTO_NAME, netSID, WTW_PF_INIT_CHAT, &funcChannelInit, 0);
		messageFunc = wtwInstProtoFunc(wtw, PROTO_NAME, netSID, WTW_PF_MESSAGE_SEND, &funcMessageSend, 0);
	}

	// strona opcji kontaktu
	{
		wtwPropertiesPage pPage;
		initStruct(pPage);

		wchar_t pageID[1024];
		swprintf(pageID, sizeof(pageID), PROTO_NAME L"/%d/Properties", netSID);

		pPage.callback			= &ContactPage::callback;
		pPage.caption			= L"W�a�ciwo�ci";
		pPage.flags				= WTW_PP_FLAG_ALLOW_ADD | WTW_PP_FLAG_ALLOW_EDT | WTW_PP_FLAG_AS_GROUP;
		pPage.netClass			= PROTO_NAME;
		pPage.netId				= netSID;
		pPage.cbData			= (void*)this;
		pPage.pageId			= pageID;
		pPage.position			= -1000;
		pPage.iconId			= L"wtwShell/command";

		wtw->fnCall(WTW_PROPERTIES_WND_ADD_STATIC_PROTO_PAGE, reinterpret_cast<WTW_PARAM>(&pPage), 0);
	}
	
	onMenuRebuildHook = wtw->evHook(WTW_EVENT_MENU_REBUILD, &wtwShell::funcMenuRebuild, (void*)this);

	for(CommandList::iterator i = commands.begin(); i != commands.end(); ++i) {
		Command* c = *i;
		//c->add();
		c->update(c->name, c->group, c->command, c->type, c->presence, c->description);
	}
}

Command* wtwShell::getCommand(const std::wstring& uuid) {
	for(CommandList::iterator i = commands.begin(); i != commands.end(); ++i) {
		if((*i)->uuid == uuid) {
			return *i;
		}
	}
	return 0;
}

void wtwShell::removeCommand(Command* c) {
	CommandList::iterator i = std::find(commands.begin(), commands.end(), c);
	if(i != commands.end()) {
		commands.erase(i);
		delete c;
	}
}

WTW_PTR wtwShell::funcChannelInit(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwContactDef* cnt = (wtwContactDef*)wParam;
	Command* c = wtwShell::instance().getCommand(cnt->id);
	if(c) {
		c->exec();
	}

	return 1;
}

WTW_PTR wtwShell::funcMessageSend(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	return 1;
}

WTW_PTR wtwShell::funcMoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	return 0;
}

WTW_PTR wtwShell::funcRemoveChannel(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwContactDef* cnt = reinterpret_cast<wtwContactDef*>(wParam);
	Command* c = wtwShell::instance().getCommand(cnt->id);
	if(c) {
		wtwShell::instance().removeCommand(c);
	}

	WTW_PARAM hCnt = 0;
	wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, reinterpret_cast<WTW_PARAM>(cnt), (WTW_PARAM)&hCnt);
	if(hCnt) {
		wtw->fnCall(WTW_CTL_CONTACT_DELETE, hCnt, 0);
	}

	return 0;
}

WTW_PTR wtwShell::funcMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwMenuCallbackEvent* event = reinterpret_cast<wtwMenuCallbackEvent*>(wParam);

	if(!event->pInfo->iContacts)
		return 0;

	if(wcscmp(event->pInfo->pContacts[0].netClass, PROTO_NAME))
		return 0;

	event->slInt.add(event->itemsToShow, WTW_MIID_CT_EDIT);
	event->slInt.add(event->itemsToShow, WTW_MIID_CT_DELETE);

	return 0;
}

void wtwShell::XmlReader::OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs) {
	if(wcscmp(pszName, L"wtwShell") == 0) {
		const wchar_t* uuid = L"";

		for(int i = 0; papszAttrs[i] != 0; i += 2) {
			if(wcscmp(papszAttrs[i], L"uuid") == 0)
			{
				uuid = papszAttrs[i+1];
			}
		}

		if(wcslen(uuid) > 0) {
			mng->uuid = uuid;
		}

		return;
	}

	if(wcscmp(pszName, L"shellCommand") == 0) {
		const wchar_t* uuid = L"";
		const wchar_t* name = L"";
		const wchar_t* group = L"";
		const wchar_t* cmd = L"";
		const wchar_t* description = L"";

		int type = Command::TYPE_COMMAND;
		int presence = WTW_PRESENCE_OFFLINE;

		for(int i = 0; papszAttrs[i] != 0; i += 2) {
			if(wcscmp(papszAttrs[i], L"uuid") == 0)
			{
				uuid = papszAttrs[i+1];
			}
			else if(wcscmp(papszAttrs[i], L"name") == 0)
			{
				name = papszAttrs[i+1];
			}
			else if(wcscmp(papszAttrs[i], L"group") == 0)
			{
				group = papszAttrs[i+1];
			}
			else if(wcscmp(papszAttrs[i], L"cmd") == 0)
			{
				cmd = papszAttrs[i+1];
			}
			else if(wcscmp(papszAttrs[i], L"type") == 0)
			{
				type = _wtoi(papszAttrs[i+1]);
			}
			else if(wcscmp(papszAttrs[i], L"presence") == 0)
			{
				presence = _wtoi(papszAttrs[i+1]);
			}
			else if(wcscmp(papszAttrs[i], L"description") == 0)
			{
				description = papszAttrs[i+1];
			}
		}
		
		if(wcslen(uuid) > 0 && wcslen(name) > 0 && wcslen(cmd) > 0 && mng->getCommand(uuid) == 0) {
			currentCommand = new Command(uuid, name, group, cmd, type, presence, description);
		} else {
			currentCommand = 0;
		}

		return;
	}
}

void wtwShell::XmlReader::OnEndElement(const XML_Char *pszName) {
	if(wcscmp(pszName, L"wtwShell") == 0) {
		mng->install();
		return;
	}

	if(wcscmp(pszName, L"shellCommand") == 0 && currentCommand != 0) {
		mng->commands.push_back(currentCommand);
		currentCommand = 0;
		return;
	}
}

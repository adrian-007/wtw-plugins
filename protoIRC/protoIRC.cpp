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
#include "protoIRC.hpp"
#include "Account.h"
#include "Channel.h"

#include "..\utils\WinApiFile.hpp"
#include "..\utils\XMLWriter.hpp"
#include "Utils.hpp"

#include <wtwInputWindows.h>

IRCManager* IRCManager::inst = nullptr;

IRCManager::IRCManager(HINSTANCE hInst) : hInstance(hInst)
{
	ServerSocket::initLua();

	rootDir = core->getProfileDir();

	wtwACMInterface am;
	initStruct(am);
	am.eventCallback = funcAccountManagerCallback;
	am.cbData = (void*)this;
	am.netClass = PROTO_CLASS;
	am.netName = L"Internet Relay Chat";
	am.flags = ACM_FLAG_ALLOW_DELETE;

	core->fnCall(WTW_ACM_INTERFACE_ADD, am, 0);
}

IRCManager::~IRCManager()
{
	ServerSocket::closeLua();

	core->fnCall(WTW_ACM_INTERFACE_REMOVE, reinterpret_cast<WTW_PARAM>(PROTO_CLASS), 0);

	for(AccountList::iterator i = accounts.begin(); i != accounts.end(); ++i)
	{
		(*i)->close();
	}

	for(AccountList::iterator i = accounts.begin(); i != accounts.end(); ++i)
	{
		delete *i;
	}

	accounts.clear();
}

void IRCManager::load()
{
	WinApiFile f;
	f.open(rootDir + L"protoIRC.xml", WinApiFile::READ_MODE, WinApiFile::ALWAYS_OPEN);
	std::wstring buf;
	buf.resize(f.getFileSize());
	f >> buf;

	XmlReader r(this);
	r.Create();
	if(!r.Parse(buf.c_str(), buf.size()))
	{
		__LOG_F(core, 1, PROTO_CLASS, L"Error while loading xml: %s", r.GetErrorString());
	}
	else
	{
		if(accounts.empty())
		{
			std::wstring message;
			message += L"Nie ustawiono jeszcze �adnego serwera IRC, mo�esz to zrobi� w zak�adce \"Sieci\" w opcjach komunikatora.\r\n";
			message += L"Je�eli nie zamierzasz u�ywa� protoko�u IRC, po prostu wy��cz wtyczk� :)";
			MessageBoxW(0, message.c_str(), L"Brak konta IRC", MB_ICONWARNING);
		}
	}
}

void IRCManager::save()
{
	WinApiFile f;
	f.open(rootDir + L"protoIRC.xml", WinApiFile::WRITE_MODE, WinApiFile::ALWAYS_CREATE);
	unsigned char smarker[3];
	smarker[0] = 0xEF;
	smarker[1] = 0xBB;
	smarker[2] = 0xBF;
	f.write(smarker, sizeof(smarker));

	xml::writer::element root("protoIRC");

	for(AccountList::const_iterator i = accounts.begin(); i != accounts.end(); ++i)
	{
		Account* a = *i;
		xml::writer::element& e = root.getChild("account");

		e.addAttrib("server", utils::text::fromWide(a->server));
		e.addAttrib("port", (int)a->port);
		e.addAttrib("name", utils::text::fromWide(a->name));
		e.addAttrib("user", utils::text::fromWide(a->user));
		e.addAttrib("password", utils::text::fromWide(a->passwd));
		e.addAttrib("nick", utils::text::fromWide(a->nick));
		e.addAttrib("quitMessage", utils::text::fromWide(a->quitMessage));
		e.addAttrib("uuid", utils::text::fromWide(a->uuid));

		for(Account::Channels::const_iterator j = a->channels.begin(); j != a->channels.end(); ++j)
		{
			Channel* c = *j;

			if(c->isSet(Channel::FLAG_TEMPORARY) == true)
			{
				continue;
			}

			xml::writer::element& ec = e.getChild("channel");

			ec.addAttrib("channel", utils::text::fromWide(c->channel));
			ec.addAttrib("name", utils::text::fromWide(c->name));
			ec.addAttrib("password", utils::text::fromWide(c->passwd));
			ec.addAttrib("group", utils::text::fromWide(c->group));
			ec.addAttrib("autojoin", c->isSet(Channel::FLAG_AUTOJOIN) ? 1 : 0);
			ec.addAttrib("uuid", utils::text::fromWide(c->uuid));
		}
	}

	std::string buf = root.getUtf8Header();
	root.print(buf);
	f << buf;
	f.close();
}

void IRCManager::XmlReader::OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs)
{
	if(wcscmp(pszName, L"protoIRC") == 0)
	{
		return;
	}

	if(wcscmp(pszName, L"account") == 0)
	{
		const wchar_t* server = 0;
		int port = 0;
		const wchar_t* name = L"";
		const wchar_t* user = L"";
		const wchar_t* pass = L"";
		const wchar_t* nick = L"";
		const wchar_t* uuid = L"";
		const wchar_t* quitMessage = L"";

		for(int i = 0; papszAttrs[i] != 0; i += 2)
		{
			if(wcscmp(papszAttrs[i], L"server") == 0)
			{
				server = papszAttrs[i+1];
				if(wcslen(server) <= 0) {
					server = 0;
					break;
				}
			}
			else if(wcscmp(papszAttrs[i], L"port") == 0)
			{
				port = _wtoi(papszAttrs[i+1]);
			} 
			else if(wcscmp(papszAttrs[i], L"name") == 0)
			{
				name = papszAttrs[i+1];
				if(wcslen(name) <= 0) {
					server = 0;
					break;
				}
			} 
			else if(wcscmp(papszAttrs[i], L"user") == 0)
			{
				user = papszAttrs[i+1];
			} 
			else if(wcscmp(papszAttrs[i], L"password") == 0)
			{
				pass = papszAttrs[i+1];
			} 
			else if(wcscmp(papszAttrs[i], L"nick") == 0)
			{
				nick = papszAttrs[i+1];
			} 
			else if(wcscmp(papszAttrs[i], L"uuid") == 0)
			{
				uuid = papszAttrs[i+1];
			}
			else if(wcscmp(papszAttrs[i], L"quitMessage") == 0)
			{
				quitMessage = papszAttrs[i+1];
			}
		}

		if(server != 0)
		{
			currentAccount = new Account(server, uuid, name, user, nick, pass);
			currentAccount->port = (unsigned short)port;
			currentAccount->quitMessage = quitMessage;
		}
	}
	else if(wcscmp(pszName, L"channel") == 0)
	{
		const wchar_t* channel = 0;
		const wchar_t* name = L"";
		const wchar_t* pass = L"";
		const wchar_t* group = L"";
		const wchar_t* uuid = L"";
		bool autojoin = false;

		for(int i = 0; papszAttrs[i] != 0; i += 2)
		{
			if(wcscmp(papszAttrs[i], L"channel") == 0)
			{
				channel = papszAttrs[i+1];
				if(wcslen(channel) <= 0) {
					channel = 0;
					break;
				}
			} 
			else if(wcscmp(papszAttrs[i], L"name") == 0)
			{
				name = papszAttrs[i+1];
				if(wcslen(name) <= 0) {
					channel = 0;
					break;
				}
			} 
			else if(wcscmp(papszAttrs[i], L"password") == 0)
			{
				pass = papszAttrs[i+1];
			} 
			else if(wcscmp(papszAttrs[i], L"group") == 0)
			{
				group = papszAttrs[i+1];
			} 
			else if(wcscmp(papszAttrs[i], L"uuid") == 0)
			{
				uuid = papszAttrs[i+1];
			}
			else if(wcscmp(papszAttrs[i], L"autojoin") == 0)
			{
				autojoin = wcscmp(papszAttrs[i+1], L"0") != 0;
			}
		}

		if(channel != 0 && currentAccount != 0)
		{
			currentAccount->setChannel(uuid, channel, group, name, pass, autojoin);
		}
	}
}

void IRCManager::XmlReader::OnEndElement(const XML_Char *pszName)
{
	if(wcscmp(pszName, L"protoIRC") == 0)
	{
	}
	else if(wcscmp(pszName, L"account") == 0)
	{
		mng->accounts.push_back(currentAccount);
		currentAccount = 0;
	}
	else if(wcscmp(pszName, L"channel") == 0)
	{
	}
}

void IRCManager::accountManagerCallback(WTW_PARAM wParam, WTW_PARAM lParam)
{
	wtwACMEvent* event = (wtwACMEvent*)wParam;
	if(wcscmp(event->netClass, PROTO_CLASS) != 0)
	{
		return;
	}

	wtwACMAccount* acc = (wtwACMAccount*)lParam;

	switch(event->event)
	{
	case ACM_EVENT_LIST:
		{
			wtwACMAccount a;
			for(AccountList::const_iterator i = accounts.begin(); i != accounts.end(); ++i)
			{
				initStruct(a);
				a.accountName = (*i)->name.c_str();
				a.netClass = PROTO_CLASS;
				a.netId = (*i)->netId;
				a.flags = ACM_FLAG_ALLOW_DELETE | ACM_FLAG_ALLOW_INFO;
				a.iconId = L"IRC/Icon/available";

				core->fnCall(WTW_ACM_ACCOUNT_ADD, (WTW_PARAM)&a, 0);
			}
			break;
		}
	case ACM_EVENT_ADD:
		{
			if(acc == 0)
			{
				wtwShowMessageWindow dlg;
				initStruct(dlg);
				dlg.windowType = SMB_TYPE_SIMPLE_INPUT;
				dlg.windowTitle = L"Profil";
				dlg.windowCaption = L"Nowy profil";
				dlg.windowMessage = L"Podaj nazw� dla nowego profilu. Wszystkie niedozwolone znaki zostan� automatycznie zast�pione znakiem '_', maksymalna d�ugo�� nazwy: 32 znaki:";
				dlg.pRetBufferSize = 32;
				dlg.pRetBuffer = new wchar_t[dlg.pRetBufferSize+1];
				dlg.pRetBuffer[dlg.pRetBufferSize] = 0;

				WTW_PTR ret = core->fnCall(WTW_SHOW_MESSAGE_BOX, (WTW_PARAM)&dlg, 0);
				if(ret == SMB_RESULT_OK || ret == SMB_RESULT_YES)
				{
					int len = wcslen(dlg.pRetBuffer);
					for(int i = 0; i < len; ++i)
					{
						if(iswalnum(dlg.pRetBuffer[i]) == 0)
						{
							dlg.pRetBuffer[i] = L'_';
						}
					}

					Account* newAcc = new Account(L"", L"", dlg.pRetBuffer);
					accounts.push_back(newAcc);
				}

				delete[] dlg.pRetBuffer;
			}
			break;
		}
	case ACM_EVENT_REMOVE:
		{
			if(acc)
			{
				Account* a = 0;
				for(AccountList::iterator i = accounts.begin(); i != accounts.end(); ++i)
				{
					if((*i)->netId == acc->netId)
					{
						a = *i;
						accounts.erase(i);
						break;
					}
				}
				if(a)
				{
					a->close();
					delete a;
				}
			}
			break;
		}
	default:
		{
			//	__LOG_F(core, 1, L"IRC_ACCMNG", L"event: 0x%X", event->event);
		}
	}
}

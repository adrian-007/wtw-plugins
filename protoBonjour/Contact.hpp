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

#ifndef PROTO_BONJOUR_CONTACT_HPP
#define PROTO_BONJOUR_CONTACT_HPP

#include <dns_sd.h>
#include "TXTRecord.hpp"
#include "../utils/XMLWriter.hpp"
#include "../utils/text.hpp"

#include "ExpatImpl.h"
#include "ApiDataSocket.hpp"

class Contact : public CExpat
{
public:
	enum {
		FLAG_GET_IP4 = 0x0001,
		FLAG_MESSSAGE_TAG = 0x0002,
		FLAG_BODY_TAG = 0x0004,
		FLAG_GOT_STREAM = 0x0008
	};

	void setFlag(unsigned int f) { flags |= f; }
	void unsetFlag(unsigned int f) { flags &= ~f; }
	bool isFlagSet(unsigned int f) { return (flags & f) == f; }

	Contact() : resolveRef(0), sock(INVALID_SOCKET), netId(0)
	{
	}

	~Contact() 
	{
		setSocket(INVALID_SOCKET);

		if(resolveRef != 0) 
		{
			DNSServiceRefDeallocate(resolveRef);
			resolveRef = 0;
		}

		{
			records().setTXTField(TXTRecord::STATUS, "");
			records().setTXTField(TXTRecord::STATUS_MSG, "");
			updatePresence();
		}

		wtwContactDef cnt;
		initStruct(cnt);
		cnt.id = serviceName.c_str();
		cnt.netClass = PROTO_CLASS;
		cnt.netId = *netId;

		WTW_PTR hCnt = 0;
		wtw_t::api::get()->call(WTW_CTL_CONTACT_FIND_EX, &cnt, &hCnt);

		if(hCnt != 0) 
		{
			wtw_t::api::get()->call(WTW_CTL_CONTACT_DELETE, hCnt);
		}

	}

	void openParser(bool initStream = true) 
	{
		CExpat::Create();
		CExpat::EnableStartElementHandler(true);
		CExpat::EnableEndElementHandler(true);
		CExpat::EnableCharacterDataHandler(true);

		if(initStream)
		{
			CExpat::Parse("<stream:stream>", -1, false);
		}
	}

	void closeParser()
	{
		CExpat::Destroy();
	}

	void updateOnList()
	{
		using wtw_t::api;
		std::wstring group;
		cfgGetStr(group, SETTING_GROUP, WTW_DEFAULT_CT_GROUP);

		wtwIconSet wgd;
		initStruct(wgd);

		wgd.iconSetID	= PROTO_CLASS L"/Icon";
		wgd.flags		= WTW_ICS_FLAG_PRELOADED;
		wgd.iType		= WTW_ICSET_TYPE_PRE_TEXT;

		wtwContactListItem item;
		initStruct(item);

		item.itemText				= serviceName.c_str();
		item.contactData.id			= serviceName.c_str();
		item.contactData.netClass	= PROTO_CLASS;
		item.contactData.netId		= *netId;
		item.itemType				= WTW_CTL_TYPE_ELEMENT;
		item.itemGroup				= group.c_str();
		item.flags					= WTW_CTL_FLAG_REPLACE_DUPLICATE;
		item.iconToAdd				= &wgd;

		api::get()->call(WTW_CTL_CONTACT_ADD, &item);
	}

	void updatePresence()
	{
		wtwContactDef c;
		wtwPresenceDef p;
		initStruct(p);
		initStruct(c);
		p.pContactData = &c;

		c.id		= serviceName.c_str();
		c.netClass	= PROTO_CLASS;
		c.netId		= *netId;

		std::string presenceName = records().getTXTField(TXTRecord::STATUS);
		std::wstring desc = utils::text::toWide(records().getTXTField(TXTRecord::STATUS_MSG));

		p.curStatus = WTW_PRESENCE_ONLINE;

		if(_stricmp(presenceName.c_str(), "avail") == 0) 
		{
			p.curStatus = WTW_PRESENCE_ONLINE;
		} 
		else if(_stricmp(presenceName.c_str(), "away") == 0) 
		{
			p.curStatus = WTW_PRESENCE_AWAY;
		} 
		else if(_stricmp(presenceName.c_str(), "dnd") == 0)
		{
			p.curStatus = WTW_PRESENCE_DND;
		}

		p.curDescription = desc.c_str();
		p.curTimeStamp = time(0);

		wtwProtocolEvent ev;
		initStruct(ev);

		ev.netClass = PROTO_CLASS;
		ev.netId = *netId; 
		ev.event = WTW_PEV_PRESENCE_RECV; 
		ev.type = WTW_PEV_TYPE_BEFORE; 

		if(wtw_t::api::get()->call(WTW_PF_CALL_HOOKS, &ev, &p)) 
		{
			return; 
		}

		ev.type = WTW_PEV_TYPE_AFTER; 
		wtw_t::api::get()->call(WTW_PF_CALL_HOOKS, &ev, &p);

		wtw_t::api::get()->call(WTW_CTL_SET_CONTACT_STATE, &p);
	}

	bool sendMessage(const std::string xml)
	{
		if(sock != INVALID_SOCKET && ApiDataSocket::trySend(wtw_t::api::get(), PROTO_CLASS, *netId, xml.c_str(), xml.length(), WTW_RAW_FLAG_TEXT | WTW_RAW_FLAG_UTF))
		{
			int sent = 0;

			do {
				int r = ::send(sock, xml.c_str() + sent, xml.length() - sent, 0);
				if(r <= 0)
				{
					break;
				}
				sent += r;
			} while(sent > 0);

			return true;
		}
		return false;
	}

	void setSocket(SOCKET s)
	{
		if(sock != INVALID_SOCKET)
		{
			std::string endstream = "</stream:stream>";

			ApiDataSocket::trySend(wtw_t::api::get(), PROTO_CLASS, *netId, endstream.c_str(), endstream.length(), WTW_RAW_FLAG_TEXT | WTW_RAW_FLAG_UTF);

			::send(sock, endstream.c_str(), endstream.length(), 0);
			::closesocket(sock);
			closeParser();
		}

		if(s != INVALID_SOCKET)
		{
			openParser();
		}

		sock = s;
	}

	void initStream(const std::string& from, const std::string& to, SOCKET s = INVALID_SOCKET)
	{
		if(s == INVALID_SOCKET && sock == INVALID_SOCKET)
		{
			return;
		}

		xml::writer::element e("stream:stream");
		e.addAttrib("xmlns", "jabber:client");
		e.addAttrib("xmlns:stream", "http://etherx.jabber.org/streams");
		e.addAttrib("from", from);
		e.addAttrib("to", to);
		e.addAttrib("version", "1.0");

		std::string msg = e.getUtf8Header();
		e.print(msg);
		msg.erase(msg.length()-3, 1);

		if(ApiDataSocket::trySend(wtw_t::api::get(), PROTO_CLASS, *netId, msg.c_str(), msg.length(), WTW_RAW_FLAG_UTF | WTW_RAW_FLAG_TEXT))
		{
			if(s == INVALID_SOCKET)
			{
				s = sock;
			}

			if(s != INVALID_SOCKET)
			{
				::send(s, msg.c_str(), msg.length(), 0);
			}
		}
	}

	SOCKET getConnectedSocket()
	{
		if(sock != INVALID_SOCKET)
		{
			return sock;
		}

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = ip4;
		service.sin_port = port;

		if(connect(sock, (sockaddr*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			return INVALID_SOCKET;
		}

		return sock;
	}

	bool isConnected() const 
	{
		return sock != INVALID_SOCKET;
	}

	/* DNS-SD references */
	DNSServiceRef resolveRef;

	std::wstring serviceName;
	unsigned short port;
	unsigned int ip4;
	SOCKET sock;

	TXTRecord& records() 
	{ 
		return mRecords; 
	}

	int* netId;
protected:
	void OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs) 
	{
		if(_wcsicmp(pszName, L"message") == 0)
		{
			setFlag(FLAG_MESSSAGE_TAG);
		}
		else if(_wcsicmp(pszName, L"body") == 0)
		{
			setFlag(FLAG_BODY_TAG);
			messageBuffer.clear();
		}
	}

	void OnEndElement(const XML_Char *pszName)
	{
		if(_wcsicmp(pszName, L"stream:stream") == 0)
		{
			setSocket(INVALID_SOCKET);
		}
		else if(_wcsicmp(pszName, L"message") == 0)
		{
			unsetFlag(FLAG_MESSSAGE_TAG);
		}
		else if(_wcsicmp(pszName, L"body") == 0)
		{
			if(isFlagSet(FLAG_MESSSAGE_TAG))
			{
				{
					std::wstring buf = messageBuffer;
					utils::text::trimSpaces(buf);
					if(buf.length() == 0)
					{
						return;
					}
				}

				wtwProtocolEvent ev;
				initStruct(ev);

				ev.event = WTW_PEV_MESSAGE_RECV; 
				ev.netClass = PROTO_CLASS;
				ev.netId = *netId;

				wtwMessageDef msg;
				initStruct(msg);
				
				msg.contactData.id = serviceName.c_str();
				msg.contactData.netClass = PROTO_CLASS;
				msg.contactData.netId = *netId;
				
				msg.msgMessage = messageBuffer.c_str();
				msg.msgFlags = WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_INCOMING;
				msg.msgTime = time(0);


				ev.type = WTW_PEV_TYPE_BEFORE; 
				if(wtw_t::api::get()->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&msg)) == 0)
				{
					ev.type = WTW_PEV_TYPE_AFTER; 
					wtw_t::api::get()->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&msg));
				}

				messageBuffer.clear();
			}
			unsetFlag(FLAG_BODY_TAG);
			unsetFlag(FLAG_MESSSAGE_TAG);
		}
	}

	void OnCharacterData(const XML_Char *pszData, int nLength) 
	{
		if(isFlagSet(FLAG_MESSSAGE_TAG) && isFlagSet(FLAG_BODY_TAG))
		{
			std::wstring data(pszData, nLength);
			messageBuffer += data;
		}
	}

private:
	unsigned int flags;
	TXTRecord mRecords;

	std::wstring messageBuffer;
};

#endif // PROTO_BONJOUR_CONTACT_HPP

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
#include "protoBonjour.hpp"
#include "Contact.hpp"

using wtw_t::api;

protoBonjour* protoBonjour::inst = 0;

protoBonjour::protoBonjour(const std::wstring& uuid) : status(0), serviceConnection(0), presenceConnection(0), browseConnection(0) {
	Window::Create(FindWindowW(L"{B993D471-D465-43f2-BBA5-DEEA18A1789E}", 0));

	if(uuid.length() > 0)
	{
		this->uuid = uuid;
	} 
	else
	{
		this->uuid = wtw_t::generateUUID();
	}

	{
		const wchar_t* pName = 0;
		api::get()->fnCall(WTW_GET_PROFILE_NAME_PTR_EX, (WTW_PARAM)&pName, 0);
		nick = pName;
	}

	//wtw_t::paths::getProfileName(api::get(), nick);

	// rejestracja protokolu
	{
		wtwProtocolDef pDef;
		initStruct(pDef);

		pDef.protoCaps1 = WTW_PROTO_CAPS1_CHAT | WTW_PROTO_CAPS1_PRESENCE;
		pDef.protoCaps2	= WTW_PROTO_CAPS2_UTF;
		pDef.protoDescr	= L"Bonjour (LAN)";
		pDef.netClass	= PROTO_CLASS;
		pDef.protoName	= PROTO_NAME;
		pDef.netGUID	= uuid.c_str();

		pDef.pInterface.pfDeleteContact	= &protoBonjour::funcRemoveContact;
		pDef.pInterface.pfMoveContact	= &protoBonjour::funcMoveContact;
		pDef.pInterface.cbData			= (void*)this;

		pDef.flags		= WTW_PROTO_FLAG_NO_PUBDIR;
		pDef.protoState = &status;

		api::get()->call(WTW_PROTO_FUNC_ADD, &pDef);
		netId = pDef.netId;

		__LOG_F(api::get(), 1, PROTO_CLASS, L"Assigned NET ID: %d", netId);

		presenceSetFunc = wtwInstProtoFunc(api::get(), PROTO_CLASS, netId, WTW_PF_STATUS_SET, &protoBonjour::funcPresenceSet, this);
		messageFunc = wtwInstProtoFunc(api::get(), PROTO_CLASS, netId, WTW_PF_MESSAGE_SEND, &protoBonjour::funcMessageSend, (void*)this);
	}

	// rejestracja itemow w menu statusow
	{
		addPresenceMenuItem(L"available", L"Dost�pny");
		addPresenceMenuItem(L"away", L"Zaraz wracam");
		addPresenceMenuItem(L"dnd", L"Jestem zaj�ty");
		addPresenceMenuItem(L"unavailable", L"Niedost�pny");
	}

	callPresenceHooks(WTW_PRESENCE_OFFLINE);
	onMenuRebuildHook = api::get()->evHook(WTW_EVENT_MENU_REBUILD, &protoBonjour::funcMenuRebuild, (void*)this);

	{
		std::wstring buf;
		cfgGetStr(buf, SETTING_ADDRESS, L"0.0.0.0");
		wtwSetProtoInfo(api::get(), PROTO_CLASS, netId, buf.c_str(), nick.c_str());
	}

	txtRecords.setTXTField(TXTRecord::NODE, "http://wtw.adrian-007.eu/");
	txtRecords.setTXTField(TXTRecord::TXTVERS, "1");
	txtRecords.setTXTField(TXTRecord::HASH_TYPE, "sha-1");
	txtRecords.setTXTField(TXTRecord::VERSION, "0.0.0.1"); //dunno what to place here...
}

protoBonjour::~protoBonjour() {
	close();
	cleanup();

	Window::DestroyWindow();
	Parser::Destroy();
}

/* DNS-SD functions and methods */
bool protoBonjour::initDNSSDConnection() {
	ScopedMutex mtx(cs);
	DNSServiceErrorType error = DNSServiceCreateConnection(&serviceConnection);

	SOCKET s = (SOCKET)DNSServiceRefSockFD(serviceConnection);
	WSAAsyncSelect(s, m_hWnd, WM_SERVICE_SOCKET, FD_READ | FD_CLOSE);

	return error == kDNSServiceErr_NoError;
}

bool protoBonjour::initDNSSDPresenceService() {
	ScopedMutex mtx(cs);
	TXTRecordRef txt;
	TXTRecordCreate(&txt, 0, 0);

	unsigned short port = static_cast<unsigned short>(cfgGetInt(SETTING_PORT, 8079));
	txtRecords.setTXTField(TXTRecord::P2P_PORT, utils::conv::toString(port));

	const StringMap& records = txtRecords.get();
	for(StringMap::const_iterator i = records.begin(); i != records.end(); ++i) 
	{
		TXTRecordSetValue(&txt, i->first.c_str(), (uint8_t)i->second.length(), i->second.c_str());
	}

	std::string userName = getUserName();
	std::string hostName = getHostName();
	std::string sName;

	if(userName.empty() == false && hostName.empty() == false)
	{
		sName = userName + "@" + hostName;
	}

	presenceConnection = serviceConnection;
	DNSServiceErrorType error = DNSServiceRegister(&presenceConnection, kDNSServiceFlagsShareConnection, 0, sName.empty() ? 0 : sName.c_str(), "_presence._tcp", 0, 0, htons(port), TXTRecordGetLength(&txt), TXTRecordGetBytesPtr(&txt), DNSSDServiceRegister_callback, this);
	TXTRecordDeallocate(&txt);

	return error == kDNSServiceErr_NoError;
}

bool protoBonjour::initDNSSDBrowseService() {
	ScopedMutex mtx(cs);
	browseConnection = serviceConnection;
	DNSServiceErrorType error = DNSServiceBrowse(&browseConnection, kDNSServiceFlagsShareConnection, 0, "_presence._tcp", 0, DNSSDBrowseService_callback, this);
	return error == kDNSServiceErr_NoError;
}

void protoBonjour::closeDNSSDConnection() {
	ScopedMutex mtx(cs);

	for(ContactIter i = contacts.begin(); i != contacts.end(); ++i) 
	{
		delete *i;
	}

	contacts.clear();

	if(browseConnection) 
	{
		DNSServiceRefDeallocate(browseConnection);
		browseConnection = 0;
	}

	if(presenceConnection) 
	{
		DNSServiceRefDeallocate(presenceConnection);
		presenceConnection = 0;
	}

	if(serviceConnection) 
	{
		DNSServiceRefDeallocate(serviceConnection);
		serviceConnection = 0;
	}
}

void protoBonjour::setDNSSDPresence(const std::string& presence, const std::string& message) {
	ScopedMutex mtx(cs);

	if(presenceConnection == 0) 
	{
		return;
	}

	//	std::string statusMsg = message.empty() ? txtRecords.getTXTField(TXTRecord::STATUS_MSG) : message;

	txtRecords.setTXTField(TXTRecord::STATUS, presence);
	if(message.empty())
	{
		txtRecords.remTXTField(TXTRecord::STATUS_MSG);
	}
	else
	{
		txtRecords.setTXTField(TXTRecord::STATUS_MSG, message);
	}

	TXTRecordRef txt;
	TXTRecordCreate(&txt, 0, 0);

	const StringMap& records = txtRecords.get();
	for(StringMap::const_iterator i = records.begin(); i != records.end(); ++i)
	{
		TXTRecordSetValue(&txt, i->first.c_str(), (uint8_t)i->second.length(), i->second.c_str());
	}

	DNSServiceUpdateRecord(presenceConnection, 0, 0, TXTRecordGetLength(&txt), TXTRecordGetBytesPtr(&txt), 0);
	TXTRecordDeallocate(&txt);
}

LRESULT protoBonjour::onServiceSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	SOCKET sock = (SOCKET)wParam;
	if(WSAGETSELECTERROR(lParam)) {
		//TODO: handle error
	} else {
		ScopedMutex mtx(cs);
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			{
				if(DNSServiceProcessResult(serviceConnection) != kDNSServiceErr_NoError)
				{
					__LOG_F(api::get(), WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"Error on processing Bonjour service data!");
				}
				break;
			}
		case FD_CLOSE:
			{
				close();
				break;
			}
		}
	}
	return 0;
}

LRESULT protoBonjour::onServerSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	SOCKET sock = (SOCKET)wParam;
	if(WSAGETSELECTERROR(lParam)) {
		//TODO: handle error
	} else {
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:
			{
				sockaddr_in sin = { 0 };
				int inLen = sizeof(sin);
				SOCKET in = ::accept(server, (sockaddr*)&sin, &inLen);

				inLen = sizeof(sin);
				::getpeername(in, (sockaddr*)&sin, &inLen);

				unsigned int userIp = sin.sin_addr.S_un.S_addr;

				__LOG_F(api::get(), WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"New incoming connection: %s", utils::text::toWide(utils::conv::getIpAsString(userIp)).c_str());

				::WSAAsyncSelect(in, m_hWnd, WM_CONTACT_SOCKET, FD_READ | FD_CLOSE);

				break;
			}
		case FD_CLOSE:
			{
				closesocket(server);
				server = INVALID_SOCKET;
				break;
			}
		}
	}
	return 0;
}

LRESULT protoBonjour::onContactSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	SOCKET sock = (SOCKET)wParam;
	if(WSAGETSELECTERROR(lParam)) {
		//TODO: handle error
	} else {
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			{
				std::string message;

				char buf[1024*10];
				int len;
				do {
					len = ::recv(sock, buf, sizeof(buf), 0);
					if(len > 0)
					{
						message.append(buf, len);
					}
				} while(::GetLastError() == EINTR && len < 0);

				if(ApiDataSocket::tryRecv(api::get(), PROTO_CLASS, netId, message.c_str(), message.length(), WTW_RAW_FLAG_TEXT | WTW_RAW_FLAG_UTF))
				{
					Contact* c = 0;

					for(ContactIter i = contacts.begin(); i != contacts.end(); ++i)
					{
						if((*i)->sock == sock)
						{
							if(!(*i)->Parse(message.c_str(), message.length(), false))
							{
								__LOG_F(api::get(), WTW_LOG_LEVEL_ERROR, PROTO_NAME, L"Error while parsing xml: %s", (*i)->GetErrorString());
							}
							return 0;
						}
					}

					currentSocket = sock;
					Parser::Create();
					Parser::EnableStartElementHandler(true);
					Parser::EnableEndElementHandler(true);

					if(!Parser::Parse(message.c_str(), message.length(), false))
					{
						__LOG_F(api::get(), WTW_LOG_LEVEL_ERROR, PROTO_NAME, L"Error while parsing XML: %s", Parser::GetErrorString());
					}

					Parser::Destroy();
					currentSocket = INVALID_SOCKET;
				}
				break;
			}
		case FD_CLOSE:
			{
				for(ContactIter i = contacts.begin(); i != contacts.end(); ++i)
				{
					if((*i)->sock == sock)
					{
						(*i)->setSocket(INVALID_SOCKET);
					}
				}
				break;
			}
		}
	}
	return 0;
}

void protoBonjour::OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs) {
	const wchar_t* from = 0;
	const wchar_t* to = 0;

	for(int i = 0; papszAttrs[i] != 0; i += 2)
	{
		if(_wcsicmp(papszAttrs[i], L"to") == 0)
		{
			to = papszAttrs[i+1];
			continue;
		}

		if(_wcsicmp(papszAttrs[i], L"from") == 0)
		{
			from = papszAttrs[i+1];
			continue;
		}
	}

	if(currentSocket != INVALID_SOCKET)
	{
		if(from != 0 && to != 0)
		{
			for(ContactIter i = contacts.begin(); i != contacts.end(); ++i)
			{
				if((*i)->serviceName == from && (*i)->sock == INVALID_SOCKET)
				{
					(*i)->setSocket(currentSocket);
					(*i)->initStream(serviceName, utils::text::fromWide(from));
					break;
				}
			}
		}
	}
}

void protoBonjour::OnEndElement(const XML_Char *pszName) {
	//	__LOG(api::get(), pszName, L"element end");
}

/* Shared functions and methods */
std::string protoBonjour::getHostName() const {
	std::string buf;
	buf.resize(256);
	if(::gethostname(&buf[0], buf.size()) != 0)
	{
		buf.clear();
	}
	else
	{
		buf = buf.c_str();
	}

	return buf;
}

std::string protoBonjour::getUserName() const {
	std::string buf;
	buf.resize(1024 * 10);
	DWORD len = buf.size();

	if(::GetUserNameA(&buf[0], &len) != 0)
	{
		buf.resize(len-1);
	}
	else
	{
		buf.clear();
	}
	return buf;
}

/* WTW functions and methods */
void protoBonjour::startup() {
	if(initDNSSDConnection() && initDNSSDPresenceService())
	{
		// utworzenie socket'a na ktorym bedziemy nasluchiwac
		ScopedMutex mtx(cs);

		std::string ip;
		unsigned short port;
		{
			std::wstring buf;
			cfgGetStr(buf, SETTING_ADDRESS, L"0.0.0.0");
			ip = utils::text::fromWide(buf);

			port = static_cast<unsigned short>(cfgGetInt(SETTING_PORT, 8079));
		}

		server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = inet_addr(ip.c_str());
		service.sin_port = htons(port);

		if(bind(server, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) 
		{
			closesocket(server);
			server = INVALID_SOCKET;
		}
		else
		{
			if(listen(server, SOMAXCONN) == SOCKET_ERROR)
			{
				closesocket(server);
				server = INVALID_SOCKET;
			}
			else
			{
				::WSAAsyncSelect(server, m_hWnd, WM_SERVER_SOCKET, FD_ACCEPT | FD_CLOSE);
			}
		}

		if(server != INVALID_SOCKET)
		{
			__LOG_F(api::get(), WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Service listening on local port: %d", port);
			status = WTW_PROTO_STATE_CONNECTED;
		}
	} 

	if(status == 0)
	{
		__LOG_F(api::get(), WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"Could not setup protocol!");
		closeDNSSDConnection();
	}
}

void protoBonjour::updateData() {
	std::wstring buf;

#define UPDATE_FIELD(fld, settingName) cfgGetStr(buf, settingName); if(wcslen(buf.c_str()) > 0) { txtRecords.setTXTField(TXTRecord::fld, utils::text::fromWide(buf)); } else { txtRecords.remTXTField(TXTRecord::fld); }
	UPDATE_FIELD(FIRST_NAME, SETTING_NAME);
	UPDATE_FIELD(LAST_NAME, SETTING_SURNAME);
	UPDATE_FIELD(NICK, SETTING_NICK);
	UPDATE_FIELD(EMAIL, SETTING_MAIL);
	UPDATE_FIELD(JABBER_ID, SETTING_JABBERID);
#undef UPDATE_FIELD

	cfgGetStr(buf, SETTING_ADDRESS, L"0.0.0.0");
	wtwSetProtoInfo(api::get(), PROTO_CLASS, netId, buf.c_str(), nick.c_str());

	if(status != 0)
	{
		std::string prs = txtRecords.getTXTField(TXTRecord::STATUS);
		std::string prsMsg = txtRecords.getTXTField(TXTRecord::STATUS_MSG);

		setDNSSDPresence(prs, prsMsg);
	}
}

void protoBonjour::close() {
	status = 0;
	closeDNSSDConnection();

	if(server != INVALID_SOCKET)
	{
		closesocket(server);
		server = INVALID_SOCKET;
	}
}

void protoBonjour::cleanup() {
	api::get()->call(WTW_PROTO_FUNC_DEL, PROTO_CLASS, netId);

	if(onMenuRebuildHook)
	{
		api::get()->evUnhook(onMenuRebuildHook);
	}

	/*if(presenceSetFunc)
	{
		api::get()->fnDestroy(presenceSetFunc);
	}

	if(messageFunc)
	{
		api::get()->fnDestroy(messageFunc);
	}*/

	presenceSetFunc = messageFunc = onMenuRebuildHook = 0;

	api::get()->call(WTW_CTL_DELETE_ALL, PROTO_CLASS, netId);
}

bool protoBonjour::callPresenceHooks(int presence) {
	wtwProtocolEvent ev;
	initStruct(ev);

	ev.netClass = PROTO_CLASS;
	ev.netId = netId; 
	ev.event = WTW_PEV_PRESENCE_SEND; 
	ev.type = WTW_PEV_TYPE_BEFORE; 

	wtwPresenceDef prs;
	initStruct(prs);
	prs.curStatus = presence;
	prs.curTimeStamp = time(0);

	if(api::get()->call(WTW_PF_CALL_HOOKS, &ev, &prs)) 
		return false; 

	ev.type = WTW_PEV_TYPE_AFTER; 
	api::get()->call(WTW_PF_CALL_HOOKS, &ev, &prs);
	return true;
}

void protoBonjour::addPresenceMenuItem(const wchar_t* presence, const wchar_t* caption) {
	wtwMenuItemDef mid;
	wchar_t iconId[1024] = { 0 };
	wchar_t itemId[1024] = { 0 };
	wchar_t menuID[256] = { 0 };

	initStruct(mid);
	swprintf(itemId, sizeof(itemId), L"%s/%d/Presence/%s", PROTO_CLASS, netId, presence);
	swprintf(menuID, sizeof(menuID), L"%s/%d/Menu", PROTO_CLASS, netId);
	swprintf(iconId, sizeof(iconId), L"%s/Icon/%s", PROTO_CLASS, presence);

	mid.itemId = itemId;
	mid.iconId = iconId;
	mid.menuID = menuID;
	mid.menuCaption = caption;

	mid.callback = &protoBonjour::funcPresenceMenuCallback;
	mid.hModule = api::get()->instance();
	mid.cbData = static_cast<void*>(this);

	api::get()->call(WTW_MENU_ITEM_ADD, &mid, 0);
}

// events
void protoBonjour::OnPresenceChange(const wchar_t* presenceName, const wchar_t* desc) {
	int newPresence = WTW_PRESENCE_OFFLINE;

	if(presenceName != 0)
	{
		if(_wcsicmp(presenceName, L"available") == 0) 
		{
			newPresence = WTW_PRESENCE_ONLINE;
		} 
		else if(_wcsicmp(presenceName, L"away") == 0) 
		{
			newPresence = WTW_PRESENCE_AWAY;
		} 
		else if(_wcsicmp(presenceName, L"dnd") == 0)
		{
			newPresence = WTW_PRESENCE_DND;
		}
	}

	if(callPresenceHooks(newPresence))
	{
		if(status == 0 && newPresence != WTW_PRESENCE_OFFLINE) 
		{
			startup();
		}

		std::string prs;
		std::string msg;

		if(desc == 0) 
		{
			msg = txtRecords.getTXTField(TXTRecord::STATUS_MSG);
		}
		else
		{
			msg = utils::text::fromWide(desc);
		}

		switch(newPresence) {
				case WTW_PRESENCE_ONLINE: prs = "avail"; break;
				case WTW_PRESENCE_AWAY: prs = "away"; break;
				case WTW_PRESENCE_DND: prs = "dnd"; break;
				default:
					{
						close();
						return;
					}
		}
		setDNSSDPresence(prs, msg);
	}
}

/* static methods */
/* DNS-SD */
void protoBonjour::DNSSDServiceRegister_callback(DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
	if(context == 0) 
	{
		return;
	}

	if(errorCode != kDNSServiceErr_NoError) 
	{
		return;
	}

	protoBonjour* proto = reinterpret_cast<protoBonjour*>(context);
	if(proto == 0)
	{
		__LOG(api::get(), PROTO_CLASS, L"Something went very wrong...");
		return;
	}
	ScopedMutex mtx(proto->cs);

	proto->serviceName = name;
	proto->initDNSSDBrowseService();

	std::wstring wname = utils::text::toWide(name);
	std::wstring wrtype = utils::text::toWide(regtype);
	std::wstring wdomain = utils::text::toWide(domain);


	__LOG_F(api::get(), WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Registered service @ name: %s", wname.c_str());
}

void protoBonjour::DNSSDBrowseService_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char* name, const char* type, const char* domain, void* context) {
	if(context == 0) 
	{
		return;
	}

	if(errorCode != kDNSServiceErr_NoError) 
	{
		return;
	}

	if(strcmp(type, "_presence._tcp.") != 0)
	{
		return;
	}

	protoBonjour* proto = reinterpret_cast<protoBonjour*>(context);
	if(proto == 0)
	{
		__LOG(api::get(), PROTO_CLASS, L"Something went very wrong...");
		return;
	}
	ScopedMutex mtx(proto->cs);

	//__LOG(api::get(), L"NAME", utils::text::toWide(name).c_str());
	if(strcmp(proto->serviceName.c_str(), name) == 0) 
	{
		// own browse
		return;
	} 
	else 
	{

	}

	std::wstring wname = utils::text::toWide(name);
	std::wstring wrtype = utils::text::toWide(type);
	std::wstring wdomain = utils::text::toWide(domain);

	if(flags & kDNSServiceFlagsAdd)
	{
		for(ContactCIter i = proto->contacts.begin(); i != proto->contacts.end(); ++i)
		{
			if((*i)->serviceName == wname)
			{
				// found, lol?
				return;
			}
		}

		Contact* cnt = new Contact;
		cnt->netId = &proto->netId;
		cnt->serviceName = wname.c_str();

		cnt->resolveRef = proto->serviceConnection;
		errorCode = DNSServiceResolve(&cnt->resolveRef, kDNSServiceFlagsShareConnection, interfaceIndex, name, type, domain, &protoBonjour::DNSResolveService_callback, (void*)cnt);

		if(errorCode != kDNSServiceErr_NoError)
		{
			delete cnt;
		}
		else
		{
			proto->contacts.push_back(cnt);
			__LOG_F(api::get(), WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Found new host: %s", wname.c_str());
		}
	} 
	else
	{
		for(ContactIter i = proto->contacts.begin(); i != proto->contacts.end(); )
		{
			Contact* c = *i;

			if(c && c->serviceName == wname)
			{
				i = proto->contacts.erase(i);
				__LOG_F(api::get(), WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Host %s removed", c->serviceName.c_str());

				delete c;
				c = 0;

				continue;
			}
			++i;
		}
	}
}

void protoBonjour::DNSResolveService_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char* fullname, const char* hosttarget, uint16_t port, uint16_t txtLen, const unsigned char* txtRecord, void* context) {
	if(context == 0)
	{
		return;
	}

	if(errorCode != kDNSServiceErr_NoError) 
	{
		return;
	}

	std::wstring wname = utils::text::toWide(fullname);
	std::wstring wrtype = utils::text::toWide(hosttarget);
	//std::wstring wdomain = utils::text::toWide(domain);

	Contact* cnt = reinterpret_cast<Contact*>(context);
	protoBonjour* proto = protoBonjour::getInstance();

	if(cnt == 0 || proto == 0)
	{
		return;
	}

	//	ScopedMutex mtx(proto->cs);

	cnt->port = port;
	cnt->records().setRAWData(txtRecord, txtLen);

	if(cnt->isFlagSet(Contact::FLAG_GET_IP4) == false)
	{
		cnt->updateOnList();

		DNSServiceRef ipQueryRef = proto->serviceConnection;
		errorCode = DNSServiceGetAddrInfo(&ipQueryRef, kDNSServiceFlagsShareConnection, interfaceIndex, kDNSServiceProtocol_IPv4, hosttarget, &protoBonjour::DNSSDServiceGetIp4_callback, context);
	}

	cnt->updatePresence();
	//__LOG_F(api::get(), 1, L"resolv", L"name = '%s', hosttarget = '%s'", wname.c_str(), wrtype.c_str());
}

void protoBonjour::DNSSDServiceGetIp4_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context) {
	if(context == 0)
	{
		return;
	}

	if(errorCode != kDNSServiceErr_NoError) 
	{
		return;
	}

	Contact* cnt = reinterpret_cast<Contact*>(context);
	protoBonjour* proto = protoBonjour::getInstance();

	if(cnt == 0 || proto == 0)
	{
		return;
	}

	//	ScopedMutex mtx(proto->cs);

	cnt->ip4 = ((sockaddr_in*)address)->sin_addr.S_un.S_addr;

	std::wstring host = utils::text::toWide(hostname);
	std::wstring ip = utils::text::toWide(utils::conv::getIpAsString(cnt->ip4));

	__LOG_F(api::get(), WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Got host (%s) IPv4: %s", cnt->serviceName.c_str(), ip.c_str());

	if(sdRef)
	{
		DNSServiceRefDeallocate(sdRef);
	}

	//cnt->ipQueryRef = 0;
}

/* WTW */
WTW_PTR protoBonjour::funcPresenceSet(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwPresenceDef* prs = (wtwPresenceDef*)wParam;
	protoBonjour* proto = (protoBonjour*)ptr;

	if(prs == 0 || proto == 0)
	{
		return 0;
	}

	const wchar_t* status = 0;

	switch(prs->curStatus)
	{
	case WTW_PRESENCE_ONLINE:
	case WTW_PRESENCE_CHAT:
		{
		status = L"available";
		break;
		}
	case WTW_PRESENCE_AWAY:
	case WTW_PRESENCE_XA:
		{
		status = L"away";
		break;
		}
	case WTW_PRESENCE_DND:
	case WTW_PRESENCE_INV:
		{
		status = L"dnd";
		}
		break;
	}

	proto->OnPresenceChange(status, prs->curDescription);
	return 0;
}

WTW_PTR protoBonjour::funcMessageSend(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwMessageDef* pMsg = (wtwMessageDef*)wParam;
	protoBonjour* proto = (protoBonjour*)ptr;

	if(pMsg && proto)
	{
		for(ContactCIter i = proto->contacts.begin(); i != proto->contacts.end(); ++i)
		{
			if((*i)->serviceName == pMsg->contactData.id)
			{
				bool isConnected = (*i)->isConnected();

				if(isConnected == false)
				{
					SOCKET s = (*i)->getConnectedSocket();
					if(s == INVALID_SOCKET)
					{
						int errCode = ::WSAGetLastError();
						__LOG_F(api::get(), WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"Could not establish connection with %s! Error code: 0x%x", (*i)->serviceName.c_str(), errCode);
						break;
					}

					(*i)->openParser(false);
					::WSAAsyncSelect(s, proto->m_hWnd, WM_CONTACT_SOCKET, FD_READ | FD_CLOSE);
					(*i)->initStream(proto->serviceName.c_str(), utils::text::fromWide((*i)->serviceName.c_str()));
				}

				xml::writer::element root("message");
				root.addAttrib("from", proto->serviceName);
				root.addAttrib("to", utils::text::fromWide((*i)->serviceName));
				xml::writer::element& m = root.getChild("body");
				m.addData(utils::text::fromWide(pMsg->msgMessage));

				if(pMsg->msgFlags & WTW_MESSAGE_FLAG_NOHTMLESC)
				{
					xml::writer::element& html = root.getChild("html");
					html.addAttrib("xmlns", "http://www.w3.org/1999/xhtml");

					xml::writer::element& body = html.getChild("body").getChild("font");
					body.addData(utils::text::fromWide(pMsg->msgMessage));
				}

				std::string data;
				root.print(data, false);

				(*i)->sendMessage(data);
				return 0;
			}
		}
	}
	return 1;
}

WTW_PTR protoBonjour::funcPresenceMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	wtwMenuItemDef* mid = (wtwMenuItemDef*)wParam;
	wtwMenuPopupInfo* pi = (wtwMenuPopupInfo*)lParam;
	protoBonjour* proto = (protoBonjour*)ptr;

	wchar_t buf[1024];
	int netId;
	if(swscanf(mid->itemId, PROTO_CLASS L"/%d/Presence/%s", &netId, buf) == 2 && netId == proto->netId) {
		proto->OnPresenceChange(buf);
	}
	return 0;
}

WTW_PTR protoBonjour::funcMoveContact(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	return 0;
}

WTW_PTR protoBonjour::funcRemoveContact(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	return 0;
}

WTW_PTR protoBonjour::funcMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	return 0;
}

WTW_PTR protoBonjour::funcMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
	return 0;
}

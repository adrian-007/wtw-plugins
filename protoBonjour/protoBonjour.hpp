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

#ifndef PROTO_BONJOUR_HPP
#define PROTO_BONJOUR_HPP

#include <vector>

#include <dns_sd.h>
#include "TXTRecord.hpp"
#include "Thread.hpp"
#include "ExpatImpl.h"

#define WM_SERVICE_SOCKET (WM_USER + 1)
#define WM_SERVER_SOCKET (WM_USER + 2)
#define WM_CONTACT_SOCKET (WM_USER + 3)

class Contact;

class protoBonjour : public CWindowImpl<protoBonjour>, CExpat {
	typedef CWindowImpl<protoBonjour> Window;
	typedef CExpat Parser;

public:
	protoBonjour(const std::wstring& uuid);
	~protoBonjour();

	static void create(protoBonjour* r) {
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
	static protoBonjour* getInstance() {
		return inst;
	}

	void startup();
	void updateData();
	void close();
	void cleanup();

protected:
	BEGIN_MSG_MAP(protoBonjour)
		MESSAGE_HANDLER(WM_SERVICE_SOCKET, onServiceSelect)
		MESSAGE_HANDLER(WM_SERVER_SOCKET, onServerSelect)
		MESSAGE_HANDLER(WM_CONTACT_SOCKET, onContactSelect)
	END_MSG_MAP()

	LRESULT onServiceSelect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onServerSelect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onContactSelect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void OnPresenceChange(const wchar_t* presenceName, const wchar_t* desc = 0);

	void OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs);
	void OnEndElement(const XML_Char *pszName);

	SOCKET currentSocket;

private:
	/* DNS-SD variables, functions and methods */
	bool initDNSSDConnection();
	bool initDNSSDPresenceService();
	bool initDNSSDBrowseService();
	void closeDNSSDConnection();

	void setDNSSDPresence(const std::string& presence, const std::string& message);

	DNSServiceRef serviceConnection; //from this one we only read
	DNSServiceRef presenceConnection; //using this one we only write
	DNSServiceRef browseConnection;

	SOCKET server;

	static void DNSSD_API DNSSDServiceRegister_callback(DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context);
	static void DNSSD_API DNSSDBrowseService_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
		const char* name, const char* type, const char* domain, void* context);
	static void DNSSD_API DNSResolveService_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
		const char* fullname, const char* hosttarget, uint16_t port, uint16_t txtLen, const unsigned char* txtRecord, void* context);
	static void DNSSD_API DNSSDServiceGetIp4_callback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, 
		const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context);

	/* Shared variables, functions and methods */
	typedef std::vector<Contact*> ContactList;
	typedef ContactList::iterator ContactIter;
	typedef ContactList::const_iterator ContactCIter;

	CriticalSection cs;
	ContactList contacts;
	TXTRecord txtRecords;
	std::string serviceName;

	std::string getHostName() const;
	std::string getUserName() const;

	/* WTW variables, functions and methods */
	DWORD status;
	int netId;

	HANDLE presenceSetFunc;
	HANDLE messageFunc;
	HANDLE onMenuRebuildHook;

	std::wstring uuid;
	std::wstring nick;

	void addPresenceMenuItem(const wchar_t* presence, const wchar_t* caption);
	bool callPresenceHooks(int presence);

	static WTW_PTR funcPresenceSet(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMessageSend(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcPresenceMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMoveContact(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcRemoveContact(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
	static WTW_PTR funcMenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);

	static protoBonjour* inst;
};

#endif

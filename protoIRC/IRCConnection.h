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

#ifndef IRC_CONNECTION_H
#define IRC_CONNECTION_H

//#include "Thread.h"
//#include "DNSLookup.hpp"
//#include <vector>

//#include "libircclient.h"
struct irc_session_t { };

class IRCConnection /*: public Thread*/ {
public:
	IRCConnection();
	void terminate();

	bool isEstablished() { return false; /*session != 0 && irc_is_connected(session);*/ }
	bool connect(const std::wstring& address, unsigned short port, const std::wstring& user, const std::wstring& nick, const std::wstring& pass);
	void disconnect(const std::wstring& reason = L"");
	void joinChannel(const std::wstring& channel, const std::wstring& pass);
	void partChannel(const std::wstring& channel, const std::wstring& reason = L"");
	void sendMessage(const std::wstring& channel, const std::wstring& message, bool meCmd = false);
	void setAway(bool away, const std::wstring& message = L"be right back");
	void setTopic(const std::wstring& channel, const std::wstring& topic);
	void requestTopic(const std::wstring& channel);
	void changeNick(const std::wstring& newNick);
	void sendCTCPRequest(const std::wstring& to, const std::wstring& request);
	void kickUser(const std::wstring& channel, const std::wstring& victim, const std::wstring& reason = L"");
	void setMode(const std::wstring& channel, const std::wstring& user, const std::wstring& mode);

	virtual void OnConnecting() = 0;
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnIncomingMessage(const std::wstring& channel, const std::wstring& from, const std::wstring& msg) = 0;
	virtual void OnUserJoin(const std::wstring& channel, const std::wstring& user) = 0;
	virtual void OnUserPart(const std::wstring& channel, const std::wstring& user, const std::wstring& reason = L"") = 0;
	virtual void OnQuit(const std::wstring& user, const std::wstring& r = L"") = 0;
	virtual void OnNickList(const std::wstring& channel, const std::vector<std::wstring>& list) = 0;
	virtual void OnAway(const wchar_t* nick, bool away) = 0;
	virtual void OnTopic(const std::wstring& who, const std::wstring& channel, const std::wstring& topic) = 0;
	virtual void OnNick(const std::wstring& oldNick, const std::wstring& newNick) = 0;
	virtual bool NickCompare(const std::wstring& nick) = 0;
	virtual void OnActionCommand(const std::wstring& channel, const std::wstring& from, const std::wstring& msg) = 0;
	virtual void OnCTCPReply(const std::wstring& from, const std::wstring& reply) = 0;
	virtual void OnKick(const std::wstring& channel, const std::wstring& who, const std::wstring& victim, const std::wstring& reason) = 0;
	virtual void OnMode(const std::wstring& channel, const std::wstring& who, const std::wstring& user, const std::wstring& mode) = 0;
	virtual void OnUmode(const std::wstring& mode) = 0;
	virtual void OnNotice(const std::wstring& channel, const std::wstring& from, const std::wstring& message) = 0;
	virtual void OnError(const std::wstring& channel, const std::wstring& message) = 0;

	virtual bool callMessageHooks(const std::wstring& channel, const std::wstring& message, bool incoming) = 0;

	std::wstring quitMessage;

protected:
//	CriticalSection cs;
//	DNSLookup dns;
private:
	void initIRCConnection();

//	irc_session_s* session;
	void threadMain();

	struct ConnectionInfo {
		std::string address;
		std::string nick;
		std::string pass;
		std::string user;
		unsigned short port;
	}ci;

	static void evn_onConnect(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onJoin(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onPart(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onQuit(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onChannel(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onTopic(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onNick(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onNumeric(irc_session_t* s, unsigned int event, const char* origin, const char** params, unsigned int count);
	static void evn_onCTCPReply(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onMeCommand(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onKick(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onMode(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onUmode(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);
	static void evn_onNotice(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count);

//	bool stop;
//	bool notify;
//	bool autoreconnect;
//	unsigned short failedAttempts;
//	bool dropOnBadNick;

//	Semaphore s;
};

#endif

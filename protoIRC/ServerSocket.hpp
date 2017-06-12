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

#pragma once

#include <boost/noncopyable.hpp>

#include <cpp/NetLib.h>
#include "DNSLookup.hpp"

//#include <lunar.h>
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

class ServerSocket : protected wtw::CNetworkSocket
{
public:
	struct Buffer
	{
		~Buffer();
		bool flush();

		Buffer& operator << (const wchar_t* str);
		Buffer& operator << (const char* str);
		Buffer& operator << (const std::wstring& str);
		Buffer& operator << (const std::string& str);

		template<typename T>
		Buffer& operator << (const T& str);

	private:
		friend class ServerSocket;
		Buffer(ServerSocket& socket, bool _lua = true);
		Buffer(const Buffer&);

		bool lua;
		std::string buf;
		ServerSocket& sock;
	};

	struct Trigger
	{
		typedef std::vector<Trigger> list;

		std::string trigger;
		std::string command;
		bool stopParsing;
	};

	ServerSocket();
	~ServerSocket();

	static void initLua();
	static void closeLua();

	void connect(std::wstring addr, unsigned short port, const std::string& nick, const std::string& username = "", const std::string& realname = "", const std::string& pass = "");
	void disconnect(const std::wstring& message = L"");

	Buffer getSendBuffer(bool lua = true)
	{
		return Buffer(*this, lua);
	}

	bool isConnected() const
	{
		return status == WTW_PROTO_STATE_CONNECTED;;
	}

	void joinChannel(const std::wstring& channel, const std::wstring& pass);
	void partChannel(const std::wstring& channel, const std::wstring& reason = L"");
	void changeNick(const std::wstring& newNick);
	void sendMessage(const std::wstring& channel, const std::wstring& message, bool meCmd = false);
	void ctcpRequest(const std::wstring& to, const std::wstring& request);
	void sendNotice(const std::wstring& to, const std::wstring& message, const std::wstring ctcp = L"");
	void setTopic(const std::wstring& channel, const std::wstring& topic);
	void setAway(const std::wstring& message = L"");

	void requestTopic(const std::wstring& channel) {}
	void kickUser(const std::wstring& channel, const std::wstring& victim, const std::wstring& reason = L"") {}
	void setMode(const std::wstring& channel, const std::wstring& user, const std::wstring& mode) {}

	int netId;
	DWORD status;

	std::string currentNick;
	std::string currentUser;
	std::string currentName;
	std::string currentPass;

protected:
#define DECLARE_EVENT(name,...) \
	virtual void name(__VA_ARGS__) { }

	DECLARE_EVENT(irc_onConnected);
	DECLARE_EVENT(irc_onDisconnected);
	DECLARE_EVENT(irc_onReady);
	DECLARE_EVENT(irc_onServerLog, const std::wstring& msg);
	DECLARE_EVENT(irc_onIncomingMessage, const std::wstring& channel, const std::wstring& from, const std::wstring& msg);
	DECLARE_EVENT(irc_onUserJoin, const std::wstring& channel, const std::wstring& user, const std::wstring& host);
	DECLARE_EVENT(irc_onUserPart, const std::wstring& channel, const std::wstring& user, const std::wstring& host, const std::wstring& reason = L"");
	DECLARE_EVENT(irc_onUserQuit, const std::wstring& user, const std::wstring& reason = L"");
	DECLARE_EVENT(irc_onNickList, const std::wstring& channel, const StringVectorW& users);
	DECLARE_EVENT(irc_onTopic, const std::wstring& channel, const std::wstring& topic, const std::wstring& setBy);
	DECLARE_EVENT(irc_onCTCP, const std::wstring& target, const std::wstring& sender, const std::wstring& cmd, const std::wstring& params);
	DECLARE_EVENT(irc_onNick, const std::wstring& oldNick, const std::wstring& newNick);
	DECLARE_EVENT(irc_onAway, const std::wstring& sender, bool away);

#undef DECLARE_EVENT

	bool callMessageHooks(const std::wstring& target, const std::wstring& sender, const std::wstring& message, bool incoming);

private:
	typedef wtw::CNetworkSocket baseClass;

	void dispatch(const std::string& prefix, const std::string& cmd, StringVectorA& params);
	void send(const std::string& line, bool lua = true);
	std::string stripNick(const std::string& prefix) const;
	std::string stripHost(const std::string& prefix) const;

	void onConnect(int bError);
	void onDestroy();
	void onClose();
	void onSendCompleted();
	void onReceive(const char *pData, int pDataLen);
	void onParseBuffer(const char *pData, int pDataLen, bool lua = true); // avoid loops between cpp and lua
	void onSSLStatus(int bSucceded);
	void onError();

	bool gracefulClose;
	DNSLookup dns;
	std::wstring address;
	unsigned short port;

	std::string incomingBuffer;

	static int socketId;
	static WTW_PTR onReconnectTimer(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);

	static lua_State* luaSTATE;
	static const luaL_Reg protoIRC_lua_api[];

	static int api_LogConsole(lua_State* L);
	static int api_LogTab(lua_State* L);
	static int api_Parse(lua_State* L);
	static int api_Send(lua_State* L);
	static int api_ProfileDir(lua_State* L);

	int lua_onOutgoingMessage(const std::string& line);
	int lua_onIncomingMessage(const std::string& line);
};

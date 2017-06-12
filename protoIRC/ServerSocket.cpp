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
#include "mpl_str_hash.hpp"
#include "ServerSocket.hpp"
#include "../utils/text.hpp"

#include <regex>

int ServerSocket::socketId = 0;

ServerSocket::Buffer::Buffer(ServerSocket& socket, bool _lua /*= true*/) : sock(socket), lua(_lua)
{
}

ServerSocket::Buffer::Buffer(const Buffer& buffer) : sock(buffer.sock), lua(buffer.lua)
{
}

ServerSocket::Buffer::~Buffer()
{
	flush();
}

bool ServerSocket::Buffer::flush()
{
	if(sock.isConnected())
	{
		size_t i = buf.length();

		if(i > 2)
		{
			i = 0;
			while((i = buf.find_first_of("\r\n", i)) != std::string::npos)
			{
				buf.erase(i, 1);
			}

			buf += "\r\n";

			sock.send(buf, lua);
			buf.clear();

			return true;
		}
	}

	return false;
}

ServerSocket::Buffer& ServerSocket::Buffer::operator << (const wchar_t* str)
{
	if(str != 0)
	{
		buf += utils::text::fromWide(str);
	}
	return *this;
}

ServerSocket::Buffer& ServerSocket::Buffer::operator << (const char* str)
{
	if(str != 0)
	{
		buf += str;
	}
	return *this;
}

ServerSocket::Buffer& ServerSocket::Buffer::operator << (const std::wstring& str)
{
	buf += utils::text::fromWide(str);
	return *this;
}

ServerSocket::Buffer& ServerSocket::Buffer::operator << (const std::string& str)
{
	buf += str;
	return *this;
}

template<typename T>
ServerSocket::Buffer& ServerSocket::Buffer::operator << (const T& str)
{
	try
	{
		buf += boost::str(boost::format("%1%") % str);
	}
	catch(...)
	{
		//TODO buf.clear() ?
	}

	return *this;
}

ServerSocket::ServerSocket() : wtw::CNetworkSocket(boost::str(boost::wformat(L"%1%/%2%") % PROTO_CLASS % (++socketId)).c_str()), status(0), netId(-1), gracefulClose(true)
{
	std::wstring timerId = boost::str(boost::wformat(L"IRC/ReconnectTimer/%1%") % baseClass::m_socketId.c_str());
	wtwTimerDef t;
	initStruct(t);

	t.id = timerId.c_str();
	t.sleepTime = 3000;
	t.callback = ServerSocket::onReconnectTimer;
	t.cbData = static_cast<void*>(this);
	core->fnCall(WTW_TIMER_CREATE, t, 0);
}

ServerSocket::~ServerSocket()
{
	std::wstring timerId = boost::str(boost::wformat(L"IRC/ReconnectTimer/%1%") % baseClass::m_socketId.c_str());
	core->fnCall(WTW_TIMER_DESTROY, (WTW_PARAM)timerId.c_str(), 0);

	close();
	destroy();
	status = 0;

	--socketId;
}

void ServerSocket::connect(std::wstring addr, unsigned short serverPort, const std::string& nick, const std::string& username /*= ""*/, const std::string& realname /*= ""*/, const std::string& pass /*= ""*/)
{
	destroy();

	address = addr;
	port = serverPort;
	currentNick = nick;
	currentPass = pass;
	currentUser = username;
	currentName = realname;

	bool ssl = addr.compare(0, 6, L"ssl://") == 0;
	create(ssl ? 1 : 0);

	if(ssl == true || addr.compare(0, 6, L"irc://") == 0)
	{
		addr.erase(0, 6);
	}

	if(dns.size() == 0 && dns.setDomain(utils::text::fromWide(addr)))
	{
		core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Found %d IP(s) for %s", dns.size(), address.c_str());
	}

	addr = utils::text::toWide(dns.front());
	dns.pop_front();

	int ret = baseClass::connect(addr.c_str(), port);
	core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Connecting to %s...", addr.c_str());

	status = WTW_PROTO_STATE_CONNECTING;
	gracefulClose = false;
}

void ServerSocket::disconnect(const std::wstring& message /*= L""*/)
{
	gracefulClose = true;

	{
		Buffer& buf = getSendBuffer();
		buf << "QUIT";

		if(message.empty() == false)
		{
			buf << " :" << message;
		}
	}

	//close();
	//destroy();
	//status = 0;
	//dns.clear();
}

WTW_PTR ServerSocket::onReconnectTimer(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwTimerEvent* e = reinterpret_cast<wtwTimerEvent*>(wParam);
	ServerSocket* sock = reinterpret_cast<ServerSocket*>(ptr);

	if(!e || !sock)
	{
		return 0;
	}

	if(wcsncmp(e->id, L"IRC/ReconnectTimer/", 19) == 0 && e->event == WTW_TIMER_EVENT_TICK && sock->status == 0 && sock->gracefulClose == false)
	{
		sock->connect(sock->address, sock->port, sock->currentNick, sock->currentUser, sock->currentName, sock->currentPass);
	}

	return 0;
}

void ServerSocket::send(const std::string& line, bool lua /*= true*/)
{
	wtwProtocolEvent ev;
	wtwRawDataDef rd;
	
	initStruct(ev);
	initStruct(rd);

	ev.event = WTW_PEV_RAW_DATA_SEND; 
	ev.netClass = PROTO_CLASS;
	ev.netId = netId; 

	rd.pData = line.c_str();
	rd.pDataLen = line.size();
	rd.flags = WTW_RAW_FLAG_TEXT | WTW_RAW_FLAG_UTF;

	ev.type = WTW_PEV_TYPE_BEFORE; 

	if(core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&rd)) == 0)
	{
		ev.type = WTW_PEV_TYPE_AFTER; 
		core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&rd));

		irc_onServerLog(utils::text::toWide(line));

		if(lua && lua_onOutgoingMessage(line) != 0)
		{
			return;
		}

		// can process message
		int ret = baseClass::send(line.c_str(), line.size());
		//core->logFormatted(1, L"send", L"'%s' 0x%08X", utils::text::toWide(line).c_str(), ret);
	}
}

void ServerSocket::onConnect(int bError)
{
	if(bError)
	{
		status = 0;
		core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Connection error (%s)", address.c_str());
	}
	else
	{
		if(address.compare(0, 6, L"ssl://") == 0)
		{
			startSSL();
		}

		status = WTW_PROTO_STATE_CONNECTED;
		core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Connected (%s)", address.c_str());
		irc_onConnected();

		Buffer& buf = getSendBuffer();
		if(currentPass.empty() == false)
		{
			buf << "PASS " << currentPass;
			buf.flush();
		}

		if(currentNick.empty() == false)
		{
			buf << "NICK " << currentNick;
			buf.flush();
		}

		buf << "USER " << (currentUser.empty() ? "nobody" : currentUser) << " 0 * :" << (currentName.empty() ? "nobody" : currentName);
		buf.flush();
	}
}

void ServerSocket::onDestroy()
{
	status = 0;
	core->logFormatted(1, PROTO_CLASS, L"Socket destroyed");
}

void ServerSocket::onClose()
{
	status = 0;
	core->logFormatted(1, PROTO_CLASS, L"Socket closed");
	irc_onDisconnected();
}

void ServerSocket::onSendCompleted()
{
	//core->logFormatted(1, PROTO_CLASS, L"Socket send completed");
}

void ServerSocket::onReceive(const char *pData, int pDataLen)
{
	onParseBuffer(pData, pDataLen, true);
}

void ServerSocket::onParseBuffer(const char *pData, int pDataLen, bool lua /*= true*/)
{
	if(pDataLen > 0 && pData)
	{
		incomingBuffer.append(pData, pDataLen);
	}

	std::string::size_type i = 0;

	while((i = incomingBuffer.find("\r\n")) != std::string::npos)
	{
		std::string line = incomingBuffer.substr(0, i+2);
		incomingBuffer.erase(0, i+2);

		//core->logFormatted(1, L"recv", L"'%s'", utils::text::toWide(line).c_str());

		wtwProtocolEvent ev;
		initStruct(ev);

		//ev.event = outgoing ? WTW_PEV_RAW_DATA_SEND : WTW_PEV_RAW_DATA_RECV;
		ev.event = WTW_PEV_RAW_DATA_RECV; 
		ev.netClass = PROTO_CLASS;
		ev.netId = netId; 

		wtwRawDataDef rd;
		initStruct(rd);
		rd.pData = line.c_str();
		rd.pDataLen = line.size();
		rd.flags = WTW_RAW_FLAG_TEXT | WTW_RAW_FLAG_UTF;

		ev.type = WTW_PEV_TYPE_BEFORE; 
		
		if(core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&rd)) == 0)
		{
			ev.type = WTW_PEV_TYPE_AFTER; 
			core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&rd));

			irc_onServerLog(utils::text::toWide(line));

			if(lua && lua_onIncomingMessage(line) != 0)
			{
				continue;
			}

			//remove \r\n
			line.erase(line.size()-2);

			std::string prefix;
			std::string cmd;
			StringVectorA params;

			std::string::size_type start = 0;
			std::string::size_type end = line.find(" ", start);

			if(line[0] == ':')
			{
				prefix = line.substr(1, end-1);

				start = end + 1;
				end = line.find(" ", start);
			}

			if(end == std::string::npos)
			{
				// something went wrong...
				continue;
			}

			cmd = line.substr(start, end-start);
			start = end + 1;

			// dirty hack but works
			line += " ";

			while((end = line.find(" ", start)) != std::string::npos)
			{
				if(line[start] == ':')
				{
					// last param
					params.push_back(line.substr(start+1, line.length()-start-2));
					break;
				}
				else
				{
					params.push_back(line.substr(start, end-start));
				}

				start = end + 1;
			}

			dispatch(prefix, cmd, params);
		}
	}

	if(incomingBuffer.empty() == false)
	{
		core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"remaining buffer: '%s'", utils::text::toWide(incomingBuffer).c_str());
	}
}

void ServerSocket::onSSLStatus(int bSucceded)
{
	core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"SSL status: %d", bSucceded);
}

void ServerSocket::onError()
{
	status = 0;
	core->logFormatted(1, PROTO_CLASS, L"Socket error");
	close();
}

void ServerSocket::dispatch(const std::string& prefix, const std::string& cmd, StringVectorA& params)
{
	//core->logFormatted(1, PROTO_CLASS, L"command: '%s' (prefix: '%s')", utils::text::toWide(cmd).c_str(), utils::text::toWide(prefix).c_str());
	//for(int i = 0; i < params.size(); ++i)
	//{
	//	core->logFormatted(1, PROTO_CLASS, L"param_%d: '%s'", i, utils::text::toWide(params[i]).c_str());
	//}

	string_hash::HashType cmdInt = string_hash::get_hash(cmd.c_str(), cmd.size());

	switch(cmdInt)
	{
	case string_hash::hash<'P','I','N','G'>::value:
		{
			if(params.size() > 0)
			{
				getSendBuffer() << "PONG :" << params[0];
			}

			if(luaSTATE != 0)
			{
				lua_gc(luaSTATE, LUA_GCCOLLECT, 0);
			}

			break;
		}
	case string_hash::hash<'J','O','I','N'>::value:
		{
			if(params.size() > 0)
			{
				std::string sender = stripNick(prefix);
				std::string host = stripHost(prefix);
				std::string target = params[0];

				irc_onUserJoin(utils::text::toWide(target), utils::text::toWide(sender), utils::text::toWide(host));
			}
			break;
		}
	case string_hash::hash<'P','A','R','T'>::value:
		{
			if(params.size() > 0)
			{
				std::string sender = stripNick(prefix);
				std::string target = params[0];
				std::string reason = params.size() == 2 ? params[1] : "";

				irc_onUserPart(utils::text::toWide(target), utils::text::toWide(sender), utils::text::toWide(reason));
			}
			break;
		}
	case string_hash::hash<'Q','U','I','T'>::value:
		{
			std::string sender = stripNick(prefix);
			std::string reason = params.size() > 0 ? params[0] : "";

			irc_onUserQuit(utils::text::toWide(sender), utils::text::toWide(reason));
			break;
		}
	case string_hash::hash<'3','5','3'>::value: // nick list
		{
			if(params.size() > 0)
			{
				std::wstring ch = utils::text::toWide(params[params.size()-2]);
				const std::vector<std::wstring>& list = Utils::split(utils::text::toWide(params[params.size()-1]));
				irc_onNickList(ch, list);
			}
			break;
		}
	case string_hash::hash<'P','R','I','V','M','S','G'>::value:
		{
			if(params.size() > 1)
			{
				std::wstring sender = utils::text::toWide(stripNick(prefix));
				std::wstring target = utils::text::toWide(params[0]);
				std::wstring message = utils::text::toWide(params[1]);
				std::wstring ctcp;
				size_t i = std::wstring::npos;

				if(message.size() > 0 && message[0] == 0x01 && message[message.size()-1] == 0x01)
				{
					i = message.find(L" ");
					
					if(i != std::wstring::npos)
					{
						ctcp = message.substr(1, i-1);
						message = message.substr(i+1, message.size()-i-2);
					}
					else
					{
						// broken or what?
						ctcp = message.substr(1, message.size()-2);
						message.clear();
					}

					irc_onCTCP(target, sender, ctcp, message);
				}
				else
				{
					irc_onIncomingMessage(target, sender, message);
				}

				/*wtwProtocolEvent ev;
				wtwMessageDef m;

				initStruct(ev);
				initStruct(m);

				m.contactData.netClass = ev.netClass = PROTO_CLASS;
				m.contactData.netId = ev.netId = netId;
				m.contactData.id = target.c_str(); // sender? hm hm, target is channel id in userlist

				m.msgFlags |= WTW_MESSAGE_FLAG_CONFERENCE | WTW_MESSAGE_FLAG_NOAUTORECV | WTW_MESSAGE_FLAG_INCOMING;
				m.msgTime = time(0);
				m.msgMessage = message.c_str();
				m.msgSubject = sender.c_str(); // TODO: well, find a better place for it

				ev.event = WTW_PEV_MESSAGE_RECV; 
				ev.type = WTW_PEV_TYPE_BEFORE; 

				if(core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&m)) == 0) 
				{
					ev.type = WTW_PEV_TYPE_AFTER; 
					core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&m));

					if(ctcp.empty() == false)
					{
						//message = message.substr(i+1);
						irc_onCTCP(target, sender, ctcp, message.substr(i+1, message.size()-i-2));
					}
					else
					{
						irc_onIncomingMessage(target, sender, message);
					}
				}*/
			}
			break;
		}
	case string_hash::hash<'4','2','2'>::value:						// MOTD file missing
	case string_hash::hash<'3','7','6'>::value:						// end of /MOTD
		{
			irc_onReady();
			break;
		}
	case string_hash::hash<'3','3','2'>::value:						// topic reply
		{
			if(params.size() > 2)
			{
				std::wstring channel = utils::text::toWide(params[1]);
				std::wstring topic = utils::text::toWide(params[2]);

				irc_onTopic(channel, topic, L"");
			}
			break;
		}
	case string_hash::hash<'T','O','P','I','C'>::value:
		{
			if(params.size() > 1)
			{
				std::wstring setBy = utils::text::toWide(stripNick(prefix));
				std::wstring channel = utils::text::toWide(params[0]);
				std::wstring topic = utils::text::toWide(params[1]);

				irc_onTopic(channel, topic, setBy);
			}
			break;
		}
	case string_hash::hash<'N','I','C','K'>::value:
		{
			if(params.size() > 0)
			{
				std::wstring oldNick = utils::text::toWide(stripNick(prefix));
				std::wstring newNick = utils::text::toWide(params[0]);

				if(_stricmp(params[0].c_str(), currentNick.c_str()) == 0)
				{
					currentNick = params[0];
				}

				irc_onNick(oldNick, newNick);
			}
			break;
		}
	case string_hash::hash<'3','0','5'>::value:
	case string_hash::hash<'3','0','6'>::value:
		{
			if(params.size() > 0)
			{
				std::wstring sender = utils::text::toWide(params[0]);
				irc_onAway(sender, cmdInt == string_hash::hash<'3','0','6'>::value);
			}
			break;
		}
	default:
		{
			core->logFormatted(WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Unhandled command: %s (0x%08X)", utils::text::toWide(cmd).c_str(), cmdInt);
			break;
		}
	}
}

bool ServerSocket::callMessageHooks(const std::wstring& target, const std::wstring& sender, const std::wstring& message, bool incoming)
{
	wtwProtocolEvent ev;
	wtwMessageDef m;

	initStruct(ev);
	initStruct(m);

	m.contactData.netClass = ev.netClass = PROTO_CLASS;
	m.contactData.netId = ev.netId = netId;
	m.contactData.id = target.c_str(); // sender? hm hm, target is channel id in userlist

	m.msgFlags |= WTW_MESSAGE_FLAG_CONFERENCE | WTW_MESSAGE_FLAG_NOAUTORECV | (incoming ? WTW_MESSAGE_FLAG_INCOMING : WTW_MESSAGE_FLAG_OUTGOING);
	m.msgTime = time(0);
	m.msgMessage = message.c_str();
	//m.msgSubject = sender.c_str(); // TODO: well, find a better place for it

	ev.event = incoming ? WTW_PEV_MESSAGE_RECV : WTW_PEV_MESSAGE_SEND; 
	ev.type = WTW_PEV_TYPE_BEFORE; 

	if(core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&m)) == 0) 
	{
		ev.type = WTW_PEV_TYPE_AFTER; 
		core->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&m));

		return true;
	}

	return false;
}

std::string ServerSocket::stripNick(const std::string& prefix) const
{
	std::string::size_type i = prefix.find_first_of("!@");

	if(i != std::string::npos)
	{
		return prefix.substr(0, i);
	}

	return std::string();
}

std::string ServerSocket::stripHost(const std::string& prefix) const
{
	std::string::size_type i = prefix.rfind("@");

	if(i != std::string::npos)
	{
		return prefix.substr(i+1);
	}

	return std::string();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// outgoing commands
void ServerSocket::joinChannel(const std::wstring& channel, const std::wstring& pass)
{
	Buffer& buf = getSendBuffer();
	buf << "JOIN " << channel;

	if(pass.empty() == false)
	{
		buf << " :" << pass;
	}
}

void ServerSocket::partChannel(const std::wstring& channel, const std::wstring& reason /*= L""*/)
{
	Buffer& buf = getSendBuffer();
	buf << "PART " << channel;

	if(reason.empty() == false)
	{
		buf << " :" << reason;
	}
}

void ServerSocket::changeNick(const std::wstring& newNick)
{
	Buffer& buf = getSendBuffer();
	buf << "NICK :" << newNick;
}

void ServerSocket::sendMessage(const std::wstring& channel, const std::wstring& message, bool meCmd /*= false*/)
{
	StringVectorW lines = Utils::split(message, L"\r\n");

	for(StringVectorW::const_iterator i = lines.begin(); i != lines.end(); ++i)
	{
		Buffer& buf = getSendBuffer();

		if(meCmd)
		{
			buf << "PRIVMSG " << channel << " :\x01" << "ACTION " << *i << "\x01";
		}
		else
		{
			buf << "PRIVMSG " << channel << " :" << *i;
		}
	}
}

void ServerSocket::ctcpRequest(const std::wstring& to, const std::wstring& request)
{
	Buffer& buf = getSendBuffer();
	buf << "PRIVMSG " << to << " :\x01" << request << "\x01";
}

void ServerSocket::sendNotice(const std::wstring& to, const std::wstring& message, const std::wstring ctcp /*= L""*/)
{
	Buffer& buf = getSendBuffer();

	if(ctcp.empty() == false)
	{
		buf << "NOTICE " << to << " :\x01" << ctcp << " " << message << "\x01";
	}
	else
	{
		buf << "NOTICE " << to << " :" << message;
	}
}

void ServerSocket::setTopic(const std::wstring& channel, const std::wstring& topic)
{
	Buffer& buf = getSendBuffer();
	buf << "TOPIC :" << topic;
}

void ServerSocket::setAway(const std::wstring& message /*= L"be right back"*/)
{
	Buffer& buf = getSendBuffer();
	buf << "AWAY";

	if(message.empty() == false)
	{
		buf << " :" << message;
	}
}

/**************************************************************************************************************/
/* SCRIPTING API																							  */
/**************************************************************************************************************/

lua_State* ServerSocket::luaSTATE = 0;

const luaL_Reg ServerSocket::protoIRC_lua_api[] =
{
	{"logConsole", ServerSocket::api_LogConsole},
	{"logTab", ServerSocket::api_LogTab},
	{"parse", ServerSocket::api_Parse},
	{"send", ServerSocket::api_Send},
	{"profileDir", ServerSocket::api_ProfileDir},
	{0, 0}
};

void ServerSocket::initLua()
{
	if(luaSTATE == 0 && (luaSTATE = luaL_newstate()) != 0)
	{
		luaL_openlibs(luaSTATE);
		lua_newtable(luaSTATE);
		luaL_setfuncs(luaSTATE, protoIRC_lua_api, 0);
		lua_setglobal(luaSTATE, "api");

		std::wstring path = core->getProfileDir();
		path += L"protoIRC_lua\\startup.lua";

		if(luaL_loadfile(luaSTATE, utils::text::fromWide(path).c_str()) == LUA_OK)
		{
			if(lua_pcall(luaSTATE, 0, LUA_MULTRET, 0) != 0)
			{
				core->logFormatted(WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"startup.lua: %s", utils::text::toWide(lua_tostring(luaSTATE, -1)).c_str());
			}
			else
			{
				core->log(PROTO_CLASS, L"Loaded startup Lua script", WTW_LOG_LEVEL_INFO);
			}
		}
		else
		{
			core->log(PROTO_CLASS, L"Failed to load lua script!", WTW_LOG_LEVEL_ERROR);
			closeLua();
		}
	}
}

void ServerSocket::closeLua()
{
	if(luaSTATE != 0)
	{
		lua_close(luaSTATE);
		luaSTATE = 0;
	}
}

int ServerSocket::api_LogConsole(lua_State* L)
{
	int top = lua_gettop(L);

	if(top == 1)
	{
		const char* msg = lua_tostring(L, 1);

		if(msg != 0)
		{
			core->log(PROTO_CLASS, utils::text::toWide(msg).c_str(), 1);
		}
	}

	return 0;
}

int ServerSocket::api_LogTab(lua_State* L)
{
	int top = lua_gettop(L);

	if(top == 2 && lua_isstring(L, 2) && lua_islightuserdata(L, 1))
	{
		const char* msg = lua_tostring(L, 2);
		ServerSocket* ss = reinterpret_cast<ServerSocket*>(lua_touserdata(L, 1));

		if(ss != 0 && msg != 0)
		{
			ss->irc_onServerLog(utils::text::toWide(msg));
		}
	}

	return 0;
}

int ServerSocket::api_Parse(lua_State* L)
{
	int top = lua_gettop(L);

	if(top == 2 && lua_isstring(L, 2) && lua_islightuserdata(L, 1))
	{
		const char* msg = lua_tostring(L, 2);
		ServerSocket* ss = reinterpret_cast<ServerSocket*>(lua_touserdata(L, 1));

		if(ss != 0 && msg != 0)
		{
			ss->onParseBuffer(msg, strlen(msg), false);
		}
	}

	return 0;
}

int ServerSocket::api_Send(lua_State* L)
{
	int top = lua_gettop(L);

	if(top == 2 && lua_isstring(L, 2) && lua_islightuserdata(L, 1))
	{
		const char* msg = lua_tostring(L, 2);
		ServerSocket* ss = reinterpret_cast<ServerSocket*>(lua_touserdata(L, 1));

		if(ss != 0 && msg != 0)
		{
			ss->getSendBuffer(false) << msg;
		}
	}

	return 0;
}

int ServerSocket::api_ProfileDir(lua_State* L)
{
	int top = lua_gettop(L);

	std::wstring path = core->getProfileDir();

	if(top > 0)
	{
		for(int i = 1; i <= top; ++i)
		{
			if(lua_isstring(L, i))
			{
				path += utils::text::toWide(lua_tostring(L, i));
			}
		}

		lua_pop(L, top);
	}

	lua_pushstring(L, utils::text::fromWide(path).c_str());
	return 1;
}

int ServerSocket::lua_onOutgoingMessage(const std::string& line)
{
	if(luaSTATE != 0)
	{
		lua_getglobal(luaSTATE, "proto_OnOutgoingLine");
		lua_pushlightuserdata(luaSTATE, this);
		lua_pushstring(luaSTATE, line.c_str());

		if(lua_pcall(luaSTATE, 2, 1, 0) != 0)
		{
			core->logFormatted(WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"Lua: %s", utils::text::toWide(lua_tostring(luaSTATE, -1)).c_str());
		}
		else
		{
			int ret = 0;

			if(lua_isnumber(luaSTATE, -1))
			{
				ret = (int)lua_tonumber(luaSTATE, -1);
			}

			lua_pop(luaSTATE, 1);

			return ret;
		}
	}

	return 0;
}

int ServerSocket::lua_onIncomingMessage(const std::string& line)
{
	if(luaSTATE != 0)
	{
		lua_getglobal(luaSTATE, "proto_OnIncomingLine");
		lua_pushlightuserdata(luaSTATE, this);
		lua_pushstring(luaSTATE, line.c_str());

		if(lua_pcall(luaSTATE, 2, 1, 0) != 0)
		{
			core->logFormatted(WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"Lua: %s", utils::text::toWide(lua_tostring(luaSTATE, -1)).c_str());
		}
		else
		{
			int ret = 0;

			if(lua_isnumber(luaSTATE, -1))
			{
				ret = (int)lua_tonumber(luaSTATE, -1);
			}

			lua_pop(luaSTATE, 1);

			return ret;
		}
	}

	return 0;
}

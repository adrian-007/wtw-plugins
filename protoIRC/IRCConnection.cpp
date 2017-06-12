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
#include "IRCConnection.h"
#include "Utils.hpp"

IRCConnection::IRCConnection() {
/*	session = 0;
	notify = false;
	autoreconnect = false;
	failedAttempts = 0;

	runThread();*/
}

void IRCConnection::terminate() {
/*	notify = false;
	stop = true;
	autoreconnect = false;

	disconnect();
	s.signal();
	join();*/
}

void IRCConnection::initIRCConnection() {
/*	ScopedLock mtx(cs);

	irc_callbacks_t cb = { 0 };

	cb.event_connect =		&IRCConnection::evn_onConnect;
	cb.event_join =			&IRCConnection::evn_onJoin;
	cb.event_part =			&IRCConnection::evn_onPart;
	cb.event_quit =			&IRCConnection::evn_onQuit;
	cb.event_channel =		&IRCConnection::evn_onChannel;
	cb.event_topic =		&IRCConnection::evn_onTopic;
	cb.event_nick =			&IRCConnection::evn_onNick;
	cb.event_numeric =		&IRCConnection::evn_onNumeric;
	cb.event_ctcp_rep =		&IRCConnection::evn_onCTCPReply;
	cb.event_ctcp_action =	&IRCConnection::evn_onMeCommand;
	cb.event_kick =			&IRCConnection::evn_onKick;
	cb.event_mode =			&IRCConnection::evn_onMode;
	cb.event_umode =		&IRCConnection::evn_onUmode;
	cb.event_notice	=		&IRCConnection::evn_onNotice;

	this->session = irc_create_session(&cb);
	irc_set_ctx(this->session, static_cast<void*>(this));

	notify = true;
	autoreconnect = true;

	OnConnecting();*/
}

bool IRCConnection::connect(const std::wstring& address, unsigned short port, const std::wstring& user, const std::wstring& nick, const std::wstring& pass) {
/*	ScopedLock mtx(cs);
	if(session)
		return true;

	autoreconnect = true;

	ci.address = utils::text::fromWide(address);
	ci.nick = utils::text::fromWide(nick);
	ci.pass = utils::text::fromWide(pass);
	ci.user = utils::text::fromWide(user);
	ci.port = port;

	initIRCConnection();

	s.signal();*/

	return true;
}

void IRCConnection::disconnect(const std::wstring& reason /*= L""*/) {
/*	ScopedLock mtx(cs);
	autoreconnect = false;

	if(session) {
		std::string r;
		if(r.empty() == false) {
			r = utils::text::fromWide(reason);
		} else {
			r = utils::text::fromWide(quitMessage);
		}
		irc_cmd_quit(session, r.empty() ? 0 : r.c_str());
		// when we receive our quit message, we'll disconnect
	}*/
}

void IRCConnection::joinChannel(const std::wstring& channel, const std::wstring& pass) {
/*	ScopedLock mtx(cs);
	if(session) {
		std::string ch = utils::text::fromWide(channel);
		std::string p = utils::text::fromWide(pass);
		irc_cmd_join(session, ch.c_str(), p.empty() ? 0 : p.c_str());
	}*/
}

void IRCConnection::partChannel(const std::wstring& channel, const std::wstring& reason /*= L""*/) {
/*	ScopedLock mtx(cs);
	if(session) {
		std::string ch = utils::text::fromWide(channel);
		std::string r = utils::text::fromWide(reason);
		irc_cmd_part(session, ch.c_str(), r.empty() ? 0 : r.c_str());
	}*/
}

void IRCConnection::sendMessage(const std::wstring& channel, const std::wstring& message, bool meCmd /*= false*/) {
/*	ScopedLock mtx(cs);
	if(callMessageHooks(channel, message, false) == false)
		return;

	if(session) {
		std::vector<std::wstring> v = Utils::split(message, L"\r\n");
		for(std::vector<std::wstring>::const_iterator i = v.begin(); i != v.end(); ++i) {
			std::string ch = utils::text::fromWide(channel);
			//std::string m = utils::text::fromWide(*i);
			char* m = irc_color_convert_to_mirc(utils::text::fromWide(*i).c_str());
			if(meCmd) {
				irc_cmd_me(session, ch.c_str(), m);
			} else {
				irc_cmd_msg(session, ch.c_str(), m);
			}
			free(m);
		}
	}*/
}

void IRCConnection::setAway(bool away, const std::wstring& message /*= L"be right back"*/) {
/*	ScopedLock mtx(cs);
	if(session) {
		if(away) {
			std::string m = utils::text::fromWide(message);
			if(m.empty())
				m = "be right back";
			irc_send_raw(session, "AWAY :%s", m.c_str());
		} else {
			irc_send_raw(session, "AWAY");
		}
	}*/
}

void IRCConnection::changeNick(const std::wstring& newNick) {
/*	ScopedLock mtx(cs);
	if(session) {
		dropOnBadNick = false;
		std::string n = utils::text::fromWide(newNick);
		irc_cmd_nick(session, n.c_str());
	}*/
}

void IRCConnection::sendCTCPRequest(const std::wstring& to, const std::wstring& request) {
/*	ScopedLock mtx(cs);
	if(session) {
		std::string n = utils::text::fromWide(to);
		std::string r = utils::text::fromWide(request);
		irc_cmd_ctcp_request(session, n.c_str(), r.c_str());
	}*/
}

void IRCConnection::setTopic(const std::wstring& channel, const std::wstring& topic) {
/*	ScopedLock mtx(cs);
	if(session) {
		std::string ch = utils::text::fromWide(channel);
		std::string t = utils::text::fromWide(topic);
		irc_cmd_topic(session, ch.c_str(), t.c_str());
	}*/
}

void IRCConnection::requestTopic(const std::wstring& channel) {
/*	ScopedLock mtx(cs);
	if(session) {
		std::string ch = utils::text::fromWide(channel);
		irc_cmd_topic(session, ch.c_str(), 0);
	}*/
}

void IRCConnection::kickUser(const std::wstring& channel, const std::wstring& victim, const std::wstring& reason /*= L""*/) {
/*	ScopedLock mtx(cs);
	if(session) {
		std::string ch = utils::text::fromWide(channel);
		std::string v = utils::text::fromWide(victim);
		std::string r = utils::text::fromWide(reason);

		irc_cmd_kick(session, v.c_str(), ch.c_str(), r.empty() ? 0 : r.c_str());
	}*/
}

void IRCConnection::setMode(const std::wstring& channel, const std::wstring& user, const std::wstring& mode) {
/*	ScopedLock mtx(cs);
	if(session) {
		std::string ch = utils::text::fromWide(channel);
		std::string u = utils::text::fromWide(user);
		std::string m = utils::text::fromWide(mode);

		irc_send_raw(session, "MODE %s %s %s", ch.c_str(), m.c_str(), u.c_str());
	}*/
}

void IRCConnection::threadMain() {
/*	stop = false;
	while(true) {
		if(session == 0) {
			s.wait();
		} else {
			int ret = -1;
			dropOnBadNick = true;

			{
				bool found;

				if(dns.getIpCount() == 0) {
					found = dns.setDomain(ci.address);
				} else {
					found = true;
				}

				if(found) {
					__LOG_F(core, WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Found %d IPs for %s.", dns.getIpCount(), utils::text::toWide(ci.address).c_str());

					std::string ip;
					while(dns.getIpCount() > 0) {
						ip = dns.getFirstIp();
						__LOG_F(core, WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Connecting to %s...", utils::text::toWide(ip).c_str());

						ret = irc_connect(session, ip.c_str(), ci.port, (ci.pass.empty() == false && ci.user.empty() == false) ? ci.pass.c_str() : 0, ci.nick.c_str(), (ci.user.empty() ? 0 : ci.user.c_str()), "WTW2");

						if(ret == 0) {
							__LOG_F(core, WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"Connected to %s!", utils::text::toWide(ip).c_str());
							failedAttempts = 0;
							break;
						} else {
							const char* err = irc_strerror(irc_errno(session));
							__LOG_F(core, WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"%s - %s", utils::text::toWide(ip).c_str(), utils::text::toWide(err).c_str());
							dns.popFirstIp();
							failedAttempts++;
						}
					}
				} else {
					__LOG_F(core, WTW_LOG_LEVEL_ERROR, PROTO_CLASS, L"Can't resolve %s!", utils::text::toWide(ci.address).c_str());
					failedAttempts++;
				}
			}

			if(ret == 0) {
				irc_run(session);
			}

			irc_destroy_session(session);
			session = 0;

			if(notify) {
				OnDisconnected();
				__LOG_F(core, WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"%s - session closed", utils::text::toWide(ci.address).c_str());
			}
			notify = true;
		}

		if(stop) {
			break;
		} else if(autoreconnect) {
			DWORD delay = 3;
			if(failedAttempts > 10)
				delay *= 10;

			__LOG_F(core, WTW_LOG_LEVEL_INFO, PROTO_CLASS, L"	Connection error, reconnecting in %ds...", delay);
			s.wait(1000 * delay);
			if(stop == false) {
				// we could exit while sleeping
				ScopedLock mtx(cs);
				initIRCConnection();
			} else {
				break;
			}
		}
	}*/
}

void IRCConnection::evn_onConnect(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	ScopedLock mtx(conn->cs);
	conn->OnConnected();*/
}

void IRCConnection::evn_onJoin(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	if(count != 1) return;
	if(!origin) return;

	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring n = utils::text::toWide(nickbuf);
	std::wstring ch = utils::text::toWide(params[0]);

	ScopedLock mtx(conn->cs);
	conn->OnUserJoin(ch, n);*/
}

void IRCConnection::evn_onPart(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	if(!origin)
		return;

	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring n = utils::text::toWide(nickbuf);
	std::wstring ch = utils::text::toWide(params[0]);
	std::wstring r;
	if(count == 2)
		r = utils::text::toWide(params[1]);

	ScopedLock mtx(conn->cs);
	conn->OnUserPart(ch, n, r);*/
}

void IRCConnection::evn_onQuit(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring n = utils::text::toWide(nickbuf);

	if(conn->NickCompare(n) == false) {
		std::wstring r;
		if(count > 0)
			r = utils::text::toWide(params[0]);
		ScopedLock mtx(conn->cs);
		conn->OnQuit(n, r);
	} else {
		irc_disconnect(conn->session);
	}*/
}

void IRCConnection::evn_onChannel(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	if(count != 2)
		return;
	if(!origin)
		return;

	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));

	std::wstring f = utils::text::toWide(nickbuf);
	std::wstring ch = utils::text::toWide(params[0]);

	char* mf = irc_color_convert_from_mirc(params[1]);
	std::wstring m = utils::text::toWide(mf);
	free(mf);

	ScopedLock mtx(conn->cs);
	if(conn->callMessageHooks(ch, m, true) == false)
		return;
	conn->OnIncomingMessage(ch, f, m);*/
}

void IRCConnection::evn_onTopic(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	if(!origin)
		return;

	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));

	std::wstring who = utils::text::toWide(nickbuf);
	std::wstring ch = utils::text::toWide(params[0]);
	std::wstring topic;

	if(count == 2)
		topic = utils::text::toWide(params[1]);

	ScopedLock mtx(conn->cs);
	conn->OnTopic(who, ch, topic);*/
}

void IRCConnection::evn_onNick(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	if(count != 1)
		return;
	if(!origin)
		return;

	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));

	std::wstring oldNick = utils::text::toWide(nickbuf);
	std::wstring newNick = utils::text::toWide(params[0]);

	ScopedLock mtx(conn->cs);
	conn->OnNick(oldNick, newNick);*/
}

void IRCConnection::evn_onNumeric(irc_session_t* s, unsigned int event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	ScopedLock mtx(conn->cs);

	switch(event) {
		case LIBIRC_RFC_RPL_NAMREPLY: 
			{
				std::wstring ch = utils::text::toWide(params[count-2]);
				std::vector<std::wstring> list = Utils::split(utils::text::toWide(params[count-1]));
				conn->OnNickList(ch, list);
				break;
			}
		case LIBIRC_RFC_RPL_AWAY: 
			{
				/*std::wstring org = utils::text::toWide(origin);
				std::wstring param;
				for(size_t i = 0; i < count; ++i)
				param += utils::text::toWide(params[i]) + L" ";

				__LOG_F(core, 1, L"IRCP", L"AWAY = %s, params = %s", org.c_str(), param.c_str());/
				break;
			}
		case LIBIRC_RFC_RPL_UNAWAY: 
			{
				conn->OnAway(0, false);
				break;
			}
		case LIBIRC_RFC_RPL_NOWAWAY: 
			{
				conn->OnAway(0, true);
				break;
			}
		case LIBIRC_RFC_ERR_NICKNAMEINUSE:
			{
				Utils::showNotify(L"IRC - b��d", L"Ten nick jest juz zaj�ty!");
				if(conn->dropOnBadNick)
					conn->disconnect();
				break;
			}
		case LIBIRC_RFC_RPL_TOPIC:
			{
				if(count >= 3)
					conn->OnTopic(L""/*utils::text::toWide(params[0])/, utils::text::toWide(params[1]), utils::text::toWide(params[2]));
				break;
			}
		case LIBIRC_RFC_RPL_NOTOPIC: 
			{
				conn->OnTopic(L"", utils::text::toWide(params[1]), L"");
				break;
			}
		case LIBIRC_RFC_ERR_CHANOPRIVSNEEDED:
			{
				if(count > 2)
					conn->OnError(utils::text::toWide(params[1]), utils::text::toWide(params[2]));
				break;
			}
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case LIBIRC_RFC_RPL_LUSERCLIENT:
		case LIBIRC_RFC_RPL_LUSEROP:
		case LIBIRC_RFC_RPL_LUSERUNKNOWN:
		case LIBIRC_RFC_RPL_LUSERCHANNELS:
		case LIBIRC_RFC_RPL_MOTDSTART:
		case LIBIRC_RFC_RPL_MOTD:
		case LIBIRC_RFC_RPL_ENDOFMOTD:
			break;

		default: {
			/*std::string p = origin;
			p += " => ";
			for(unsigned int i = 0; i < count; ++i) {
			p += params[i];
			p += "|";
			}
			std::wstring wp = utils::text::toWide(p);
			__LOG_F(core, 1, L"IRC_CMD", L"Unhandled NUMREPLY: %d. Params (%d): %s", event, count, wp.c_str());/
				 }
	}*/
}

void IRCConnection::evn_onCTCPReply(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	if(!origin)
		return;
	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring n = utils::text::toWide(nickbuf);
	std::wstring m = utils::text::toWide(params[0]);

	ScopedLock mtx(conn->cs);
	conn->OnCTCPReply(n, m);*/
}

void IRCConnection::evn_onMeCommand(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	if(!origin)
		return;

	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring n = utils::text::toWide(nickbuf);
	std::wstring ch = utils::text::toWide(params[0]);
	std::wstring m = utils::text::toWide(params[1]);

	ScopedLock mtx(conn->cs);
	conn->OnActionCommand(ch, n, m);*/
}

void IRCConnection::evn_onKick(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	if(!origin)
		return;

	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring victim = utils::text::toWide(nickbuf);
	std::wstring channel = utils::text::toWide(params[0]);
	std::wstring who;
	std::wstring reason;

	if(count > 1)
		who = utils::text::toWide(params[1]);
	if(count > 2)
		reason = utils::text::toWide(params[2]);

	if(reason == victim)
		reason.clear();

	ScopedLock mtx(conn->cs);
	conn->OnKick(channel, victim, who, reason);*/
}

void IRCConnection::evn_onMode(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	if(!origin)
		return;

	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring who = utils::text::toWide(nickbuf);
	std::wstring ch = utils::text::toWide(params[0]);
	std::string m = params[1];

	std::vector<std::string> nicks, mode;
	for(unsigned int i = 2; i < count; ++i) {
		irc_target_get_nick(params[i], nickbuf, sizeof(nickbuf));

		nicks.push_back(nickbuf);

		std::string t;
		t.resize(2);
		t[0] = m[0];
		t[1] = m[i-1];

		mode.push_back(t);
	}

	ScopedLock mtx(conn->cs);
	for(size_t i = 0; i < nicks.size(); ++i) {
		conn->OnMode(ch, who, utils::text::toWide(nicks[i]), utils::text::toWide(mode[i]));
	}*/
}

void IRCConnection::evn_onUmode(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
	if(!origin)
		return;

	/*char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));

	std::string p = nickbuf;
	p += " => ";
	for(unsigned int i = 0; i < count; ++i) {
	p += params[i];
	p += "|";
	}
	std::wstring wp = utils::text::toWide(p);
	__LOG_F(core, 1, L"IRC_CMD", L"UMODE %s", wp.c_str());*/
}

void IRCConnection::evn_onNotice(irc_session_t* s, const char* event, const char* origin, const char** params, unsigned int count) {
/*	IRCConnection* conn = reinterpret_cast<IRCConnection*>(irc_get_ctx(s));
	if(!origin)
		return;
	char nickbuf[128];
	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
	std::wstring n = utils::text::toWide(nickbuf);
	std::wstring c = utils::text::toWide(params[0]);
	std::wstring m;
	if(count > 1) {
		m = utils::text::toWide(params[1]);
	}

	ScopedLock mtx(conn->cs);
	conn->OnNotice(c, n, m);*/
}

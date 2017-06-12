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

#ifndef WTW_SOUNDS_EVENT_DISPATCHER
#define WTW_SOUNDS_EVENT_DISPATCHER

#ifdef WTW_PARAM
#include <utlIgnore.h>
#endif

template<typename T1>
class EventDispatcher {
public:
	enum  //21
	{
		EVENT_MESSAGE_ERROR = 0,
		EVENT_MESSAGE_WARNING,
		EVENT_MESSAGE_INFO,
		EVENT_MESSAGE_CHAT_NEW,
		EVENT_MESSAGE_CHAT,
		EVENT_MESSAGE_CHAT_INACTIVE,
		EVENT_MESSAGE_CONFERENCE_NEW,
		EVENT_MESSAGE_CONFERENCE,
		EVENT_MESSAGE_CONFERENCE_INACTIVE,
		EVENT_AV_AUDIO_CALLING_IN,
		EVENT_AV_AUDIO_CALLING_OUT,
		EVENT_AV_AUDIO_REJECTED,
		EVENT_AV_AUDIO_END,
		EVENT_AV_VIDEO_CALLING_IN,
		EVENT_AV_VIDEO_CALLING_OUT,
		EVENT_AV_VIDEO_REJECTED,
		EVENT_AV_VIDEO_END,
		EVENT_PRESENCE_ONLINE,
		EVENT_PRESENCE_CHAT,
		EVENT_PRESENCE_DND,
		EVENT_PRESENCE_AWAY,
		EVENT_PRESENCE_XA,
		EVENT_PRESENCE_INVISIBLE, // sporny ficzer...
		EVENT_PRESENCE_OFFLINE,
		EVENT_PRESENCE_ERROR,
		EVENT_PRESENCE_BLOCKED,
		EVENT_MISC_NETWORK_LOGGED_IN,
		EVENT_MISC_NETWORK_LOGGED_OFF,
		EVENT_MISC_AUTH_REQUEST,
		EVENT_MISC_NEW_MAIL,
		EVENT_MISC_ALERT,
		EVENT_MISC_INCOMING_FILE,
		EVENT_LAST
	};

	enum
	{
		EVENT_TYPE_MESSAGE = 0,
		EVENT_TYPE_PRESENCE,
		EVENT_TYPE_MISC
	};

	static int getGroupIndex(int event)
	{
		if(event >= EVENT_MESSAGE_ERROR && event <= EVENT_MESSAGE_CONFERENCE_INACTIVE)
		{
			return 0;
		}

		if(event >= EVENT_PRESENCE_ONLINE && event <= EVENT_PRESENCE_BLOCKED)
		{
			return 1;
		}

		if(event >= EVENT_AV_AUDIO_CALLING_IN && event <= EVENT_AV_VIDEO_END)
		{
			return 2;
		}

		if(event >= EVENT_MISC_NETWORK_LOGGED_IN && event < EVENT_LAST)
		{
			return 3;
		}
		return -1;
	}

	static std::wstring getEventName(int type) 
	{
		switch(type) 
		{
		case EVENT_MESSAGE_ERROR:				return L"B��d";
		case EVENT_MESSAGE_WARNING:				return L"Ostrze�enie";
		case EVENT_MESSAGE_INFO:				return L"Informacja";
		case EVENT_MESSAGE_CHAT_NEW:			return L"Rozmowa (rozpocz�cie)";
		case EVENT_MESSAGE_CHAT:				return L"Rozmowa (okno aktywne)";
		case EVENT_MESSAGE_CHAT_INACTIVE:		return L"Rozmowa (okno nieaktywne)";
		case EVENT_MESSAGE_CONFERENCE_NEW:		return L"Konferencja (rozpocz�cie)";
		case EVENT_MESSAGE_CONFERENCE:			return L"Konferencja (okno aktywne)";
		case EVENT_MESSAGE_CONFERENCE_INACTIVE:	return L"Konferencja (okno nieaktywne)";
		case EVENT_AV_AUDIO_CALLING_IN:			return L"Rozmowa g�osowa przychodz�ca";
		case EVENT_AV_AUDIO_CALLING_OUT:		return L"Rozmowa g�osowa wychodz�ca";
		case EVENT_AV_AUDIO_REJECTED:			return L"Rozmowa g�osowa odrzucona";
		case EVENT_AV_AUDIO_END:				return L"Rozmowa g�osowa zako�czona";
		case EVENT_AV_VIDEO_CALLING_IN:			return L"Rozmowa wideo przychodz�ca";
		case EVENT_AV_VIDEO_CALLING_OUT:		return L"Rozmowa wideo wychodz�ca";
		case EVENT_AV_VIDEO_REJECTED:			return L"Rozmowa wideo odrzucona";
		case EVENT_AV_VIDEO_END:				return L"Rozmowa wideo zako�czona";
		case EVENT_PRESENCE_ONLINE:				return L"Dost�pny";
		case EVENT_PRESENCE_CHAT:				return L"Porozmawiajmy";
		case EVENT_PRESENCE_DND:				return L"Jestem zaj�ty";
		case EVENT_PRESENCE_AWAY:				return L"Zaraz wracam";
		case EVENT_PRESENCE_XA:					return L"Wr�c� p�niej";
		case EVENT_PRESENCE_INVISIBLE:			return L"Niewidoczny";
		case EVENT_PRESENCE_OFFLINE:			return L"Niedost�pny";
		case EVENT_PRESENCE_ERROR:				return L"B��d";
		case EVENT_PRESENCE_BLOCKED:			return L"Zablokowany";
		case EVENT_MISC_NETWORK_LOGGED_IN:		return L"Zalogowany do sieci";
		case EVENT_MISC_NETWORK_LOGGED_OFF:		return L"Wylogowany z sieci";
		case EVENT_MISC_AUTH_REQUEST:			return L"��danie autoryzacji";
		case EVENT_MISC_NEW_MAIL:				return L"Nowa poczta";
		case EVENT_MISC_ALERT:					return L"Alert";
		case EVENT_MISC_INCOMING_FILE:			return L"Transfer pliku (przychodz�cy)";
		default: return std::wstring();
		}
		return std::wstring();
	}

	static int getEventFromName(const wchar_t* name) 
	{
		int type = -1;
#define FIND_ITEM(x, y) if(wcscmp(name, x) == 0) type = y
		FIND_ITEM(L"message.error",					EVENT_MESSAGE_ERROR);
		FIND_ITEM(L"message.warning",				EVENT_MESSAGE_WARNING);
		FIND_ITEM(L"message.info",					EVENT_MESSAGE_INFO);
		FIND_ITEM(L"message.chat.new",				EVENT_MESSAGE_CHAT_NEW);
		FIND_ITEM(L"message.chat",					EVENT_MESSAGE_CHAT);
		FIND_ITEM(L"message.chat.inactive",			EVENT_MESSAGE_CHAT_INACTIVE);
		FIND_ITEM(L"message.conference.new",		EVENT_MESSAGE_CONFERENCE_NEW);
		FIND_ITEM(L"message.conference",			EVENT_MESSAGE_CONFERENCE);
		FIND_ITEM(L"message.conference.inactive",	EVENT_MESSAGE_CONFERENCE_INACTIVE);
		FIND_ITEM(L"presence.online",				EVENT_PRESENCE_ONLINE);
		FIND_ITEM(L"presence.chat",					EVENT_PRESENCE_CHAT);
		FIND_ITEM(L"presence.dnd",					EVENT_PRESENCE_DND);
		FIND_ITEM(L"presence.away",					EVENT_PRESENCE_AWAY);
		FIND_ITEM(L"presence.xa",					EVENT_PRESENCE_XA);
		FIND_ITEM(L"presence.invisible",			EVENT_PRESENCE_INVISIBLE);
		FIND_ITEM(L"presence.error",				EVENT_PRESENCE_ERROR);
		FIND_ITEM(L"presence.blocked",				EVENT_PRESENCE_BLOCKED);
		FIND_ITEM(L"av.audio.in",					EVENT_AV_AUDIO_CALLING_IN);
		FIND_ITEM(L"av.audio.out",					EVENT_AV_AUDIO_CALLING_OUT);
		FIND_ITEM(L"av.audio.rejected",				EVENT_AV_AUDIO_REJECTED);
		FIND_ITEM(L"av.audio.end",					EVENT_AV_AUDIO_END);
		FIND_ITEM(L"av.video.in",					EVENT_AV_VIDEO_CALLING_IN);
		FIND_ITEM(L"av.video.out",					EVENT_AV_VIDEO_CALLING_OUT);
		FIND_ITEM(L"av.video.rejected",				EVENT_AV_VIDEO_REJECTED);
		FIND_ITEM(L"av.video.end",					EVENT_AV_VIDEO_END);
		FIND_ITEM(L"misc.network.loggedIn",			EVENT_MISC_NETWORK_LOGGED_IN);
		FIND_ITEM(L"misc.network.loggedOff",		EVENT_MISC_NETWORK_LOGGED_OFF);
		FIND_ITEM(L"misc.auth.request",				EVENT_MISC_AUTH_REQUEST);
		FIND_ITEM(L"misc.newMail",					EVENT_MISC_NEW_MAIL);
		FIND_ITEM(L"misc.alert",					EVENT_MISC_ALERT);
		FIND_ITEM(L"misc.fileTransfer.in",			EVENT_MISC_INCOMING_FILE);
#undef FIND_ITEM

		if(type >= EVENT_LAST)
		{
			type = -1;
		}

		return type;
	}

	static std::wstring getEventSettingName(int type)
	{
		switch(type)
		{
		case EVENT_MESSAGE_ERROR:				return L"message.error";
		case EVENT_MESSAGE_WARNING:				return L"message.warning";
		case EVENT_MESSAGE_INFO:				return L"message.info";
		case EVENT_MESSAGE_CHAT:				return L"message.chat";
		case EVENT_MESSAGE_CHAT_NEW:			return L"message.chat.new";
		case EVENT_MESSAGE_CHAT_INACTIVE:		return L"message.chat.inactive";
		case EVENT_MESSAGE_CONFERENCE:			return L"message.conference";
		case EVENT_MESSAGE_CONFERENCE_NEW:		return L"message.conference.new";
		case EVENT_MESSAGE_CONFERENCE_INACTIVE: return L"message.conference.inactive";
		case EVENT_PRESENCE_ONLINE:				return L"presence.online";
		case EVENT_PRESENCE_CHAT:				return L"presence.chat";
		case EVENT_PRESENCE_DND:				return L"presence.dnd";
		case EVENT_PRESENCE_AWAY:				return L"presence.away";
		case EVENT_PRESENCE_XA:					return L"presence.xa";
		case EVENT_PRESENCE_INVISIBLE:			return L"presence.invisible";
		case EVENT_PRESENCE_OFFLINE:			return L"presence.offline";
		case EVENT_PRESENCE_ERROR:				return L"presence.error";
		case EVENT_PRESENCE_BLOCKED:			return L"presence.blocked";
		case EVENT_AV_AUDIO_CALLING_IN:			return L"av.audio.in";
		case EVENT_AV_AUDIO_CALLING_OUT:		return L"av.audio.out";
		case EVENT_AV_AUDIO_REJECTED:			return L"av.audio.rejected";
		case EVENT_AV_AUDIO_END:				return L"av.audio.end";
		case EVENT_AV_VIDEO_CALLING_IN:			return L"av.video.in";
		case EVENT_AV_VIDEO_CALLING_OUT:		return L"av.video.out";
		case EVENT_AV_VIDEO_REJECTED:			return L"av.video.rejected";
		case EVENT_AV_VIDEO_END:				return L"av.video.end";
		case EVENT_MISC_NETWORK_LOGGED_IN:		return L"misc.network.loggedIn";
		case EVENT_MISC_NETWORK_LOGGED_OFF:		return L"misc.network.loggedOff";
		case EVENT_MISC_AUTH_REQUEST:			return L"misc.auth.request";
		case EVENT_MISC_NEW_MAIL:				return L"misc.newMail";
		case EVENT_MISC_ALERT:					return L"misc.alert";
		case EVENT_MISC_INCOMING_FILE:			return L"misc.fileTransfer.in";
		default: return std::wstring();
		}
		return std::wstring();
	}

#ifdef WTW_PARAM
	static int getStatusFromList(wtwContactDef* cnt) 
	{
		wtwPresenceDef p;
		initStruct(p);
		p.pContactData = cnt;

		wtw->fnCall(WTW_CTL_GET_PRESENCE, (WTW_PARAM)&p, 0);
		return p.curStatus;
	}

	static bool isChatWindowState(wtwContactDef* cnt, bool active)
	{
		wtwChatWindowAttributes wa;
		wa.pContactData = cnt;

		WTW_PTR ret = S_OK;

		if(wtw->fnExists(WTW_FUNCT_GET_CHATWND_ATTRIBUTES))
		{
			if(active)
			{
				wa.flags = WTW_WA_FLAG_IS_WINDOW_ACTIVE;
			}
			else
			{
				wa.flags = WTW_WA_FLAG_IS_VISIBLE_OR_IN_TABS | WTW_WA_FLAG_IS_WINDOW_VISIBLE;
			}

			ret = wtw->fnCall(WTW_FUNCT_GET_CHATWND_ATTRIBUTES, wa, 0);
		}
		else
		{
			if(active)
			{
				if(wtw->fnCall(WTW_FUNCT_IS_CONTACT_CHAT_ACTIVE, (WTW_PARAM)cnt, 0) == 0)
				{
					ret = S_FALSE;
				}
			}
		}

		if(ret != S_OK)
		{
			return false;
		}

		return true;
	}

	static bool isContactIgnored(wtwContactDef* cnt, int type = 0)
	{
		if(wtw->fnExists(IGNORE_IS_IGNORED))
		{
			utlIgnoreEntry ie;
			ie.pContacts = cnt;
			ie.iContacts = 1;

			switch(type)
			{
			case EVENT_TYPE_MESSAGE:
				{
					DWORD tmp;

					ie.flags |= IGNORE_FLAG_CHECK_USER | IGNORE_FLAG_CHECK_MESSAGES;
					ie.userLockName = L"sounds/ignoreMessage";
					ie.rInfo = &tmp;

					if(wtw->fnCall(IGNORE_IS_IGNORED, ie, 0) == S_OK)
					{
						if(ie.rInfo[0] & (IGNORE_FLAG_CHECK_USER | IGNORE_FLAG_CHECK_MESSAGES))
						{
							return true;
						}
					}
					break;
				}
			case EVENT_TYPE_PRESENCE:
				{
					DWORD tmp;

					ie.flags |= IGNORE_FLAG_CHECK_USER | IGNORE_FLAG_CHECK_PRESENCE | IGNORE_FLAG_CHECK_WHATIFY;
					ie.userLockName = L"sounds/ignorePresence";
					ie.rInfo = &tmp;
					if(wtw->fnCall(IGNORE_IS_IGNORED, ie, 0) == S_OK)
					{
						if(ie.rInfo[0] & (IGNORE_FLAG_CHECK_USER | IGNORE_FLAG_CHECK_PRESENCE))
						{
							return true;
						}
					}
					break;
				}
			case EVENT_TYPE_MISC:
				{
					break;
				}
			}
		}

		return false;
	}

	static WTW_PTR dispatchProtocol(WTW_PARAM lParam, WTW_PARAM wParam, void* data) 
	{
		T1* parent = reinterpret_cast<T1*>((WTW_PTR)data);

		if(!parent)
		{
			return 0;
		}

		wtwProtocolEvent* evn = (wtwProtocolEvent*)lParam;

		if(!evn || evn->type == WTW_PEV_TYPE_BEFORE)
		{
			return 0;
		}

		bool activeOnNet = true;
		{
			wtwProtocolInfo pi;
			initStruct(pi);
			pi.netClass = evn->netClass;
			pi.netId = evn->netId;

			wtwBuffer buf;
			buf.bufferSize = 1024;
			buf.pBuffer = new wchar_t[buf.bufferSize];
			memset(buf.pBuffer, 0, sizeof(wchar_t)* buf.bufferSize);

			if (wtw->fnCall(WTW_PROTO_FUNC_GET_GUID_EX, pi, buf) == S_OK)
			{
				std::wstring nets;
				cfgGetStr(nets, L"sounds.networks");
			
				if(nets.find(buf.pBuffer) == std::wstring::npos)
				{
					activeOnNet = false;
				}
			}

			delete[] buf.pBuffer;
		}

		switch(evn->event) 
		{
		case WTW_PEV_MESSAGE_RECV: 
			{
				wtwMessageDef* msg = (wtwMessageDef*)wParam;

				if(msg == 0 || msg->contactData.id == 0)
				{
					return 0;
				}

				if((msg->msgFlags & WTW_MESSAGE_FLAG_INCOMING) == 0)
				{
					return 0;
				}

				if(isContactIgnored(&msg->contactData, EVENT_TYPE_MESSAGE))
				{
					return 0;
				}

				if(msg->msgFlags & WTW_MESSAGE_FLAG_ERROR)
				{
					parent->onMessage(EVENT_MESSAGE_ERROR, activeOnNet);
				}

				if(msg->msgFlags & WTW_MESSAGE_FLAG_WARNING) 
				{
					parent->onMessage(EVENT_MESSAGE_WARNING, activeOnNet);
				}

				if(msg->msgFlags & WTW_MESSAGE_FLAG_INFO)
				{
					parent->onMessage(EVENT_MESSAGE_INFO, activeOnNet);
				}

				if(msg->msgFlags & WTW_MESSAGE_FLAG_CONFERENCE)
				{
					parent->onMessage(EVENT_MESSAGE_CONFERENCE, activeOnNet);
				}

				if(msg->msgFlags & WTW_MESSAGE_FLAG_CHAT_MSG)
				{
					bool isActive = isChatWindowState(&msg->contactData, true);
					bool isVisible = isChatWindowState(&msg->contactData, false);

					if(isVisible == false && isActive == false)
					{
						parent->onMessage(EVENT_MESSAGE_CHAT_NEW, activeOnNet);
					}
					else if(isActive == false)
					{
						parent->onMessage(EVENT_MESSAGE_CHAT_INACTIVE, activeOnNet);
					}
					else
					{
						parent->onMessage(EVENT_MESSAGE_CHAT, activeOnNet);
					}
				}

				break;
			}
		case WTW_PEV_PRESENCE_RECV:
			{
				wtwPresenceDef* prs = (wtwPresenceDef*)wParam;

				if(prs == 0 || prs->pContactData == 0)
				{
					return 0;
				}

				if(prs->flags & WTW_PRESENCE_FLAG_NO_NOTIFY)
				{
					return 0;
				}

				if(isContactIgnored(prs->pContactData, EVENT_TYPE_PRESENCE))
				{
					return 0;
				}

				int oldStatus = EventDispatcher::getStatusFromList(prs->pContactData);
				int status = 0;

				switch(prs->curStatus) 
				{
				case WTW_PRESENCE_ONLINE: 
					{ 
						status = EVENT_PRESENCE_ONLINE; 
						break; 
					}
				case WTW_PRESENCE_CHAT: 
					{
						status = EVENT_PRESENCE_CHAT;
						break;
					}
				case WTW_PRESENCE_DND: 
					{
						status = EVENT_PRESENCE_DND;
						break;
					}
				case WTW_PRESENCE_AWAY: 
					{
						status = EVENT_PRESENCE_AWAY;
						break;
					}
				case WTW_PRESENCE_XA: 
					{
						status = EVENT_PRESENCE_XA;
						break;
					}
				case WTW_PRESENCE_INV: 
					{
						status = EVENT_PRESENCE_INVISIBLE;
						break;
					}
				case WTW_PRESENCE_OFFLINE: 
					{
						status = EVENT_PRESENCE_OFFLINE;
						break;
					}
				case WTW_PRESENCE_ERROR: 
					{
						status = EVENT_PRESENCE_ERROR;
						break;
					}
				case WTW_PRESENCE_BLOCK: 
					{
						status = EVENT_PRESENCE_BLOCKED;
						break;
					}
				default: 
					{
						break;
					}
				}
				parent->onStatus(status, oldStatus, activeOnNet);
				break;
			}
		case WTW_PEV_CALL_RING:
			{
				__LOG(wtw, L"DISP", L"_RING");

				wtwProtocolCall* pPc = (wtwProtocolCall*)wParam;
				bool isValid = true;

				if(pPc != 0)
				{
					if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_ERROR)
					{
						isValid = false;
					}

					if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_VIDEO)
					{
						if(isValid)
						{
							if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_INCOMING)
							{
								parent->onAudioVideo(EVENT_AV_VIDEO_CALLING_IN, activeOnNet);
							}
							else if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_OUTGOING)
							{
								parent->onAudioVideo(EVENT_AV_VIDEO_CALLING_OUT, activeOnNet);
							}
						}
						else
						{
							parent->onAudioVideo(EVENT_AV_VIDEO_END, activeOnNet);
						}
					}
					else if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_AUDIO)
					{
						if(isValid)
						{
							if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_INCOMING)
							{
								parent->onAudioVideo(EVENT_AV_AUDIO_CALLING_IN, activeOnNet);
							}
							else if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_OUTGOING)
							{
								parent->onAudioVideo(EVENT_AV_AUDIO_CALLING_OUT, activeOnNet);
							}
						}
						else
						{
							parent->onAudioVideo(EVENT_AV_AUDIO_END, activeOnNet);
						}
					}
					else
					{
						isValid = false;
					}
				}
				else
				{
					isValid = false;
				}

				break;
			}
		case WTW_PEV_CALL_START:
			{
				__LOG(wtw, L"DISP", L"_START");

				wtwProtocolCall* pPc = (wtwProtocolCall*)wParam;
				if(pPc != 0)
				{
					parent->stopAll();
				}
				break;
			}
		case WTW_PEV_CALL_END:
			{
				__LOG(wtw, L"DISP", L"_END");

				wtwProtocolCall* pPc = (wtwProtocolCall*)wParam;
				if(pPc != 0)
				{
					if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_VIDEO)
					{
						parent->onAudioVideo(EVENT_AV_VIDEO_END, activeOnNet);
					}
					else if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_AUDIO)
					{
						parent->onAudioVideo(EVENT_AV_AUDIO_END, activeOnNet);
					}
				}
				break;
			}
		case WTW_PEV_CALL_ACCEPT:
			{
				__LOG(wtw, L"DISP", L"_ACCEPT");

				wtwProtocolCall* pPc = (wtwProtocolCall*)wParam;
				if(pPc != 0)
				{
					if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_REJECTED)
					{
						if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_VIDEO)
						{
							parent->onAudioVideo(EVENT_AV_VIDEO_REJECTED, activeOnNet);
						}
						else if(pPc->flags & WTW_PROTOCOL_CALL_FLAG_AUDIO)
						{
							parent->onAudioVideo(EVENT_AV_AUDIO_REJECTED, activeOnNet);
						}
					}
					else
					{
						parent->stopAll();
					}
				}
				break;
			}
		case WTW_PEV_NETWORK_LOGIN: 
			{
				parent->onMiscEvent(EVENT_MISC_NETWORK_LOGGED_IN);
				break;
			}
		case WTW_PEV_NETWORK_LOGOUT: 
			{
				parent->onMiscEvent(EVENT_MISC_NETWORK_LOGGED_OFF);
				break;
			}
		case WTW_PEV_AUTH_REQUEST:
			{
				parent->onMiscEvent(EVENT_MISC_AUTH_REQUEST);
				break;
			}
		case WTW_PEV_NEW_MAIL_NOTIFY:
			{
				parent->onMiscEvent(EVENT_MISC_NEW_MAIL);
				break;
			}
		case WTW_PEV_ALERT_RECV:
			{
				parent->onMiscEvent(EVENT_MISC_ALERT);
				break;
			}
		case WTW_PEV_FILE_TRANSFER_REQ:
			{
				parent->onMiscEvent(EVENT_MISC_INCOMING_FILE);
				break;
			}
		default: { }
		}
		return 0;
	}

#endif // WTW_PARAM

};

#endif

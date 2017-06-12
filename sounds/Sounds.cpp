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

#include "stdinc.h"
#include <ctime>

#include "Sounds.hpp"
#include "WinApiFile.hpp"

#define PLAY_FUNC_ID L"Sounds/Player/PlayFile"

const wchar_t* SoundsManager::PresenceSettingNames[SoundsManager::PRESENCE_LAST] = 
{
	L"avail",
	L"chat",
	L"dnd",
	L"away",
	L"xa",
	L"invisible",
	L"offline",
	L"error",
	L"blocked"
};

SoundsManager* SoundsManager::__instance = 0;

void SoundsManager::createInstance()
{
	if(__instance != 0)
	{
		delete __instance;
	}

	__instance = new SoundsManager;
}

void SoundsManager::destroyInstance()
{
	if(__instance != 0)
	{
		delete __instance;
		__instance = 0;
	}
}

SoundsManager& SoundsManager::instance()
{
	return *__instance;
}

SoundsManager::SoundsManager() : lastMessageEvent(0), lastStatusEvent(0), lastMiscEvent(0), protoHook(0), extFuncHandle(0)
{
	if(wtw->fnExists(IGNORE_ADD_OPTION))
	{
		utlIgnoreOptionEntry ie;
		ie.lockId = L"sounds/ignoreMessage";
		ie.lockName = L"Powiadomienia d�wi�kowe o wiadomo�ciach";
		ie.lockOptionName = L"Ignoruj powiadomienia d�wi�kowe o wiadomo�ciach";
		
		wtw->fnCall(IGNORE_ADD_OPTION, ie, 0);
	
		ie.lockId = L"sounds/ignorePresence";
		ie.lockName = L"Powiadomienia d�wi�kowe o zmianie statusu";
		ie.lockOptionName = L"Ignoruj powiadomienia d�wi�kowe o zmianie statusu";
		
		wtw->fnCall(IGNORE_ADD_OPTION, ie, 0);
	}

	extFuncHandle = wtw->fnCreate(PLAY_FUNC_ID, SoundsManager::externalFuncPlayFile, (void*)this);

	std::wstring curSetFile;
	cfgGetStr(curSetFile, L"sounds.xml.set");

	try
	{
		reload(curSetFile);
	}
	catch(...)
	{
		// ...
	}
}

SoundsManager::~SoundsManager()
{
	if(extFuncHandle != 0)
	{
		wtw->fnDestroy(extFuncHandle);
		extFuncHandle = 0;
	}
	
	clear();

	player.cleanup(true);
}

void SoundsManager::XmlReader::OnStartElement(const XML_Char* pszName, const XML_Char** papszAttrs) 
{
	if(!papszAttrs || !pszName)
	{
		return;
	}

	if(!wcscmp(pszName, L"wtwSounds") && !wcscmp(papszAttrs[0], L"author") && !wcscmp(papszAttrs[2], L"description"))
	{
		mng->author = std::wstring(papszAttrs[1]);
		mng->description = std::wstring(papszAttrs[3]);
	}
	else if(!wcscmp(pszName, L"item"))
	{
		const wchar_t* type = 0;
		const wchar_t* file = 0;

		for(int i = 0; papszAttrs[i] != 0; i += 2)
		{
			if(!wcscmp(papszAttrs[i], L"type"))
			{
				type = papszAttrs[i+1];
			}
			else if(!wcscmp(papszAttrs[i], L"file"))
			{
				file = papszAttrs[i+1];
			}
		}

		if(type && file)
		{
			int pos = SoundEvents::getEventFromName(type);
			if(pos != -1)
			{
				mng->items[pos]->file = std::wstring(file);
			}
		}
	} 
	else if(!wcscmp(pszName, L"external"))
	{
		const wchar_t* key = 0;
		const wchar_t* file = 0;

		for(int i = 0; papszAttrs[i] != 0; i += 2)
		{
			if(!wcscmp(papszAttrs[i], L"key"))
			{
				key = papszAttrs[i+1];
			}
			else if(!wcscmp(papszAttrs[i], L"file"))
			{
				file = papszAttrs[i+1];
			}
		}

		if(key != 0 && file != 0)
		{
			if(mng->externalItems.find(key) == mng->externalItems.end())
			{
				Item* item = new Item;
				item->enabled = true;
				item->file = file;
				mng->externalItems.insert(std::make_pair(std::wstring(key), item));
			}
		}
	}
}

void SoundsManager::reload(const std::wstring& fileName) throw(...)
{
	cfgSetStr(L"sounds.xml.set", fileName.c_str());

	author.clear();
	description.clear();

	setVolume(getVolume()); // ok, to jest dziwne... :P

	clear();

	if(fileName.empty())
	{
		throw 0;
	}

	if(!protoHook)
	{
		protoHook = wtw->evHook(WTW_ON_PROTOCOL_EVENT, SoundEvents::dispatchProtocol, (void*)this);
	}

	for(int i = 0; i < SoundEvents::EVENT_LAST; ++i) 
	{
		Item* item = new Item;
		item->enabled = cfgGetInt(SoundEvents::getEventSettingName(i).c_str(), 1) ? true : false;
		items.push_back(item);
	}

	std::string buf;
	WinApiFile file;

	if(!file.open(player.getRootPath() + fileName, WinApiFile::READ_MODE, WinApiFile::OPEN_IF_EXIST))
	{
		throw 1;
	}

	buf.resize(file.getFileSize());
	file >> buf;
	file.close();

	XmlReader xml(this);
	xml.Create();

	if(!xml.Parse(buf.c_str(), buf.length())) 
	{
		__LOG(wtw, L"SNDS", xml.GetErrorString());
		clear();
		throw 2;
	}

	//fallback
	{
		std::wstring mainChatSound = items[SoundEvents::EVENT_MESSAGE_CHAT]->file;

		if(items[SoundEvents::EVENT_MESSAGE_CHAT_NEW]->file.empty())
		{
			items[SoundEvents::EVENT_MESSAGE_CHAT_NEW]->file = mainChatSound;
		}

		if(items[SoundEvents::EVENT_MESSAGE_CHAT_INACTIVE]->file.empty())
		{
			items[SoundEvents::EVENT_MESSAGE_CHAT_INACTIVE]->file = mainChatSound;
		}
	}
}

void SoundsManager::reset()
{
	cfgSetStr(L"sounds.xml.set", L"");
	clear();
}

void SoundsManager::clear()
{
	if(protoHook) 
	{
		wtw->evUnhook(protoHook);
		protoHook = 0;
	}

	for(ItemList::iterator i = items.begin(); i != items.end(); ++i)
	{
		delete *i;
	}

	items.clear();
}

bool SoundsManager::checkTime(time_t& t, const wchar_t* name, const wchar_t* delay)
{
	if(cfgGetInt(name, 0)) 
	{
		time_t now = time(0);

		if((now - t) > cfgGetInt(delay, 1)) 
		{
			t = now;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool SoundsManager::checkStatus()
{
	if(cfgGetInt(L"sounds.muteOnAvail", 0))
	{
		wtwPresenceDef p;
		initStruct(p);
		wtw->fnCall(WTW_PF_WTW_STATUS_GET, (WTW_PARAM)&p, 0);
		return p.curStatus == WTW_PRESENCE_ONLINE || p.curStatus == WTW_PRESENCE_CHAT;
	}
	else if(cfgGetInt(L"sounds.muteOnDnd", 0))
	{
		wtwPresenceDef p;
		initStruct(p);
		wtw->fnCall(WTW_PF_WTW_STATUS_GET, (WTW_PARAM)&p, 0);
		return p.curStatus == WTW_PRESENCE_DND;
	}

	return false;
}

bool SoundsManager::checkIgnore(int presenceNew, int presenceOld)
{
	int idx1, idx2;
	//	__LOG_F(wtw, 1, L"ignore", L"new = %d, old = %d", presenceNew, presenceOld);

	switch(presenceOld) // z wtwPresenceDef
	{
	case WTW_PRESENCE_ONLINE:		idx1 = SoundsManager::PRESENCE_ONLINE; break;
	case WTW_PRESENCE_CHAT:			idx1 = SoundsManager::PRESENCE_CHAT; break;
	case WTW_PRESENCE_DND:			idx1 = SoundsManager::PRESENCE_DND; break;
	case WTW_PRESENCE_AWAY:			idx1 = SoundsManager::PRESENCE_AWAY; break;
	case WTW_PRESENCE_XA:			idx1 = SoundsManager::PRESENCE_XA; break;
	case WTW_PRESENCE_INV:			idx1 = SoundsManager::PRESENCE_INVISIBLE; break;
	case WTW_PRESENCE_OFFLINE:		idx1 = SoundsManager::PRESENCE_OFFLINE; break;
	case WTW_PRESENCE_ERROR:		idx1 = SoundsManager::PRESENCE_ERROR; break;
	case WTW_PRESENCE_BLOCK:		idx1 = SoundsManager::PRESENCE_BLOCKED; break;
	default: idx1 = -1;
	}

	switch(presenceNew) // z EventDispatcher'a
	{
	case SoundEvents::EVENT_PRESENCE_ONLINE:		idx2 = SoundsManager::PRESENCE_ONLINE; break;
	case SoundEvents::EVENT_PRESENCE_CHAT:			idx2 = SoundsManager::PRESENCE_CHAT; break;
	case SoundEvents::EVENT_PRESENCE_DND:			idx2 = SoundsManager::PRESENCE_DND; break;
	case SoundEvents::EVENT_PRESENCE_AWAY:			idx2 = SoundsManager::PRESENCE_AWAY; break;
	case SoundEvents::EVENT_PRESENCE_XA:			idx2 = SoundsManager::PRESENCE_XA; break;
	case SoundEvents::EVENT_PRESENCE_INVISIBLE:		idx2 = SoundsManager::PRESENCE_INVISIBLE; break;
	case SoundEvents::EVENT_PRESENCE_OFFLINE:		idx2 = SoundsManager::PRESENCE_OFFLINE; break;
	case SoundEvents::EVENT_PRESENCE_ERROR:			idx2 = SoundsManager::PRESENCE_ERROR; break;
	case SoundEvents::EVENT_PRESENCE_BLOCKED:		idx2 = SoundsManager::PRESENCE_BLOCKED; break;
	default: idx2 = -1;
	}

	if(idx1 == -1 || idx2 == -1)
	{
		return false;
	}

	wchar_t buf[512] = { 0 };
	wsprintf(buf, L"sounds.presence.ignore.%s.%s", SoundsManager::PresenceSettingNames[idx1], SoundsManager::PresenceSettingNames[idx2]);

	return cfgGetInt(buf, 0) != 0;
}

void SoundsManager::onMessage(int type, bool activeOnNet) 
{
	if(activeOnNet == false && cfgGetInt(L"sounds.alwaysNotifyOnMsg", 1) == 0)
	{
		return;
	}

	if(items.empty() || type < 0 || type >= SoundEvents::EVENT_LAST)
	{
		return;
	}

	if(checkStatus() || checkTime(lastMessageEvent, L"sounds.filter.message", L"sounds.delay.message"))
	{
		return;
	}

	Item* i = items[type];

	if(i->enabled)
	{
		player.playSound(i->file);
	}
}

void SoundsManager::onStatus(int type, int oldStatus, bool activeOnNet) 
{
	if(activeOnNet == false)
	{
		return;
	}

	if(items.empty() || type < 0 || type >= SoundEvents::EVENT_LAST)
	{
		return;
	}

	// gdy ignorujemy to nie aktualizujemy czasu odpalenia eventu, temu to sprawdzenie jest wczesniej
	if(checkIgnore(type, oldStatus))
	{
		return;
	}

	if(checkStatus() || checkTime(lastStatusEvent, L"sounds.filter.status", L"sounds.delay.status"))
	{
		return;
	}

	Item* i = items[type];

	if(i->enabled)
	{
		player.playSound(i->file);
	}
}

void SoundsManager::onAudioVideo(int type, bool activeOnNet)
{
	if(activeOnNet == false)
	{
		return;
	}

	if(items.empty() || type < 0 || type >= SoundEvents::EVENT_LAST)
	{
		return;
	}

	if(checkStatus())
	{
		return;
	}

	Item* i = items[type];

	bool loop;

	if(type == SoundEvents::EVENT_AV_AUDIO_REJECTED || type == SoundEvents::EVENT_AV_AUDIO_END || type == SoundEvents::EVENT_AV_VIDEO_REJECTED || type == SoundEvents::EVENT_AV_VIDEO_END)
	{
		loop = false;
	}
	else
	{
		loop = true;
	}

	{
		if(i->enabled)
		{
			if(loop == false)
			{
				player.cleanup(true);
			}

			player.playSound(i->file/*, false, loop*/);
		}
	}
}

void SoundsManager::onMiscEvent(int type) 
{
	if(items.empty() || type < 0 || type >= SoundEvents::EVENT_LAST)
	{
		return;
	}

	if(checkStatus() || checkTime(lastMiscEvent, L"sounds.filter.misc", L"sounds.delay.misc"))
	{
		return;
	}

	Item* i = items[type];

	if(i->enabled)
	{
		player.playSound(i->file);
	}
}

WTW_PTR SoundsManager::externalFuncPlayFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr) 
{
	SoundsManager* m = reinterpret_cast<SoundsManager*>(ptr);
	const wchar_t* param = (const wchar_t*)lParam;

	if(param && m) 
	{
		if(Player::isFileExist(param)) 
		{
			return m->player.playSound(param, true) ? 0 : 1;
		}
		else
		{
			SoundsManager::MappedItems::const_iterator i = m->externalItems.find(param);

			if(i != m->externalItems.end())
			{
				return m->player.playSound(i->second->file) ? 0 : 1;
			}
		}
	}

	return 1;
}

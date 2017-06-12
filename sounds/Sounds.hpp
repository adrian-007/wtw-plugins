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

#ifndef SOUNDS_HPP
#define SOUNDS_HPP

#include <vector>
#include <unordered_map>

#include "EventDispatcher.hpp"
#include "ExpatImpl.h"
#include "Player.hpp"

class SoundsManager {
private:
	struct Item {
		Item() : enabled(false) 
		{
		}

		std::wstring file;
		bool enabled;
	};

	class XmlReader : public CExpatImpl<SoundsManager::XmlReader> {
	public:
		XmlReader(SoundsManager* e) : mng(e) 
		{ 
		}

		void OnPostCreate()
		{
			EnableElementHandler();
		}

		void OnStartElement(const XML_Char *pszName, const XML_Char **papszAttrs);
	private:
		SoundsManager* mng;
	};

public:
	typedef EventDispatcher<SoundsManager> SoundEvents;

	static void createInstance();
	static void destroyInstance();
	static SoundsManager& instance();

	enum 
	{
		PRESENCE_ONLINE = 0,
		PRESENCE_CHAT,
		PRESENCE_DND,
		PRESENCE_AWAY,
		PRESENCE_XA,
		PRESENCE_INVISIBLE,
		PRESENCE_OFFLINE,
		PRESENCE_ERROR,
		PRESENCE_BLOCKED,
		PRESENCE_LAST
	};

	static const wchar_t* PresenceSettingNames[PRESENCE_LAST];

	SoundsManager();
	virtual ~SoundsManager();

	void reload(const std::wstring& fileName) throw(...);
	void reset();
	void clear();

	const std::wstring& getAuthor() const 
	{
		return author;
	}

	const std::wstring& getDescription() const 
	{
		return description;
	}

	Player* getPlayer() 
	{
		return &player;
	}

	void setVolume(int vol) 
	{
		if(vol > 9 || vol < 0)
		{
			vol = 9;
		}

		cfgSetInt(L"sounds.volume", vol);
	}

	inline int getVolume() 
	{
		return cfgGetInt(L"sounds.volume", 9);
	}

	inline void stopAll()
	{
		player.cleanup(true);
	}

private:
	static SoundsManager* __instance;

	typedef std::vector<Item*> ItemList;
	typedef std::tr1::unordered_map<std::wstring, Item*> MappedItems;

	friend class SoundEvents;

	bool checkIgnore(int presenceNew, int presenceOld);

	void onStatus(int type, int oldStatus, bool activeOnNet);
	void onMessage(int type, bool activeOnNet);
	void onAudioVideo(int type, bool activeOnNet);
	void onMiscEvent(int type);

	bool checkTime(time_t& t, const wchar_t* name, const wchar_t* delay);
	bool checkStatus();

	MappedItems externalItems;
	ItemList items;
	Player player;

	std::wstring author;
	std::wstring description;

	HANDLE protoHook;
	HANDLE extFuncHandle;

	time_t lastStatusEvent;
	time_t lastMessageEvent;
	time_t lastMiscEvent;

	static WTW_PTR externalFuncPlayFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr);
};

#endif

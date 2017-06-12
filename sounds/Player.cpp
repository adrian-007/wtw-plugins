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
#include "Player.hpp"

#include "../utils/wtw.hpp"
#include <mmsystem.h>
#include "libplayer/mp3.h"

int Player::volumes[10] = 
{
	-3000,
	-2750,
	-2500,
	-2250,
	-1750,
	-1250,
	-1000,
	-750,
	-500,
	0
};

bool Player::isFileExist(const std::wstring& path)
{
	DWORD attrs = GetFileAttributesW(path.c_str());
	return attrs != 0xFFFFFFFF;
}

Player::Player() : mute(false)
{
	wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, rootPath);

	rootPath += L"Sounds\\";

	::CreateDirectoryW(rootPath.c_str(), 0);

	wtwTimerDef t;
	initStruct(t);
	t.id = L"sounds/timer";
	t.sleepTime = 5000;
	t.callback = onTimer;
	t.cbData = (void*)this;

	wtw->fnCall(WTW_TIMER_CREATE, (WTW_PARAM)&t, 0);
}

Player::~Player()
{
	wtw->fnCall(WTW_TIMER_DESTROY, (WTW_PARAM)L"sounds/timer", 0);
}

void Player::cleanup(bool force /*= false*/)
{
	long e;
	int cnt = 0;

	for(SoundList::iterator i = sounds.begin(); i != sounds.end();)
	{
		Mp3* s = *i;

		if(force == true)
		{
			s->Stop();
		}

		if(s->WaitForCompletion(0, &e) == true || force == true)
		{
			i = sounds.erase(i);
			delete s;
			++cnt;
		}
		else
		{
			++i;
		}
	}

	if(cnt > 0 && wtw != 0)
	{
		__LOG_F(wtw, WTW_LOG_LEVEL_DEBUG, L"SNGC", L"Player collected %d unused object(s) (%d bytes)", cnt, sizeof(Mp3) * cnt);
	}
}

bool Player::getMute()
{
	mute = !mute;

	if(mute == true)
	{
		for(SoundList::iterator i = sounds.begin(); i != sounds.end(); ++i)
		{
			Mp3* s = *i;
			s->Stop();
		}
	}

	return mute;
}

bool Player::playSound(const std::wstring& file, bool absolute)
{
	if(mute || wtw->fnCall(WTW_GET_SILENT_MODE, 0, 0))
	{
		return false;
	}

	if(wtw->fnExists(WTW_IS_FULLSCREEN_ACTIVE))
	{
		if(cfgGetInt(L"sounds.disableOnFullscreen", 0) && wtw->fnCall(WTW_IS_FULLSCREEN_ACTIVE, 0, 0) == S_OK)
		{
			return false;
		}
	}

	if(file.empty() || (file[0] == L'%' && file[file.length()-1] == L'%')) 
	{
		int type = -1;

		if(!wcscmp(file.c_str(), L"%beep%"))
		{
			type = MB_OK;
		} 
		else if(!wcscmp(file.c_str(), L"%warning%"))
		{
			type = MB_ICONWARNING;
		}
		else if(!wcscmp(file.c_str(), L"%error%"))
		{
			type = MB_ICONERROR;
		}
		else if(!wcscmp(file.c_str(), L"%info%"))
		{
			type = MB_ICONINFORMATION;
		}
		else if(cfgGetInt(L"sounds.use.sysBeep", 0))
		{
			type = MB_OK;
		}

		if(type != -1)
		{
			MessageBeep(type);
		}
	}
	else
	{
		std::wstring absolutePath = (absolute ? file : (rootPath + file));

		if(isFileExist(absolutePath))
		{
			Mp3* s = new Mp3;

			if(s->Load(absolutePath.c_str()) == true)
			{
				s->SetVolume(getVolume());

				if(s->Play())
				{
					sounds.push_back(s);
					return true;
				}
			}

			delete s;

			__LOG(wtw, L"SNPL", L"Failed to play sound using DirectShow interface, falling back to PlaySound", WTW_LOG_LEVEL_ERROR);

			DWORD flags = SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP;
			PlaySoundW(absolutePath.c_str(), 0, flags);

			return true;
		}
	}

	return false;
}

long Player::getVolume()
{
	int volume = cfgGetInt(L"sounds.volume", 9);

	if(volume > 9 || volume < 0)
	{
		volume = 9;
		cfgSetInt(L"sounds.volume", volume);
	}

	return volumes[volume];
}

WTW_PTR Player::onTimer(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
{
	wtwTimerEvent* e = (wtwTimerEvent*)wParam;
	if(!e || !ptr)
	{
		return 0;
	}

	if(wcscmp(e->id, L"sounds/timer") == 0 && e->event == WTW_TIMER_EVENT_TICK)
	{
		WTW_PARAM p = (WTW_PARAM)ptr;
		((Player*)p)->cleanup();
	}

	return 0;
}

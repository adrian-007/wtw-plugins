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

#ifndef WTW_SOUNDS_PLAYER_HPP
#define WTW_SOUNDS_PLAYER_HPP

#include <deque>

class Mp3;

class Player
{
public:
	Player();
	~Player();
	
	static bool isFileExist(const std::wstring& path);

	bool playSound(const std::wstring& file, bool absolute = false);
	bool getMute();
	void cleanup(bool force = false);

	inline const std::wstring& getRootPath() const
	{
		return rootPath;
	}
private:
	static int volumes[10];
	long getVolume();

	typedef std::deque<Mp3*> SoundList;
	SoundList sounds;

	bool mute;
	std::wstring rootPath;

	static WTW_PTR onTimer(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);
};

#endif

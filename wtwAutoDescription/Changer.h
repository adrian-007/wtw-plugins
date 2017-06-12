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

#ifndef CHANGER_H
#define CHANGER_H

#include <string>
#include <list>

#define CHANGER_TIMER_ID L"wtwAutoDesc/Timer"

class Changer {
public:
	Changer();
	~Changer();

	void tick();
	void reload();

	static WTW_PTR onTimer(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr) {
		wtwTimerEvent* e = (wtwTimerEvent*)wParam;
		if(!e || !ptr)
			return 0;

		if(wcscmp(e->id, CHANGER_TIMER_ID) == 0 && e->event == WTW_TIMER_EVENT_TICK) {
			WTW_PARAM p = (WTW_PARAM)ptr;
			((Changer*)p)->tick();
		}
		return 0;
	}
	bool changeState() {
		active = !active;
		return active;
	}
	const std::wstring getFilePath() const {
		return filePath;
	}

private:
	unsigned int ticks;
	bool active;

	std::wstring filePath;
	std::list<std::wstring> descriptions;
	std::list<std::wstring>::iterator curPos;
};

#endif

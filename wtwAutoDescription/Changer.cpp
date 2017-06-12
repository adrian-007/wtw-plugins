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
#include "Changer.h"
#include "../utils/wtw.hpp"

#include <fstream>

#define LOG(x) __LOG(wtw, L"AutoDescription", x)

Changer::Changer() : ticks(0), active(false) {
	wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, filePath);
	filePath += L"AutoDescriptions.txt";

	wtwTimerDef t;
	initStruct(t);
	t.id = CHANGER_TIMER_ID;
	t.sleepTime = 1000;
	t.callback = Changer::onTimer;
	t.cbData = (void*)this;
	wtw->fnCall(WTW_TIMER_CREATE, (WTW_PARAM)&t, 0);

	curPos = descriptions.begin();

	reload();
}

Changer::~Changer() {
	wtw->fnCall(WTW_TIMER_DESTROY, (WTW_PARAM)CHANGER_TIMER_ID, 0);
}

void Changer::tick() {
	if(active && descriptions.size() > 0 && (++ticks % cfgGetInt(L"tick", 120)) == 0) {
		if(curPos == descriptions.end())
			curPos = descriptions.begin();

		wtwPresenceDef p;
		initStruct(p);
		wtw->fnCall(WTW_PF_WTW_STATUS_GET, (WTW_PARAM)&p, 0);
		if((p.curStatus & WTW_PRESENCE_INV) == 0 || (p.curStatus & WTW_PRESENCE_OFFLINE) == 0 && cfgGetInt(L"noHidden", 1) == 0) {
			p.curDescription = curPos->c_str();
			wtw->fnCall(WTW_PF_WTW_STATUS_SET, (WTW_PARAM)&p, 0);
			curPos++;
		}
		ticks = 0;
	}
}

void Changer::reload() {
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0);
	std::wstring buf;

	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD fileSize = GetFileSize(hFile, 0);
		if(fileSize != 0xFFFFFFFF) {
			DWORD r;
			buf.resize(fileSize);
			memset(&buf[0], 0, buf.size());
			if(!ReadFile(hFile, &buf[0], buf.size(), &r, 0)) {
				buf.clear();
				LOG(L"can't read file");
			} else {
				buf[r] = 0;
				buf.erase(0, 1);
			}
		}

		CloseHandle(hFile);

		if(!buf.empty()) {
			std::wstring::size_type i = 0, j = 0;

			while((i = buf.find(L"\r\n", j)) != std::wstring::npos) {
				descriptions.push_back(buf.substr(j, i-j));
				j = i + 2;
			}
			if(j < buf.size())
				descriptions.push_back(buf.substr(j, buf.size()-j));

			i = 0;

			for(std::list<std::wstring>::iterator it = descriptions.begin(); it != descriptions.end(); ++it) {
				std::wstring& str = *it;
				while((i = str.find(L"\\n", i)) != std::wstring::npos) {
					str.replace(i, 2, L"\n");
					i += 2;
				}
			}
		}
	} else {
		LOG(L"can't open file");
	}
}

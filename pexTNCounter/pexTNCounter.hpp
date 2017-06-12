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

#ifndef PEX_GG_TN_COUNTER
#define PEX_GG_TN_COUNTER

#include <vector>

#define PRIVATE_RLS

class pexTNCounter {
	static pexTNCounter* inst;
public:
	pexTNCounter(HINSTANCE hInstance);
	~pexTNCounter();

	static void create(pexTNCounter* m) {
		pexTNCounter::inst = m;
	}

	static void destroy() {
		delete pexTNCounter::inst;
		pexTNCounter::inst = 0;
	}

	static pexTNCounter* instance() {
		return pexTNCounter::inst;
	}

	static bool isInstance() {
		return pexTNCounter::inst != 0;
	}

private:
	struct item {
		typedef std::vector<item*> list;

		item(HWND parent);
		~item();

		std::wstring id;
		std::wstring netClass;
		int netId;

		CStatic label, frame;
		unsigned int subclassID;
		static unsigned int subclassIDs;
		static HFONT hFont;
		static LRESULT CALLBACK SubclassFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

		void setCount(unsigned int count);
	};

	void onWindowCreated(wtwContactDef* cnt, wtwChatWindowInfo* nfo);
	void onWindowDestroyed(wtwContactDef* cnt, wtwChatWindowInfo* nfo);
	void onTypingNotify(wtwContactDef* cnt, int count);

	static WTW_PTR onWndCreate(WTW_PARAM cnt, WTW_PARAM nfo, void* ptr) {
		((pexTNCounter*)ptr)->onWindowCreated((wtwContactDef*)cnt, (wtwChatWindowInfo*)nfo);
		return 0;
	}
	static WTW_PTR onWndDestroy(WTW_PARAM cnt, WTW_PARAM nfo, void* ptr) {
		((pexTNCounter*)ptr)->onWindowDestroyed((wtwContactDef*)cnt, (wtwChatWindowInfo*)nfo);
		return 0;
	}
	static WTW_PTR onProtoEvent(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr) {
		wtwProtocolEvent* evn = (wtwProtocolEvent*)lParam;

		if(!evn || evn->type == WTW_PEV_TYPE_AFTER)
			return 0;

		if(wcscmp(evn->netClass, L"GG") == 0) {
			switch(evn->event) 
			{
			case WTW_PEV_RAW_DATA_RECV:
				{
					wtwRawDataDef* rd = (wtwRawDataDef*)wParam;
					if(rd)
					{
						if(rd->flags & WTW_RAW_FLAG_BIN) {
							unsigned int type = *(unsigned int*)(rd->pData);
							//unsigned int len = *(unsigned int*)(rd->pData + 4);
							if(type == 0x0059) // typing notify
							{
								unsigned short count = *(unsigned short*)(rd->pData + 8);
								unsigned int uin = *(unsigned int*)(rd->pData + 10);

								wtwContactDef c;
								c.id = new wchar_t[512];
								c.netClass = evn->netClass;
								c.netId = evn->netId;
								wsprintf((LPWSTR)c.id, L"%d", uin);

								((pexTNCounter*)ptr)->onTypingNotify(&c, count);
#ifdef PRIVATE_RLS
								__LOG_F(wtw, WTW_LOG_LEVEL_INFO, L"TYPN", L"%s typing, count: %d", c.id, count);
#endif
								delete c.id;
							}
						}
					}
					break;
				}
			case WTW_PEV_MESSAGE_RECV:
				{
					wtwMessageDef* msg = (wtwMessageDef*)wParam;
					if(!msg)
						return 0;
					if((msg->msgFlags & WTW_MESSAGE_FLAG_INCOMING) == 0)
						return 0;

					if(wcscmp(msg->contactData.netClass, L"GG") == 0) {
						if(msg->msgFlags & WTW_MESSAGE_FLAG_CHAT_MSG)
							((pexTNCounter*)ptr)->onTypingNotify(&msg->contactData, 0);
					}
					break;
				}
			case WTW_PEV_TYPING_NOTIFY_RECV:
				{
					break;
				}
			}
		}
		return 0;
	}

	HINSTANCE hInstance;
	HANDLE wndCreateHook;
	HANDLE wndDestroyHook;
	HANDLE protoHook;

	pexTNCounter::item::list items;
	pexTNCounter::item* findItem(const wchar_t* id, const wchar_t* netClass, int netId);
};

#endif

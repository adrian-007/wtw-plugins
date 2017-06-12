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
#include <string>

#include "SettingsPage.h"
#include "winamp.h"
#include "../pluginInfo.h"

#define memzero(x, y) memset(x, 0, y)

void* config = 0;
WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;

bool __stdcall DllMain(HANDLE hInst, DWORD reason, LPVOID /*reserved*/) 
{
	switch(reason)
	{
	case DLL_PROCESS_ATTACH: 
		{
			ghInstance = (HINSTANCE)hInst;
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			ghInstance = 0;
			break;
		}
	default:
		break;
	}

	return true;
}

WTWPLUGINFO plugInfo = 
{
	sizeof(WTWPLUGINFO),											// rozmiar struktury
	L"wtwWinampStatus",												// nazwa wtyczki
	L"Aktualnie odtwarzany utwór w opisie",							// opis wtyczki
	__COPYRIGHTS(2009),												// prawa autorskie
	__AUTHOR_NAME,													// autor
	__AUTHOR_CONTACT,												// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,												// strona www autora
	__AUTOUPDATE_URL(L"wtwWinampStatus.xml"),						// url do pliku xml z danymi do autoupdate
	PLUGIN_API_VERSION,												// wersja api
	MAKE_QWORD(0, 2, 1, 2),											// wersja wtyczki
	WTW_CLASS_UTILITY,												// klasa wtyczki
	0,																// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{35E0ABC4-3F84-4081-A7A1-4976BE125DA8}",						// guid (jezeli chcemy tylko jedna instancje wtyczki)
	0,																// zaleznosci
	0, 0, 0, 0														// zarezerwowane (4 pola)
};

#define SM_TIMER_ID L"wtwWinampStatus/Timer"

class StatusManager
{
public:
	StatusManager() : active(false) 
	{
		wtwPresenceDef p;
		initStruct(p);
		wtw->fnCall(WTW_PF_WTW_STATUS_GET, (WTW_PARAM)&p, 0);
		userStatus = p.curDescription;

		descChangeHook = wtw->evHook(WTW_ON_BEFORE_STATUS_SET, onDescChange, (void*)this);
		msgProcessHook = wtw->evHook(WTW_EVENT_CHATWND_BEFORE_MSG_PROC, onMsgProcess, (void*)this);
	}

	~StatusManager() 
	{
		setTimer(false);

		wtw->evUnhook(descChangeHook);
		wtw->evUnhook(msgProcessHook);

		setUserStatus();
	}

	void setTimer(bool enable)
	{
		if(enable)
		{
			wtwTimerDef t;
			initStruct(t);
			t.id = SM_TIMER_ID;
			t.sleepTime = 1000;
			t.callback = onTimer;
			t.cbData = (void*)this;
			wtw->fnCall(WTW_TIMER_CREATE, (WTW_PARAM)&t, 0);
		}
		else
		{
			wtw->fnCall(WTW_TIMER_DESTROY, (WTW_PARAM)SM_TIMER_ID, 0);
		}
	}

	bool changeState()
	{
		active = !active;

		setTimer(active);

		if(active == false)
		{
			setUserStatus();
			winampStatus.clear();
			currentTitle.clear();
		}
		return active;
	}

private:
	static WTW_PTR onTimer(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
	{
		wtwTimerEvent* e = (wtwTimerEvent*)wParam;
		if(!e || !ptr)
		{
			return 0;
		}

		if(wcscmp(e->id, SM_TIMER_ID) == 0 && e->event == WTW_TIMER_EVENT_TICK)
		{
			WTW_PARAM p = (WTW_PARAM)ptr;
			((StatusManager*)p)->checkStatus();
		}

		return 0;
	}

	static WTW_PTR onDescChange(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
	{
		wtwNewGlobalStatus* gs = (wtwNewGlobalStatus*)wParam;
		((StatusManager*)ptr)->onDescriptionChange(gs);

		return 0;
	}

	static WTW_PTR onMsgProcess(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
	{
		wtwBmpStruct* m = (wtwBmpStruct*)wParam;
		((StatusManager*)ptr)->onMessageReplace(m);

		return 0;
	}

	void replaceParam(std::wstring& buf, const std::wstring& name, const std::wstring& value) const
	{
		std::wstring::size_type i;

		while((i = buf.find(name)) != std::wstring::npos) 
		{
			buf.replace(i, name.length(), value);
			i += value.length();
		}
	}

	std::wstring formatWinampStatus(HWND wnd, bool replaceDesc = true) const
	{
		std::wstring format;
		cfgGetStr(format, L"winamp.format", L"%songTitle% - wtwWinampStatus");

		wchar_t buf[128];
		int tmp;

		tmp = (int)SendMessage(wnd, WM_USER, 0, IPC_GETINFO);
		memzero(buf, sizeof(buf));
		wsprintf(buf, L"%d", tmp);
		replaceParam(format, L"%sampleRate", buf);

		tmp = (int)SendMessage(wnd, WM_USER, 1, IPC_GETINFO);
		memzero(buf, sizeof(buf));
		wsprintf(buf, L"%d", tmp);
		replaceParam(format, L"%bitRate%", buf);

		tmp = (int)SendMessage(wnd, WM_USER, 2, IPC_GETINFO);
		memzero(buf, sizeof(buf));
		wsprintf(buf, L"%d", tmp);
		replaceParam(format, L"%channels%", buf);

		tmp = (int)SendMessage(wnd, WM_USER, 1, IPC_GETOUTPUTTIME);
		memzero(buf, sizeof(buf));
		wsprintf(buf, L"%01lu:%02d:%02d", (unsigned long)(tmp / (60*60)), (int)((tmp / 60) % 60), (int)(tmp % 60));
		replaceParam(format, L"%length%", buf);

		if(replaceDesc)
		{
			replaceParam(format, L"%userStatus%", userStatus);
		}

		replaceParam(format, L"%songTitle%", currentTitle);
		return format;
	}

	std::wstring getSongTitle(HWND wnd) const
	{
		std::wstring curTitle;

		curTitle.resize(::GetWindowTextLength(wnd) + 1);
		curTitle.resize(GetWindowTextW(wnd, &curTitle[0], curTitle.size()));

		std::wstring::size_type i;

		i = curTitle.find(L". ");
		if(i != std::wstring::npos)
		{
			curTitle = curTitle.substr(i+2);
		}

		i = curTitle.rfind(L" - Winamp");
		if(i != std::wstring::npos)
		{
			curTitle = curTitle.substr(0, i);
		}
		return curTitle;
	}

	void checkStatus()
	{
		HWND wnd = FindWindowW(L"Winamp v1.x", NULL);
		bool cleanup = true;

		if(wnd && active)
		{
			int state = (int)SendMessage(wnd, WM_USER, 0, IPC_ISPLAYING);
			bool isPlaying = state == 1 || (state == 3 && cfgGetInt(L"winamp.setAtPause", 0) != 0);

			if(isPlaying)
			{
				cleanup = false;

				std::wstring curTitle = getSongTitle(wnd);

				if(wcscmp(curTitle.c_str(), currentTitle.c_str()) == 0)
				{
					return;
				}

				wtwPresenceDef p;
				initStruct(p);
				wtw->fnCall(WTW_PF_WTW_STATUS_GET, (WTW_PARAM)&p, 0);

				currentTitle = curTitle;

				winampStatus = formatWinampStatus(wnd);
				p.curDescription = winampStatus.c_str();
				wtw->fnCall(WTW_PF_WTW_STATUS_SET, (WTW_PARAM)&p, 0);
			}
		}

		if(cleanup) 
		{
			setUserStatus();
			winampStatus.clear();
			currentTitle.clear();
		}
	}

	void onDescriptionChange(wtwNewGlobalStatus* gs)
	{
		//	__LOG_F(wtw, 1, L"WINAMP", L"userStatus = %s\nwinampStatus = %s\n, newStatus = %s", userStatus.c_str(), winampStatus.c_str(), gs->newStatus.curDescription);

		if(gs->newStatus.curDescription == 0)
		{
			userStatus.clear();
		}

		if(wcsncmp(gs->newStatus.curDescription, winampStatus.c_str(), winampStatus.size()) != 0 || winampStatus.empty() || active == false)
		{			
			userStatus = gs->newStatus.curDescription;
		}
	}

	void onMessageReplace(wtwBmpStruct* rs)
	{
		if(wcscmp(rs->message.msgMessage, L"/w") == 0 || wcscmp(rs->message.msgMessage, L"/winamp") == 0) 
		{
			HWND wnd = FindWindowW(L"Winamp v1.x", NULL);

			if(wnd)
			{
				std::wstring tmp = getSongTitle(wnd);
				rs->fnReplaceMsg(tmp.c_str(), rs);
			}
		}
	}

	void setUserStatus()
	{
		wtwPresenceDef p;
		initStruct(p);
		wtw->fnCall(WTW_PF_WTW_STATUS_GET, (WTW_PARAM)&p, 0);
		p.curDescription = userStatus.c_str();
		wtw->fnCall(WTW_PF_WTW_STATUS_SET, (WTW_PARAM)&p, 0);
	}

	HANDLE descChangeHook;
	HANDLE msgProcessHook;

	bool active;
	std::wstring winampStatus;
	std::wstring currentTitle;
	std::wstring userStatus;
};

StatusManager* sm = 0;

WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*) 
{
	bool active = sm->changeState();
	{
		wtwMenuItemDef* pMi = (wtwMenuItemDef*)wParam;
		if(pMi)
		{
			wtwMenuItemDef mi = *pMi;
			mi.flags |= WTW_MENU_MODIFY_FLAGS;

			if(active == false)
			{
				mi.flags &= ~WTW_MENU_ITEM_FLAG_CHECKED;
			}
			else
			{
				mi.flags |= WTW_MENU_ITEM_FLAG_CHECKED;
			}

			mi.menuID = WTW_MENU_ID_MAIN_OPT;
			wtw->fnCall(WTW_MENU_ITEM_MODIFY, reinterpret_cast<WTW_PARAM>(&mi), 0);
		}
	}

	{
		wtwTrayNotifyDef nt;
		initStruct(nt);
		nt.textMain = L"wtwWinampStatus";
		nt.textLower = active ? L"Ustawianie opisu włączone. Jeżeli Winamp aktualnie odtwarza jakiś utwór, jego nazwa zostanie umieszczona w Twoim opisie" : L"Ustawianie opisu wyłączone";
		nt.flags = WTW_TN_FLAG_HDR_MULTILINE | WTW_TN_FLAG_TXT_MULTILINE;

		wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), NULL);
	}

	return 0;
}

extern "C" 
{
	WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/) 
	{
		return &plugInfo;
	}

	int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f)
	{
		wtw = f;
		{
			wtwMyConfigFile pBuff;
			initStruct(pBuff);

			pBuff.bufferSize = MAX_PATH + 1;
			pBuff.pBuffer = new wchar_t[MAX_PATH + 1];

			wtw->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&pBuff), reinterpret_cast<WTW_PARAM>(ghInstance));

			if(wtw->fnCall(WTW_SETTINGS_INIT_EX, reinterpret_cast<WTW_PARAM>(pBuff.pBuffer), reinterpret_cast<WTW_PARAM>(&config)) != S_OK)
			{
				config = 0;
			}
			else
			{
				wtw->fnCall(WTW_SETTINGS_READ, (WTW_PARAM)config, NULL);
			}

			delete[] pBuff.pBuffer;
		}
		{
			wtwOptionPageDef def;
			initStruct(def);

			def.callback	= SettingsPage::callback;
			def.id			= L"wtwWinampStatus/Options";
			def.caption		= L"Winamp";
			def.parentId	= WTW_OPTIONS_GROUP_PLUGINS;

			wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(ghInstance), reinterpret_cast<WTW_PARAM>(&def));
		}
		{
			wtwMenuItemDef mi;
			initStruct(mi);

			mi.itemId		= L"wtwWinampStatus/Active";
			mi.menuID		= WTW_MENU_ID_MAIN_OPT;
			mi.callback		= MenuCallback;
			mi.hModule		= ghInstance;
			mi.menuCaption	= L"Odtwarzany utwór jako opis";

			wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
		}

		if(!sm)
		{
			sm = new StatusManager;
		}

		return 0;
	}

	int __stdcall pluginUnload(DWORD /*callReason*/)
	{
		wtw->fnCall(WTW_MENU_ITEM_CLEAR, reinterpret_cast<WTW_PARAM>(ghInstance), 0);
		wtw->fnCall(WTW_OPTION_PAGE_REMOVE_ALL, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

		if(sm)
		{
			delete sm;
			sm = 0;
		}

		wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), NULL);
		wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));

		return 0;
	}
} // extern "C"
